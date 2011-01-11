#include "ArchiveBinary.h"
#include "Object.h"
#include "Registry.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

#include "Foundation/SmartBuffer/SmartBuffer.h"
#include "Foundation/Container/Insert.h" 
#include "Foundation/Memory/Endian.h"

using Helium::Insert;
using namespace Helium;
using namespace Helium::Reflect; 

// version / feature management 
const uint32_t ArchiveBinary::CURRENT_VERSION = 7;

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
    Log::Debug(TXT("Opening file '%s'\n"), m_Path.c_str());
#endif

    Reflect::CharStreamPtr stream = new CharFileStream( m_Path, write ); 
    stream->SetByteOrder( m_ByteOrder );
    OpenStream( stream, write );
}

void ArchiveBinary::OpenStream( CharStream* stream, bool write )
{
    // save the mode here, so that we safely refer to it later.
    m_Mode = (write) ? ArchiveModes::Write : ArchiveModes::Read; 

    // open the stream, this is "our interface" 
    stream->Open(); 

    // Setup stream
    m_Stream = stream; 
}

void ArchiveBinary::Close()
{
    HELIUM_ASSERT( m_Stream );

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
    m_Stream->SetByteOrder( byteOrder );

    // read character encoding
    CharacterEncoding characterEncoding = CharacterEncodings::ASCII;
    uint8_t encodingByte;
    m_Stream->Read(&encodingByte);
    characterEncoding = (CharacterEncoding)encodingByte;
    if ( characterEncoding != CharacterEncodings::ASCII && characterEncoding != CharacterEncodings::UTF_16 )
    {
        throw Reflect::StreamException( TXT( "Input stream contains an unknown character encoding: %d\n" ), characterEncoding ); 
    }
    m_Stream->SetCharacterEncoding( characterEncoding );

    // read version
    m_Stream->Read(&m_Version);

    if (m_Version > CURRENT_VERSION)
    {
        throw Reflect::StreamException( TXT( "Input stream version is higher than what is supported (input: %d, current: %d)\n" ), m_Version, CURRENT_VERSION); 
    }

    // deserialize main file elements
    {
        REFLECT_SCOPE_TIMER( ("Read Objects") );
        Deserialize(m_Objects, ArchiveFlags::Status);
    }

    // invalidate the search type and abort flags so we process the append block
    const Class* searchClass = m_SearchClass;
    if ( m_SearchClass != NULL )
    {
        m_SearchClass = NULL;
        m_Skip = false;
    }

    // restore state, just in case someone wants to consume this after the fact
    m_SearchClass = searchClass;

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

    // write BOM
    uint16_t feff = 0xfeff;
    m_Stream->Write( &feff ); // byte order mark

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

    // write version
    HELIUM_ASSERT( m_Version == CURRENT_VERSION );
    m_Stream->Write(&m_Version); 

    // serialize main file elements
    {
        REFLECT_SCOPE_TIMER( ("Main Spool Write") );

        Serialize(m_Objects, ArchiveFlags::Status);
    }

    // do cleanup
    m_Stream->Flush();

    info.m_ArchiveState = ArchiveStates::Complete;
    e_Status.Raise( info );
}

void ArchiveBinary::Serialize(Object* object)
{
    // write the crc of the class of object (used to factory allocate an instance when reading)
    uint32_t classCrc = Helium::Crc32( object->GetClass()->m_Name );
    m_Stream->Write(&classCrc); 

    // stub out the length we are about to write
    uint32_t start_offset = (uint32_t)m_Stream->TellWrite();
    m_Stream->Write(&start_offset); 

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Get(stdout);
    Log::Debug( TXT( "Serializing %s\n" ), object->GetClass()->m_Name );
    m_Indent.Push();
#endif

    PreSerialize(object);

    object->PreSerialize();

    if (object->HasType(Reflect::GetType<Data>()))
    {
        Data* s = DangerousCast<Data>(object);

        s->Serialize(*this);
    }
    else
    {
        // push a new struct on the stack
        WriteFields data;
        data.m_Count = 0;
        data.m_CountOffset = m_Stream->TellWrite();
        m_FieldStack.push(data);
        {
            // stub out the number of fields we are about to write
            m_Stream->Write(&m_FieldStack.top().m_Count);

            // serialize each field of the object
            SerializeFields(object);

            // seek back and write our count
            m_Stream->SeekWrite(m_FieldStack.top().m_CountOffset, std::ios_base::beg);
            m_Stream->Write(&m_FieldStack.top().m_Count); 
        }
        m_FieldStack.pop();
    }

    object->PostSerialize();

    // compute amound written
    uint32_t end_offset = (uint32_t)m_Stream->TellWrite();
    uint32_t length = end_offset - start_offset;

    // seek back and write written amount at start offset
    m_Stream->SeekWrite(start_offset, std::ios_base::beg);
    m_Stream->Write(&length); 

    // seek back to the end of the stream
    m_Stream->SeekWrite(0, std::ios_base::end);

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Pop();
#endif
}

void ArchiveBinary::Serialize(const std::vector< ObjectPtr >& elements, uint32_t flags)
{
    int32_t size = (int32_t)elements.size();
    m_Stream->Write(&size); 

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Get(stdout);
    Log::Debug(TXT("Serializing %d elements\n"), elements.size());
    m_Indent.Push();
#endif

    std::vector< ObjectPtr >::const_iterator itr = elements.begin();
    std::vector< ObjectPtr >::const_iterator end = elements.end();
    for (int index = 0; itr != end; ++itr, ++index )
    {
        Serialize(*itr);

        if ( flags & ArchiveFlags::Status )
        {
            StatusInfo info( *this, ArchiveStates::ObjectProcessed );
            info.m_Progress = (int)(((float)(index) / (float)elements.size()) * 100.0f);
            e_Status.Raise( info );
        }
    }

    if ( flags & ArchiveFlags::Status )
    {
        StatusInfo info( *this, ArchiveStates::ObjectProcessed );
        info.m_Progress = 100;
        e_Status.Raise( info );
    }

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Pop();
#endif

    const int32_t terminator = -1;
    m_Stream->Write(&terminator); 
}

void ArchiveBinary::SerializeFields( Object* object )
{
    const Composite* composite = object->GetClass();

    std::stack< const Composite* > bases;
    for ( const Composite* current = composite; current != NULL; current = current->m_Base )
    {
        bases.push( current );
    }

    while ( !bases.empty() )
    {
        const Composite* current = bases.top();
        bases.pop();

        DynArray< Field >::ConstIterator itr = current->m_Fields.Begin();
        DynArray< Field >::ConstIterator end = current->m_Fields.End();
        for ( ; itr != end; ++itr )
        {
            const Field* field = &*itr;

            // check to see if we should serialize (will return non-null if we are gtg)
            DataPtr data = field->ShouldSerialize( object, &m_Cache );
            if ( data )
            {
                uint32_t fieldNameCrc = Crc32( field->m_Name );
                m_Stream->Write(&fieldNameCrc); 

#ifdef REFLECT_ARCHIVE_VERBOSE
                m_Indent.Get(stdout);
                Log::Debug(TXT("Serializing field %s (class %s)\n"), field->m_Name, field->m_DataClass->m_Name);
                m_Indent.Push();
#endif

                PreSerialize( object, field );
                Serialize( data );
                data->Disconnect();
                m_Cache.Free( data );

#ifdef REFLECT_ARCHIVE_VERBOSE
                m_Indent.Pop();
#endif

                // we wrote a field, so increment our count
                HELIUM_ASSERT(m_FieldStack.size() > 0);
                m_FieldStack.top().m_Count++;
            }
        }
    }

    const int32_t terminator = -1;
    m_Stream->Write(&terminator); 
}

ObjectPtr ArchiveBinary::Allocate()
{
    ObjectPtr object;

    // read type string
    uint32_t typeCrc = Helium::BeginCrc32();
    m_Stream->Read(&typeCrc); 
    const Class* type = Reflect::Registry::GetInstance()->GetClass( typeCrc );

    // read length info if we have it
    uint32_t length = 0;
    m_Stream->Read(&length);

    if (m_Skip)
    {
        // skip it, but account for already reading the length from the stream
        m_Stream->SeekRead(length - sizeof(uint32_t), std::ios_base::cur);
    }
    else
    {
        if (type)
        {
            // allocate instance by name
            m_Cache.Create( type, object );
        }

        // if we failed
        if (!object.ReferencesObject())
        {
            // skip it, but account for already reading the length from the stream
            m_Stream->SeekRead(length - sizeof(uint32_t), std::ios_base::cur);

            // if you see this, then data is being lost because:
            //  1 - a type was completely removed from the codebase
            //  2 - a type was not found because its type library is not registered
            Log::Debug( TXT( "Unable to create object of type %s, size %d, skipping...\n" ), type ? type->m_Name : TXT("Unknown"), length);
#pragma TODO("Support blind data")
        }
    }

    return object;
}

void ArchiveBinary::Deserialize(ObjectPtr& object)
{
    //
    // If we don't have an object allocated for deserialization, pull one from the stream
    //

    if (!object.ReferencesObject())
    {
        object = Allocate();
    }

    //
    // We should now have an instance (unless data was skipped)
    //

    if (object.ReferencesObject())
    {
#ifdef REFLECT_ARCHIVE_VERBOSE
        m_Indent.Get(stdout);
        Log::Debug(TXT("Deserializing %s\n"), object->GetClass()->m_Name, object->GetType());
        m_Indent.Push();
#endif

        object->PreDeserialize();

        if (object->HasType(Reflect::GetType<Data>()))
        {
            Data* s = DangerousCast<Data>(object);

            s->Deserialize(*this);
        }
        else
        {
            DeserializeFields(object);
        }

        if ( !TryObjectCallback( object, &Object::PostDeserialize ) )
        {
            object = NULL; // discard the object
        }

        if ( object )
        {
            PostDeserialize(object);
        }

#ifdef REFLECT_ARCHIVE_VERBOSE
        m_Indent.Pop();
#endif
    }
}

void ArchiveBinary::Deserialize(std::vector< ObjectPtr >& elements, uint32_t flags)
{
    uint32_t start_offset = (uint32_t)m_Stream->TellRead();

    int32_t element_count = -1;
    m_Stream->Read(&element_count); 

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Get(stdout);
    Log::Debug(TXT("Deserializing %d elements\n"), element_count);
    m_Indent.Push();
#endif

    if (element_count > 0)
    {
        for (int i=0; i<element_count && !m_Abort; i++)
        {
            ObjectPtr object;
            Deserialize(object);

            if (object.ReferencesObject())
            {
                if ( object->HasType( m_SearchClass ) )
                {
                    m_Skip = true;
                }

                if ( flags & ArchiveFlags::Status )
                {
                    uint32_t current = (uint32_t)m_Stream->TellRead();

                    StatusInfo info( *this, ArchiveStates::ObjectProcessed );
                    info.m_Progress = (int)(((float)(current - start_offset) / (float)m_Size) * 100.0f);
                    e_Status.Raise( info );

                    m_Abort |= info.m_Abort;
                }
            }

            if (object.ReferencesObject() || flags & ArchiveFlags::Sparse)
            {
                elements.push_back( object );
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
            throw Reflect::DataFormatException( TXT( "Unterminated object array block" ) );
        }
    }

    if ( flags & ArchiveFlags::Status )
    {
        StatusInfo info( *this, ArchiveStates::ObjectProcessed );
        info.m_Progress = 100;
        e_Status.Raise( info );
    }
}

void ArchiveBinary::DeserializeFields(Object* object)
{
    int32_t fieldCount = -1;
    m_Stream->Read(&fieldCount); 

    for (int i=0; i<fieldCount; i++)
    {
        uint32_t fieldNameCrc = BeginCrc32();
        m_Stream->Read( &fieldNameCrc );

        const Class* type = object->GetClass();
        HELIUM_ASSERT( type );
        const Field* field = type->FindFieldByName(fieldNameCrc);
        HELIUM_ASSERT( field );

#ifdef REFLECT_ARCHIVE_VERBOSE
        m_Indent.Get(stdout);
        Log::Debug(TXT("Deserializing field %s\n"), field->m_Name);
        m_Indent.Push();
#endif

        // our missing component
        ObjectPtr component;

        if ( field )
        {
            // pull and object and downcast to data
            DataPtr latent_data = ObjectCast<Data>( Allocate() );
            if (!latent_data.ReferencesObject())
            {
                // this should never happen, the type id read from the file is bogus
                throw Reflect::TypeInformationException( TXT( "Unknown data for field %s" ), field->m_Name );
#pragma TODO("Support blind data")
            }

            // if the types match we are a natural fit to just deserialize directly into the field data
            if ( field->m_DataClass == field->m_DataClass )
            {
                // set data pointer
                latent_data->ConnectField( object, field );

                // process natively
                Deserialize( (ObjectPtr&)latent_data );

                // post process
                PostDeserialize( object, field );

                // disconnect
                latent_data->Disconnect();
            }
            else // else the type does not match, deserialize it into temp data then attempt to cast it into the field data
            {
                REFLECT_SCOPE_TIMER(("Casting"));

                // construct current serialization object
                ObjectPtr current_element;
                m_Cache.Create( field->m_DataClass, current_element );

                // downcast to data
                DataPtr current_data = ObjectCast<Data>(current_element);
                if (!current_data.ReferencesObject())
                {
                    // this should never happen, the type id in the rtti data is bogus
                    throw Reflect::TypeInformationException( TXT( "Invalid type id for field %s" ), field->m_Name );
                }

                // process into temporary memory
                current_data->ConnectField(object, field);

                // process natively
                Deserialize( (ObjectPtr&)latent_data );

                // attempt cast data into new definition
                if ( !Data::CastValue( latent_data, current_data, DataFlags::Shallow ) )
                {
                    // to the component block!
                    component = latent_data;
                }
                else
                {
                    // post process
                    PostDeserialize( object, field );
                }

                // disconnect
                current_data->Disconnect();
            }
        }
        else // else the field does not exist in the current class anymore
        {
            try
            {
                Deserialize( component );
            }
            catch (Reflect::LogisticException& ex)
            {
                Log::Debug( TXT( "Unable to deserialize %s::%s into component (%s), discarding\n" ), type->m_Name, field->m_Name, ex.What());
            }
        }

        if ( component.ReferencesObject() )
        {
            // attempt processing
            if (!object->ProcessComponent(component, field->m_Name))
            {
                Log::Debug( TXT( "%s did not process %s, discarding\n" ), object->GetClass()->m_Name, component->GetClass()->m_Name );
            }
        }

#ifdef REFLECT_ARCHIVE_VERBOSE
        m_Indent.Pop();
#endif
    }

    int32_t terminator = -1;
    m_Stream->Read(&terminator); 
    if (terminator != -1)
    {
        throw Reflect::DataFormatException( TXT( "Unterminated field array block" ) );
    }
}

void ArchiveBinary::ToStream( Object* object, std::iostream& stream )
{
    std::vector< ObjectPtr > elements(1);
    elements[0] = object;
    ToStream( elements, stream );
}

ObjectPtr ArchiveBinary::FromStream( std::iostream& stream, const Class* searchClass )
{
    if ( searchClass == NULL )
    {
        searchClass = Reflect::GetClass<Object>();
    }

    ArchiveBinary archive;
    archive.m_SearchClass = searchClass;

    Reflect::CharStreamPtr charStream = new CharStream( &stream, false ); 
    archive.OpenStream( charStream, false );
    archive.Read();
    archive.Close(); 

    std::vector< ObjectPtr >::iterator itr = archive.m_Objects.begin();
    std::vector< ObjectPtr >::iterator end = archive.m_Objects.end();
    for ( ; itr != end; ++itr )
    {
        if ((*itr)->HasType(searchClass))
        {
            return *itr;
        }
    }

    return NULL;
}

void ArchiveBinary::ToStream( const std::vector< ObjectPtr >& elements, std::iostream& stream )
{
    ArchiveBinary archive;

    // fix the spool
    archive.m_Objects = elements;

    Reflect::CharStreamPtr charStream = new CharStream( &stream, false ); 
    archive.OpenStream( charStream, true );
    archive.Write();   
    archive.Close(); 
}

void ArchiveBinary::FromStream( std::iostream& stream, std::vector< ObjectPtr >& elements )
{
    ArchiveBinary archive;

    Reflect::CharStreamPtr charStream = new CharStream( &stream, false ); 
    archive.OpenStream( charStream, false );
    archive.Read();
    archive.Close(); 

    elements = archive.m_Objects;
}
