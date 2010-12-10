#include "ArchiveBinary.h"
#include "Element.h"
#include "Registry.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

#include "Foundation/SmartBuffer/SmartBuffer.h"
#include "Foundation/Container/Insert.h" 
#include "Foundation/Checksum/CRC32.h"
#include "Foundation/Memory/Endian.h"

using Helium::Insert;
using namespace Helium;
using namespace Helium::Reflect; 

//#define REFLECT_DEBUG_BINARY_CRC
//#define REFLECT_DISABLE_BINARY_CRC

// version / feature management 
const uint32_t ArchiveBinary::CURRENT_VERSION                            = 7;

// our ORIGINAL version id was '!', don't ever re-use that byte
HELIUM_COMPILE_ASSERT( (ArchiveBinary::CURRENT_VERSION & 0xff) != 33 );

// CRC
const uint32_t CRC_DEFAULT = 0x10101010;
const uint32_t CRC_INVALID = 0xffffffff;

#ifdef REFLECT_DEBUG_BINARY_CRC
const uint32_t CRC_BLOCK_SIZE = 4;
#else
const uint32_t CRC_BLOCK_SIZE = 4096;
#endif

// this is sneaky, but in general people shouldn't use this
namespace Helium
{
    namespace Reflect
    {
        FOUNDATION_API bool g_OverrideCRC = false;
    }
}

//
// Binary Archive implements our own custom serialization technique
//

ArchiveBinary::ArchiveBinary( const Path& path, ByteOrder byteOrder )
: Archive( path, byteOrder )
, m_Version( CURRENT_VERSION )
, m_Size( 0 )
, m_Skip( false )
{
}

ArchiveBinary::ArchiveBinary()
: Archive()
, m_Version( CURRENT_VERSION )
, m_Size( 0 )
, m_Skip( false )
{
}

void ArchiveBinary::Open( bool write )
{
#ifdef REFLECT_ARCHIVE_VERBOSE
    Debug(TXT("Opening file '%s'\n"), path.c_str());
#endif

    Reflect::CharStreamPtr stream = new FileStream<char>( m_Path, write, m_ByteOrder ); 
    OpenStream( stream, write );
}

void ArchiveBinary::OpenStream( CharStream* stream, bool write )
{
    // save the mode here, so that we safely refer to it later.
    m_Mode = (write) ? ArchiveModes::Write : ArchiveModes::Read; 

    // open the stream, this is "our interface" 
    stream->Open(); 

    // Set precision
    stream->SetPrecision(32);

    // Setup stream
    m_Stream = stream; 

    // Header:
    if (write)
    {
        Start();
    }
}

void ArchiveBinary::Close()
{
    if (m_Mode == ArchiveModes::Write)
    {
        Finish(); 
    }

    m_Stream->Close(); 
    m_Stream = NULL; 
}

void ArchiveBinary::Read()
{
    REFLECT_SCOPE_TIMER( ("Reflect - Binary Read") );

    StatusInfo info( *this, ArchiveStates::Starting );
    e_Status.Raise( info );

    m_Abort = false;

    // determine the size of the input stream
    m_Stream->SeekRead(0, std::ios_base::end);
    m_Size = (long) m_Stream->TellRead();
    m_Stream->SeekRead(0, std::ios_base::beg);

    // fail on an empty input stream
    if ( m_Size == 0 )
    {
        throw Reflect::StreamException( TXT( "Input stream is empty" ) );
    }

    // setup visitors
    PreDeserialize();

    // read byte order
    ByteOrder byteOrder = Helium::PlatformByteOrder;
    uint16_t byteOrderMarker = 0;
    m_Stream->Read( &byteOrderMarker );
    switch ( byteOrderMarker )
    {
    case 0xfeff:
        byteOrder = Helium::PlatformByteOrder;
        break;
    case 0xfffe:
        switch ( Helium::PlatformByteOrder )
        {
        case ByteOrders::LittleEndian:
            byteOrder = ByteOrders::BigEndian;
            break;
        case ByteOrders::BigEndian:
            byteOrder = ByteOrders::LittleEndian;
            break;
        }
        break;
    default:
        throw Helium::Exception( TXT( "Unknown byte order read from file: %s" ), m_Path.c_str() );
    }

    // read version
    m_Stream->Read(&m_Version);

    if (m_Version > CURRENT_VERSION)
    {
        throw Reflect::StreamException( TXT( "Input stream version is higher than what is supported (input: %d, current: %d)\n" ), m_Version, CURRENT_VERSION); 
    }

    CharacterEncoding encoding = CharacterEncodings::ASCII;
    // character encoding
    uint8_t encodingByte;
    m_Stream->Read(&encodingByte);
    encoding = (CharacterEncoding)encodingByte;
    if ( encoding != CharacterEncodings::ASCII && encoding != CharacterEncodings::UTF_16 )
    {
        throw Reflect::StreamException( TXT( "Input stream contains an unknown character encoding: %d\n" ), encoding); 
    }

    // read and verify CRC
    uint32_t crc = CRC_DEFAULT;
    uint32_t current_crc = Helium::BeginCrc32();
    m_Stream->Read(&crc); 

#ifdef REFLECT_DISABLE_BINARY_CRC
    crc = CRC_DEFAULT;
#endif

    // if we are not the stub
    if (crc != CRC_DEFAULT)
    {
        REFLECT_SCOPE_TIMER( ("CRC Check") );

        PROFILE_SCOPE_ACCUM(g_ChecksumAccum);

        uint32_t count = 0;
        uint8_t block[CRC_BLOCK_SIZE];
        memset(block, 0, CRC_BLOCK_SIZE);

        // snapshot our starting location
        uint32_t start = (uint32_t)m_Stream->TellRead();

        // roll through file
        while (!m_Stream->Done())
        {
            // read block
            m_Stream->ReadBuffer(block, CRC_BLOCK_SIZE);

            // how much we got
            uint32_t got = (uint32_t) m_Stream->ElementsRead();

            // crc block
            current_crc = Helium::UpdateCrc32(current_crc, block, got);

#ifdef REFLECT_DEBUG_BINARY_CRC
            Log::Print("CRC %d (length %d) for datum 0x%08x is 0x%08x\n", count++, got, *(uint32_t*)block, current_crc);
#endif
        }

        // check result
        if (crc != current_crc && !g_OverrideCRC)
        {
            if (crc == CRC_INVALID)
            {
                throw Reflect::ChecksumException( TXT( "Corruption detected, file was not successfully written (incomplete CRC)" ), current_crc, crc );
            }
            else
            {
                throw Reflect::ChecksumException( TXT( "Corruption detected, crc is 0x%08x, should be 0x%08x" ), current_crc, crc);
            }
        }

        // clear error bits
        m_Stream->Clear();

        // seek back to past our crc data to start reading our valid file
        m_Stream->SeekRead(start, std::ios_base::beg);
    }

    // load some offsets
    uint32_t type_offset;
    m_Stream->Read(&type_offset); 
    uint32_t string_offset;
    m_Stream->Read(&string_offset);
    uint32_t element_offset = (uint32_t)m_Stream->TellRead();

    // deserialize string pool
    {
        REFLECT_SCOPE_TIMER( ("String Pool Read") );

        m_Stream->SeekRead(string_offset, std::ios_base::beg);

        // deserialize string table
        m_Strings.Deserialize(this, encoding); 
    }

    // deserialize type data
    {
        REFLECT_SCOPE_TIMER( ("Type Information Read") );

        m_Stream->SeekRead(type_offset, std::ios_base::beg);

        int32_t type_count = -1;
        m_Stream->Read(&type_count); 
        HELIUM_ASSERT(type_count >= 0);

#ifdef REFLECT_ARCHIVE_VERBOSE
        Debug(TXT("Deserializing %d types\n"), type_count);
#endif

        m_ClassesByShortName.clear();

        for (int i=0; i<type_count; i++)
        {
            ClassPtr c = Class::Create();

            DeserializeComposite(c);

            m_ClassesByShortName[ c->m_Name ] = c;
        }

        int32_t terminator = -1;
        m_Stream->Read(&terminator);

        if (terminator != -1)
        {
            throw Reflect::DataFormatException( TXT( "Error reading file, unterminated type information type block" ) );
        }
    }

    // seek back to start of element stream
    m_Stream->SeekRead(element_offset, std::ios_base::beg);

    // set m_Size to be the size of just the instance block (2 sections)
    m_Size = (long) (type_offset - element_offset); 

    // deserialize main file elements
    {
        REFLECT_SCOPE_TIMER( ("Main Spool Read") );

        Deserialize(m_Spool, ArchiveFlags::Status);
    }

    // invalidate the search type and abort flags so we process the append block
    const Class* searchClass = m_SearchClass;
    if ( m_SearchClass != NULL )
    {
        m_SearchClass = NULL;
        m_Skip = false;
    }

    std::vector< ElementPtr > append;

    // deserialize appended file elements
    {
        REFLECT_SCOPE_TIMER( ("Append Spool Read") );

        Deserialize(append);
    }

    // restore state, just in case someone wants to consume this after the fact
    m_SearchClass = searchClass;

    // tell visitors to process append
    PostDeserialize(append);

    info.m_ArchiveState = ArchiveStates::Complete;
    e_Status.Raise( info );
}

void ArchiveBinary::Write()
{
    REFLECT_SCOPE_TIMER( ("Reflect - Binary Write") );

    StatusInfo info( *this, ArchiveStates::Starting );
    e_Status.Raise( info );

    // setup visitors
    PreSerialize();

    // save character encoding value
    CharacterEncoding encoding;
#ifdef UNICODE
    encoding = CharacterEncodings::UTF_16;
    HELIUM_COMPILE_ASSERT( sizeof(wchar_t) == 2 );
#else
    encoding = CharacterEncodings::ASCII;
    HELIUM_COMPILE_ASSERT( sizeof(char) == 1 );
#endif
    uint8_t encodingByte = (uint8_t)encoding;
    m_Stream->Write(&encodingByte);

    // always start with the invalid crc, incase we don't make it to the end
    uint32_t crc = CRC_INVALID;

    // save the offset and write the invalid crc to the stream
    uint32_t crc_offset = (uint32_t)m_Stream->TellWrite();
    m_Stream->Write(&crc);

    // save some offsets to write offsets to
    uint32_t type_offset = (uint32_t)m_Stream->TellWrite();
    m_Stream->Write(&type_offset); 
    uint32_t string_offset = (uint32_t)m_Stream->TellWrite();
    m_Stream->Write(&string_offset);

    // serialize main file elements
    {
        REFLECT_SCOPE_TIMER( ("Main Spool Write") );

        Serialize(m_Spool, ArchiveFlags::Status);
    }

    // tell visitors to generate append
    std::vector< ElementPtr > append;
    PostSerialize(append);

    // serialize appended file elements
    {
        REFLECT_SCOPE_TIMER( ("Append Spool Write") );

        Serialize(append);
    }

    // serialize type data
    {
        REFLECT_SCOPE_TIMER( ("Type Information Write") );

        // write our current location back at our offset
        uint32_t type_location = (uint32_t)m_Stream->TellWrite();
        m_Stream->SeekWrite(type_offset, std::ios_base::beg);
        m_Stream->Write(&type_location);
        m_Stream->SeekWrite(0, std::ios_base::end);

        {
#ifdef REFLECT_ARCHIVE_VERBOSE
            Debug(TXT("Serializing %d types\n"), m_Classes.size());
#endif

            int32_t count = (int32_t)m_Classes.size();
            m_Stream->Write(&count); 

            std::set< const Class* >::iterator itr = m_Classes.begin();
            std::set< const Class* >::iterator end = m_Classes.end();
            for ( ; itr != end; ++itr )
            {
                SerializeComposite( *itr );
            }

            m_Classes.clear();
        }

        const static int32_t terminator = -1;
        m_Stream->Write(&terminator); 
    }

    // serialize string pool
    {
        REFLECT_SCOPE_TIMER( ("String Pool Write") );

        // write our current location back at our offset
        uint32_t string_location = (uint32_t)m_Stream->TellWrite();
        m_Stream->SeekWrite(string_offset, std::ios_base::beg);
        m_Stream->Write(&string_location); 
        m_Stream->SeekWrite(0, std::ios_base::end);

        // serialize string table
        m_Strings.Serialize(this); 
    }

    // CRC
    {
        REFLECT_SCOPE_TIMER( ("CRC Build") );

        uint32_t count = 0;
        uint8_t block[CRC_BLOCK_SIZE];
        memset(&block, 0, CRC_BLOCK_SIZE);

        // make damn sure this didn't change
        HELIUM_ASSERT(crc == CRC_INVALID);

        // reset this local back to default for computation
        crc = Helium::BeginCrc32();

        // seek to our starting point (after crc location)
        m_Stream->SeekRead(crc_offset + sizeof(crc), std::ios_base::beg);

        // roll through file
        while (!m_Stream->Done())
        {
            // read block
            m_Stream->ReadBuffer(block, CRC_BLOCK_SIZE);

            // how much we got
            uint32_t got = (uint32_t) m_Stream->ElementsRead();

            // crc block
            crc = Helium::UpdateCrc32(crc, block, got);

#ifdef REFLECT_DEBUG_BINARY_CRC
            Log::Print("CRC %d (length %d) for datum 0x%08x is 0x%08x\n", count++, got, *(uint32_t*)block, crc);
#endif
        }

        // clear errors
        m_Stream->Clear();

        // if we just so happened to hit the invalid crc, disable crc checking
        if (crc == CRC_INVALID)
        {
            crc = CRC_DEFAULT;
        }

        // seek back and write our crc data
        m_Stream->SeekWrite(crc_offset, std::ios_base::beg);
        HELIUM_ASSERT(!m_Stream->Fail());
        m_Stream->Write(&crc); 

    }

    // do cleanup
    m_Stream->SeekWrite(0, std::ios_base::end);
    m_Stream->Flush();

#ifdef REFLECT_DEBUG_BINARY_CRC
    Debug("File written with size %d, crc 0x%08x\n", m_Stream->TellWrite(), crc);
#endif

    info.m_ArchiveState = ArchiveStates::Complete;
    e_Status.Raise( info );
}

void ArchiveBinary::Start()
{
    uint16_t feff = 0xfeff;
    m_Stream->Write( &feff ); // byte order mark

    // just for good measure
    m_Version = CURRENT_VERSION;
    m_Stream->Write(&m_Version); 
}

void ArchiveBinary::Finish()
{

}

void ArchiveBinary::Serialize(const ElementPtr& element)
{
    REFLECT_SCOPE_TIMER_INST( ("Serialize %s", element->GetClass()->m_Name.c_str()) );

    // use the string pool index for this type's short name
    int32_t index = m_Strings.Insert(element->GetClass()->m_Name);
    m_Stream->Write(&index); 

    // get and stub out the start offset where we are now (will become length after writing is done)
    uint32_t start_offset = (uint32_t)m_Stream->TellWrite();
    m_Stream->Write(&start_offset); 

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Get(stdout);
    Debug(TXT("Serializing %s (type %d)\n"), element->GetClass()->m_Name.c_str(), element->GetType());
    m_Indent.Push();
#endif

    PreSerialize(element);

    {
        REFLECT_SCOPE_TIMER_INST( ("PreSerialize %s", element->GetClass()->m_Name.c_str()) );

        element->PreSerialize();
    }

    if (element->HasType(Reflect::GetType<Data>()))
    {
        Data* s = DangerousCast<Data>(element);

        s->Serialize(*this);
    }
    else
    {
        // push a new struct on the stack
        WriteFields data;
        data.m_Count = 0;
        data.m_CountOffset = m_Stream->TellWrite();
        m_FieldStack.push(data);

        // write some placeholder info
        m_Stream->Write(&m_FieldStack.top().m_Count);

        SerializeFields(element);

        // write our terminator
        const static int32_t terminator = -1;
        m_Stream->Write(&terminator); 

        // seek back and write our count
        HELIUM_ASSERT(m_FieldStack.size() > 0);
        m_Stream->SeekWrite(m_FieldStack.top().m_CountOffset, std::ios_base::beg);
        m_Stream->Write(&m_FieldStack.top().m_Count); 
        m_FieldStack.pop();

        // seek back to end
        m_Stream->SeekWrite(0, std::ios_base::end);
    }

    {
        REFLECT_SCOPE_TIMER_INST( ("PostSerialize %s", element->GetClass()->m_Name.c_str()) );

        element->PostSerialize();
    }

    // save our end offset to substract the start from
    uint32_t end_offset = (uint32_t)m_Stream->TellWrite();

    // seek back to the start offset
    m_Stream->SeekWrite(start_offset, std::ios_base::beg);

    // compute amound written
    uint32_t length = end_offset - start_offset;

    // write written amount at start offset
    m_Stream->Write(&length); 

    // seek back to the end of the stream
    m_Stream->SeekWrite(0, std::ios_base::end);

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Pop();
#endif
}

void ArchiveBinary::Serialize(const std::vector< ElementPtr >& elements, uint32_t flags)
{
    REFLECT_SCOPE_TIMER_INST( "" )

        int32_t size = (int32_t)elements.size();
    m_Stream->Write(&size); 

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Get(stdout);
    Debug(TXT("Serializing %d elements\n"), elements.size());
    m_Indent.Push();
#endif

    std::vector< ElementPtr >::const_iterator itr = elements.begin();
    std::vector< ElementPtr >::const_iterator end = elements.end();
    for (int index = 0; itr != end; ++itr, ++index )
    {
        Serialize(*itr);

        if ( flags & ArchiveFlags::Status )
        {
            StatusInfo info( *this, ArchiveStates::ElementProcessed );
            info.m_Progress = (int)(((float)(index) / (float)elements.size()) * 100.0f);
            e_Status.Raise( info );
        }
    }

    if ( flags & ArchiveFlags::Status )
    {
        StatusInfo info( *this, ArchiveStates::ElementProcessed );
        info.m_Progress = 100;
        e_Status.Raise( info );
    }

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Pop();
#endif

    const static int32_t terminator = -1;
    m_Stream->Write(&terminator); 
}

void ArchiveBinary::SerializeFields( const ElementPtr& element )
{
    //
    // Serialize fields
    //

    const Class* type = element->GetClass();
    HELIUM_ASSERT(type != NULL);

    REFLECT_SCOPE_TIMER_INST( "" );

    std::vector< ConstFieldPtr >::const_iterator itr = type->m_Fields.begin();
    std::vector< ConstFieldPtr >::const_iterator end = type->m_Fields.end();
    for ( ; itr != end; ++itr )
    {
        SerializeField(element, *itr);
    }
}

void ArchiveBinary::SerializeField(const ElementPtr& element, const Field* field)
{
    REFLECT_SCOPE_TIMER_INST( ("Serialize %s", field->m_Name.c_str()) );

    // don't write no write fields
    if ( field->m_Flags & FieldFlags::Discard )
    {
        return;
    }

    // construct serialization object
    ElementPtr e;
    m_Cache.Create( field->m_DataClass, e );

    HELIUM_ASSERT( e.ReferencesObject() );

    // downcast serializer
    DataPtr serializer = ObjectCast<Data>(e);

    if (!serializer.ReferencesObject())
    {
        // this should never happen, the type id in the rtti data is bogus
        throw Reflect::TypeInformationException( TXT( "Invalid type id for field '%s'" ), field->m_Name.c_str() );
    }

    // set data pointer
    serializer->ConnectField(element.Ptr(), field);

    // bool for test results
    bool serialize = true;

    // check for equality
    if ( serialize && field->m_Default.ReferencesObject() )
    {
        bool force = (field->m_Flags & FieldFlags::Force) != 0;
        if (!force && field->m_Default->Equals(serializer))
        {
            serialize = false;
        }
    }

    // don't write empty containers
    if ( serialize &&  e->HasType( Reflect::GetType<ContainerData>() ) )
    {
        ContainerDataPtr container = DangerousCast<ContainerData>(e);

        if ( container->GetSize() == 0 )
        {
            serialize = false;
        }
    }

    // last chance to not write, call through virtual API
    if (serialize)
    {
        PreSerialize(element, field);

        // write our latent field ID to the stream, this will always be valid since we persist ALL of the type information data
        m_Stream->Write(&field->m_Index); 

#ifdef REFLECT_ARCHIVE_VERBOSE
        m_Indent.Get(stdout);
        Debug(TXT("Serializing field %s (field id %d)\n"), field->m_Name.c_str(), field->m_Index);
        m_Indent.Push();
#endif

        // process
        Serialize( serializer );

#ifdef REFLECT_ARCHIVE_VERBOSE
        m_Indent.Pop();
#endif

        // we wrote a field, so increment our count
        HELIUM_ASSERT(m_FieldStack.size() > 0);
        m_FieldStack.top().m_Count++;
    }

    // disconnect
    serializer->Disconnect();
}

ElementPtr ArchiveBinary::Allocate()
{
    ElementPtr element;

    // read type string
    int32_t index = -1;
    m_Stream->Read(&index); 
    const tstring& str = m_Strings.Get(index);

    // read length info if we have it
    uint32_t length = 0;
    if (m_Version > 1)
    {
        m_Stream->Read(&length);

        if (m_Skip)
        {
            // skip it, but account for already reading the length from the stream
            m_Stream->SeekRead(length - sizeof(uint32_t), std::ios_base::cur);

            // we should just keep processing even though we return null
            return NULL;
        }
    }

    // find type by short name string
    M_StrToClass::iterator found = m_ClassesByShortName.find(str);
    if (found == m_ClassesByShortName.end())
    {
        // we failed to find a type in the latent type data, that is bad
        HELIUM_BREAK();
        throw Reflect::TypeInformationException( TXT( "Unable to locate type '%s'" ), str.c_str());
    }

    // this is guaranteed to be our legacy short name name
    const tstring& name (found->second->m_Name);

    // allocate instance by short name and remap the new and different short name to the legacy short name name for later lookup
    if (m_Cache.Create(name, element) && name != element->GetClass()->m_Name)
    {
        // map current short name name to LEGACY short name name so we can retrieve type information via a lookup later
        Insert<std::map< tstring, tstring >>::Result inserted = m_ShortNameMapping.insert( std::map< tstring, tstring >::value_type (element->GetClass()->m_Name, name) );

        // check for insanity
        if ( !inserted.second && inserted.first->second != name )
        {
            throw Reflect::TypeInformationException( TXT( "Overloaded name mapping for '%s', this is not supported" ), name.c_str());
        }
    }

    // if we failed
    if (!element.ReferencesObject())
    {
        if (m_Version > 1)
        {
            // skip it, but account for already reading the length from the stream
            m_Stream->SeekRead(length - sizeof(uint32_t), std::ios_base::cur);

            // if you see this, then data is being lost because:
            //  1 - a type was completely removed from the codebase
            //  2 - a type was not found because its type library is not registered
            Log::Debug( TXT( "Unable to create object of type '%s', size %d, skipping...\n" ), str.c_str(), length);
        }
        else
        {
            HELIUM_BREAK();
            throw Reflect::DataFormatException( TXT( "Unable to create object, unknown type '%s'" ), str.c_str());
        }
    }

    return element;
}

void ArchiveBinary::Deserialize(ElementPtr& element)
{
    //
    // If we don't have an object allocated for deserialization, pull one from the stream
    //

    if (!element.ReferencesObject())
    {
        element = Allocate();
    }

    //
    // We should now have an instance (unless data was skipped)
    //

    if (element.ReferencesObject())
    {
        REFLECT_SCOPE_TIMER_INST( ("Deserialize %s", element->GetClass()->m_Name.c_str()) );

#ifdef REFLECT_ARCHIVE_VERBOSE
        m_Indent.Get(stdout);
        Debug(TXT("Deserializing %s (type %d)\n"), element->GetClass()->m_Name.c_str(), element->GetType());
        m_Indent.Push();
#endif

        {
            REFLECT_SCOPE_TIMER_INST( ("PreDeserialize %s", element->GetClass()->m_Name.c_str()) );

            element->PreDeserialize();
        }

        if (element->HasType(Reflect::GetType<Data>()))
        {
            Data* s = DangerousCast<Data>(element);

            s->Deserialize(*this);
        }
        else
        {
            DeserializeFields(element);
        }

        {
            REFLECT_SCOPE_TIMER_INST( ("PostDeserialize %s", element->GetClass()->m_Name.c_str()) );

            if ( !TryElementCallback( element, &Element::PostDeserialize ) )
            {
                element = NULL; // discard the object
            }
        }

        if ( element )
        {
            PostDeserialize(element);
        }

#ifdef REFLECT_ARCHIVE_VERBOSE
        m_Indent.Pop();
#endif
    }
}

void ArchiveBinary::Deserialize(std::vector< ElementPtr >& elements, uint32_t flags)
{
    uint32_t start_offset = (uint32_t)m_Stream->TellRead();

    int32_t element_count = -1;
    m_Stream->Read(&element_count); 

    REFLECT_SCOPE_TIMER_INST( "" )

#ifdef REFLECT_ARCHIVE_VERBOSE
        m_Indent.Get(stdout);
    Debug(TXT("Deserializing %d elements\n"), element_count);
    m_Indent.Push();
#endif

    if (element_count > 0)
    {
        for (int i=0; i<element_count && !m_Abort; i++)
        {
            ElementPtr element;
            Deserialize(element);

            if (element.ReferencesObject())
            {
                if ( element->HasType( m_SearchClass ) )
                {
                    m_Skip = true;
                }

                if ( flags & ArchiveFlags::Status )
                {
                    uint32_t current = (uint32_t)m_Stream->TellRead();

                    StatusInfo info( *this, ArchiveStates::ElementProcessed );
                    info.m_Progress = (int)(((float)(current - start_offset) / (float)m_Size) * 100.0f);
                    e_Status.Raise( info );

                    m_Abort |= info.m_Abort;
                }
            }

            if (element.ReferencesObject() || flags & ArchiveFlags::Sparse)
            {
                elements.push_back(element);
            }
        }
    }

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Pop();
#endif

    if (!m_Abort)
    {
        int32_t terminator = -1;
        m_Stream->Read(&terminator);

        if (terminator != -1)
        {
            throw Reflect::DataFormatException( TXT( "Unterminated element array block" ) );
        }
    }

    if ( flags & ArchiveFlags::Status )
    {
        StatusInfo info( *this, ArchiveStates::ElementProcessed );
        info.m_Progress = 100;
        e_Status.Raise( info );
    }
}

void ArchiveBinary::DeserializeFields(const ElementPtr& element)
{
    int32_t field_count = -1;
    m_Stream->Read(&field_count); 

    REFLECT_SCOPE_TIMER_INST( "" )

        if (field_count > 0)
        {
            const Class* type = NULL;

            // find the type of this object
            M_StrToClass::iterator type_found = m_ClassesByShortName.find(element->GetClass()->m_Name);

            // get Element's type info
            if ( type_found != m_ClassesByShortName.end() )
            {
                type = type_found->second;
            }
            else
            {
                // our short name has changed so look up the legacy short name name given the short name of the current object
                std::map< tstring, tstring >::const_iterator name_found = m_ShortNameMapping.find( element->GetClass()->m_Name );

                // we should always find it, else its a bug/internal error
                if ( name_found == m_ShortNameMapping.end() )
                {
                    throw Reflect::TypeInformationException( TXT( "Unable to remap short name '%s'" ), element->GetClass()->m_Name.c_str());
                }

                // we throw if there is an internal error, so just dereference the result
                type = m_ClassesByShortName.find( name_found->second )->second;
            }

            if (type == NULL)
            {
                Log::Debug( TXT( "Unable to resolve type from short name '%s'\n" ), element->GetClass()->m_Name.c_str());
            }

            // while we haven't hit the terminator
            for (int i=0; i<field_count; i++)
            {
                int32_t field_id = -1;
                m_Stream->Read(&field_id); 

                if (type != NULL)
                {
                    const Field* field = type->FindFieldByIndex(field_id);
                    HELIUM_ASSERT(field);

#ifdef REFLECT_ARCHIVE_VERBOSE
                    m_Indent.Get(stdout);
                    Debug(TXT("Deserializing field %s (field id %d)\n"), field->m_Name.c_str(), field_id);
                    m_Indent.Push();
#endif

                    // process
                    DeserializeField(element, field);

#ifdef REFLECT_ARCHIVE_VERBOSE
                    m_Indent.Pop();
#endif
                }
            }
        }

        int32_t terminator = -1;
        m_Stream->Read(&terminator); 

        if (terminator != -1)
        {
            throw Reflect::DataFormatException( TXT( "Unterminated field array block" ) );
        }
}

void ArchiveBinary::DeserializeField(const ElementPtr& element, const Field* latent_field)
{
    REFLECT_SCOPE_TIMER_INST( ("Deserialize %s", latent_field->m_Name.c_str()) );

    // get the type info for the instance we are writing too
    const Class* type = element->GetClass();

    // the field to serialize
    const Field* current_field = type->FindFieldByName(latent_field->m_Name);

    // our missing component
    ElementPtr component;

    if ( current_field )
    {
        // pull and element and downcast to serializer
        DataPtr latent_serializer = ObjectCast<Data>( Allocate() );

        if (!latent_serializer.ReferencesObject())
        {
            // this should never happen, the type id read from the file is bogus
            throw Reflect::TypeInformationException( TXT( "Invalid type id for field '%s'" ), latent_field->m_Name.c_str() );
        }

        // keep in mind that m_DataClass of latent field is the current type id that matches the latent short name
        if (current_field->m_DataClass == latent_field->m_DataClass)
        {
            // set data pointer
            latent_serializer->ConnectField( element.Ptr(), current_field );

            // process natively
            Deserialize( (ElementPtr&)latent_serializer );

            // post process
            PostDeserialize( element, current_field );

            // disconnect
            latent_serializer->Disconnect();
        }
        else
        {
            REFLECT_SCOPE_TIMER(("Casting"));

            // construct current serialization object
            ElementPtr current_element;
            m_Cache.Create( current_field->m_DataClass, current_element );

            // downcast to serializer
            DataPtr current_serializer = ObjectCast<Data>(current_element);
            if (!current_serializer.ReferencesObject())
            {
                // this should never happen, the type id in the rtti data is bogus
                throw Reflect::TypeInformationException( TXT( "Invalid type id for field '%s'" ), current_field->m_Name.c_str() );
            }

            // process into temporary memory
            current_serializer->ConnectField(element.Ptr(), current_field);

            // process natively
            Deserialize( (ElementPtr&)latent_serializer );

            // attempt cast data into new definition
            if (!Data::CastValue( latent_serializer, current_serializer, DataFlags::Shallow ))
            {
                // to the component block!
                component = latent_serializer;
            }
            else
            {
                // post process
                PostDeserialize( element, current_field );
            }

            // disconnect
            current_serializer->Disconnect();
        }
    }
    else
    {
        try
        {
            // attempt to process our lost component natively
            Deserialize( component );
        }
        catch (Reflect::LogisticException& ex)
        {
            Log::Debug( TXT( "Unable to deserialize %s::%s into component (%s), discarding\n" ), type->m_Name.c_str(), latent_field->m_Name.c_str(), ex.What());
        }
    }

    if (component.ReferencesObject())
    {
        // attempt processing
        if (!element->ProcessComponent(component, latent_field->m_Name))
        {
            Log::Debug( TXT( "%s did not process %s, discarding\n" ), element->GetClass()->m_Name.c_str(), component->GetClass()->m_Name.c_str());
        }
    }
}

void ArchiveBinary::SerializeComposite(const Composite* composite)
{
#ifdef REFLECT_ARCHIVE_VERBOSE
    Log::Debug(TXT(" Serializing %s (%d fields)\n"), composite->m_Name.c_str(), composite->m_Fields.size());
#endif

    int32_t string_index = m_Strings.Insert(composite->m_Name);
    m_Stream->Write(&string_index); 

    int32_t field_count = (int32_t)composite->m_Fields.size();
    m_Stream->Write(&field_count);

    std::stack< const Composite* > bases;
    for ( const Composite* current = composite; current != NULL; current = current->m_Base )
    {
        bases.push( current );
    }

    for ( const Composite* current = bases.top(); !bases.empty(); bases.pop(), current = bases.top() )
    {
        std::vector< ConstFieldPtr >::const_iterator itr = current->m_Fields.begin();
        std::vector< ConstFieldPtr >::const_iterator end = current->m_Fields.end();
        for ( ; itr != end; ++itr )
        {
            SerializeField(*itr);
        }
    }

    const static int32_t terminator = -1;
    m_Stream->Write(&terminator); 
}

bool ArchiveBinary::DeserializeComposite(Composite* composite)
{
    int32_t string_index = -1;
    m_Stream->Read(&string_index); 
    composite->m_Name = m_Strings.Get(string_index);

    int32_t field_count = -1;
    m_Stream->Read(&field_count); 

#ifdef REFLECT_ARCHIVE_VERBOSE
    Log::Debug(TXT(" Deserializing %s (%d fields)\n"), composite->m_Name.c_str(), field_count);
#endif

    for ( int32_t i=0; i<field_count; ++i )
    {
        FieldPtr field = Field::Create(composite);
        field->m_Index = i;
        composite->m_Fields.push_back( field );

        if (!DeserializeField(field))
        {
            return false;
        }
    }

    int32_t terminator = -1;
    m_Stream->Read(&terminator); 

    if (terminator != -1)
    {
        throw Reflect::DataFormatException( TXT( "Error reading file, unterminated type field block" ) );
    }

    return !m_Stream->Fail();
}

void ArchiveBinary::SerializeField(const Field* field)
{
    // field name
    int32_t string_index = m_Strings.Insert(field->m_Name);
    m_Stream->Write(&string_index); 

    // field type id short name
    const Class* c = field->m_DataClass;
    if (c != NULL)
    {
        string_index = m_Strings.Insert(c->m_Name);
    }
    else
    {
        string_index = -1;
    }
    m_Stream->Write(&string_index); 

#ifdef REFLECT_ARCHIVE_VERBOSE
    Log::Debug(TXT("  Serializing %s (short name %s)\n"), c->m_Name.c_str(), c->m_Name.c_str());
#endif
}

bool ArchiveBinary::DeserializeField(Field* field)
{
    int32_t string_index = -1;

    // field name
    m_Stream->Read(&string_index); 
    field->m_Name = m_Strings.Get(string_index);

    // field type id short name
    m_Stream->Read(&string_index); 
    if (string_index >= 0)
    {
        const tstring& str (m_Strings.Get(string_index));

        const Class* c = Registry::GetInstance()->GetClass(str);

        if ( c )
        {
            field->m_DataClass = c;
        }
        else
        {
            field->m_DataClass = NULL;
        }

#ifdef REFLECT_ARCHIVE_VERBOSE
        Log::Debug(TXT("  Deserializing %s (short name %s)\n"), c->m_Name.c_str(), str.c_str());
#endif
    }

    return !m_Stream->Fail();
}

void ArchiveBinary::ToStream( const ElementPtr& element, std::iostream& stream )
{
    std::vector< ElementPtr > elements(1);
    elements[0] = element;
    ToStream( elements, stream );
}

ElementPtr ArchiveBinary::FromStream( std::iostream& stream, const Class* searchClass )
{
    if ( searchClass == NULL )
    {
        searchClass = Reflect::GetClass<Element>();
    }

    ArchiveBinary archive;
    archive.m_SearchClass = searchClass;

    Reflect::CharStreamPtr charStream = new Reflect::Stream<char>( &stream ); 
    archive.OpenStream( charStream, false );
    archive.Read();
    archive.Close(); 

    std::vector< ElementPtr >::iterator itr = archive.m_Spool.begin();
    std::vector< ElementPtr >::iterator end = archive.m_Spool.end();
    for ( ; itr != end; ++itr )
    {
        if ((*itr)->HasType(searchClass))
        {
            return *itr;
        }
    }

    return NULL;
}

void ArchiveBinary::ToStream( const std::vector< ElementPtr >& elements, std::iostream& stream )
{
    ArchiveBinary archive;

    // fix the spool
    archive.m_Spool = elements;

    Reflect::CharStreamPtr charStream = new Reflect::Stream<char>(&stream); 
    archive.OpenStream( charStream, true );
    archive.Write();   
    archive.Close(); 
}

void ArchiveBinary::FromStream( std::iostream& stream, std::vector< ElementPtr >& elements )
{
    ArchiveBinary archive;

    Reflect::CharStreamPtr charStream = new Reflect::Stream<char>(&stream); 
    archive.OpenStream( charStream, false );
    archive.Read();
    archive.Close(); 

    elements = archive.m_Spool;
}
