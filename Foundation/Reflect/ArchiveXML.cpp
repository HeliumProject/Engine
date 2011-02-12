#include "Foundation/Reflect/ArchiveXML.h"

#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Structure.h"
#include "Foundation/Reflect/Data/DataDeduction.h"
#include "Foundation/Log.h"

#include <strstream>
#include <expat.h>

using namespace Helium;
using namespace Helium::Reflect;

const uint32_t ArchiveXML::CURRENT_VERSION = 4;

class StlVectorPusher : NonCopyable
{
public:
    std::vector< ObjectPtr >& m_ObjectVector;

    explicit StlVectorPusher( std::vector< ObjectPtr >& objectVector )
        : m_ObjectVector( objectVector )
    {
    }

    void operator()( const ObjectPtr& object )
    {
        m_ObjectVector.push_back( object );
    }
};

class DynArrayPusher : NonCopyable
{
public:
    DynArray< ObjectPtr >& m_ObjectArray;

    explicit DynArrayPusher( DynArray< ObjectPtr >& objectArray )
        : m_ObjectArray( objectArray )
    {
    }

    void operator()( const ObjectPtr& object )
    {
        m_ObjectArray.Push( object );
    }
};

ArchiveXML::ArchiveXML( const Path& path, ByteOrder byteOrder )
: Archive( path, byteOrder )
, m_Version( CURRENT_VERSION )
, m_Size( 0 )
, m_Skip( false )
, m_Body( NULL )
{

}

ArchiveXML::ArchiveXML()
: Archive()
, m_Version( CURRENT_VERSION )
, m_Size( 0 )
, m_Skip( false )
, m_Body( NULL )
{

}

ArchiveXML::~ArchiveXML()
{

}

void ArchiveXML::Open( bool write )
{
#ifdef REFLECT_ARCHIVE_VERBOSE
    Log::Debug(TXT("Opening file '%s'\n"), m_Path.c_str());
#endif

    Reflect::TCharStreamPtr stream = new TCharFileStream( m_Path, write );
    OpenStream( stream, write );
}

void ArchiveXML::OpenStream( TCharStream* stream, bool write )
{
    // save the mode here, so that we safely refer to it later.
    m_Mode = (write) ? ArchiveModes::Write : ArchiveModes::Read; 

    // open the stream, this is "our interface" 
    stream->Open(); 

    // Set precision
    stream->SetPrecision(32);

    // Setup stream
    m_Stream = stream; 
}

void ArchiveXML::Close()
{
    HELIUM_ASSERT( m_Stream );

    m_Stream->Close(); 
    m_Stream = NULL; 
}

void ArchiveXML::Read()
{
    REFLECT_SCOPE_TIMER(( "Reflect - XML Read" ));

    ArchiveStatus info( *this, ArchiveStates::Starting );
    e_Status.Raise( info );

    m_Abort = false;

    // determine the size of the input stream
    m_Stream->SeekRead(0, std::ios_base::end);
    m_Size = m_Stream->TellRead();
    m_Stream->SeekRead(0, std::ios_base::beg);

    // fail on an empty input stream
    if ( m_Size == 0 )
    {
        throw Reflect::StreamException( TXT( "Input stream is empty" ) );
    }

    // while there is data, parse buffer
    {
        REFLECT_SCOPE_TIMER( ("Parse XML") );

        long step = 0;
        const unsigned bufferSizeInBytes = 4096;
        char* buffer = static_cast< char* >( alloca( bufferSizeInBytes ) );
        while (!m_Stream->Fail() && !m_Abort)
        {
            m_Progress = (int)(((float)(step++ * bufferSizeInBytes) / (float)m_Size) * 100.0f);

            // divide by the character size so wide char builds don't override the allocation
            //  stream objects read characters, not byte-by-byte
            m_Stream->ReadBuffer(buffer, bufferSizeInBytes / sizeof(tchar_t));
            int bytesRead = static_cast<int>(m_Stream->ElementsRead());

            m_Document.ParseBuffer(buffer, bytesRead * sizeof(tchar_t), bytesRead == 0);
        }
    }

    m_Iterator.SetCurrent( m_Document.GetRoot() );

    // read file format version attribute
    const String* version = m_Iterator.GetCurrent()->GetAttributeValue( Name( TXT( "FileFormatVersion" ) ) );
    if ( version )
    {
        tstringstream str ( version->GetData() );
        str >> m_Version;
    }

    m_Iterator.Advance();

    // deserialize main file objects
    {
        REFLECT_SCOPE_TIMER( ("Read Objects") );
        Deserialize(m_Objects, ArchiveFlags::Status);
    }

    info.m_State = ArchiveStates::ObjectProcessed;
    info.m_Progress = 100;
    e_Status.Raise( info );

    info.m_State = ArchiveStates::Complete;
    e_Status.Raise( info );
}

void ArchiveXML::Write()
{
    REFLECT_SCOPE_TIMER(( "Reflect - XML Write" ));

    ArchiveStatus info( *this, ArchiveStates::Starting );
    e_Status.Raise( info );

#ifdef UNICODE
    uint16_t feff = 0xfeff;
    m_Stream->Write( &feff ); // byte order mark
#endif

    *m_Stream << TXT( "<?xml version=\"1.0\" encoding=\"" ) << Helium::GetEncoding() << TXT( "\"?>\n" );
    *m_Stream << TXT( "<Reflect FileFormatVersion=\"" ) << m_Version << TXT( "\">\n" );

    // serialize main file objects
    {
        REFLECT_SCOPE_TIMER( ("Write Objects") );
        Serialize(m_Objects, ArchiveFlags::Status);
    }

    *m_Stream << TXT( "</Reflect>\n\0" );

    info.m_State = ArchiveStates::Complete;
    e_Status.Raise( info );
}

void ArchiveXML::Serialize( Object* object )
{
    Serialize( object, NULL );
}

void ArchiveXML::Serialize( void* structure, const Structure* type )
{
    Serialize( structure, type, NULL );
}

void ArchiveXML::Serialize(const std::vector< ObjectPtr >& objects, uint32_t flags)
{
    Serialize( objects.begin(), objects.end(), flags );
}

void ArchiveXML::Serialize( const DynArray< ObjectPtr >& objects, uint32_t flags )
{
    Serialize( objects.Begin(), objects.End(), flags );
}

void ArchiveXML::Serialize(Object* object, const tchar_t* fieldName)
{
    if ( object )
    {
        object->PreSerialize( NULL );
    }

    m_Indent.Push();
    m_Indent.Get( *m_Stream );
    *m_Stream << TXT( "<Object Type=\"" );
    if ( object )
    {
        *m_Stream << object->GetClass()->m_Name;
    }
    
    *m_Stream << TXT( "\"" );

    if ( fieldName )
    {
        // our link back to the field we are nested in
        *m_Stream << TXT( " Name=\"" ) << fieldName << TXT( "\"" );
    }

    if ( !object || object->IsCompact() )
    {
        *m_Stream << TXT( ">" );
    }
    else
    {
        *m_Stream << TXT( ">\n" );
    }

    if ( object )
    {
        Data* data = SafeCast<Data>(object);

        if ( data )
        {
            data->Serialize(*this);
        }
        else
        {
            SerializeFields(object);
        }
    }

    if ( object && !object->IsCompact() )
    {
        m_Indent.Get(*m_Stream);
    }

    *m_Stream << TXT( "</Object>\n" );

    m_Indent.Pop();

    if ( object )
    {
        object->PostSerialize( NULL );
    }
}

void ArchiveXML::Serialize( void* structure, const Structure* type, const tchar_t* fieldName )
{
    m_Indent.Push();
    m_Indent.Get( *m_Stream );
    *m_Stream << TXT( "<Object Type=\"" );
    *m_Stream << type->m_Name;
    *m_Stream << TXT( "\"" );

    if ( fieldName )
    {
        // our link back to the field we are nested in
        *m_Stream << TXT( " Name=\"" ) << fieldName << TXT( "\"" );
    }

    *m_Stream << TXT( ">\n" );

    SerializeFields(structure, type);

    m_Indent.Get(*m_Stream);
    *m_Stream << TXT( "</Object>\n" );

    m_Indent.Pop();
}

template< typename ConstIteratorType >
void ArchiveXML::Serialize( ConstIteratorType begin, ConstIteratorType end, uint32_t flags )
{
    size_t size = static_cast< size_t >( end - begin );

    ConstIteratorType itr = begin;
    for (int index = 0; itr != end; ++itr, ++index )
    {
        Serialize(*itr, NULL);

        if ( flags & ArchiveFlags::Status )
        {
            ArchiveStatus info( *this, ArchiveStates::ObjectProcessed );
            info.m_Progress = (int)(((float)(index) / (float)size) * 100.0f);
            e_Status.Raise( info );
        }
    }

    if ( flags & ArchiveFlags::Status )
    {
        ArchiveStatus info( *this, ArchiveStates::ObjectProcessed );
        info.m_Progress = 100;
        e_Status.Raise( info );
    }
}

void ArchiveXML::SerializeFields( Object* object )
{
    const Class* type = object->GetClass();
    HELIUM_ASSERT(type != NULL);

    DynArray< Field >::ConstIterator itr = type->m_Fields.Begin();
    DynArray< Field >::ConstIterator end = type->m_Fields.End();
    for ( ; itr != end; ++itr )
    {
        const Field* field = &*itr;
        DataPtr data = object->ShouldSerialize( field );
        if ( data )
        {
            object->PreSerialize( field );
            Serialize( data, field->m_Name );
            object->PostSerialize( field );

            // might be useful to cache the data object here
            data->Disconnect();
        }
    }
}

void ArchiveXML::SerializeFields( void* structure, const Structure* type )
{
    DynArray< Field >::ConstIterator itr = type->m_Fields.Begin();
    DynArray< Field >::ConstIterator end = type->m_Fields.End();
    for ( ; itr != end; ++itr )
    {
        const Field* field = &*itr;
        DataPtr data = field->ShouldSerialize( structure );
        if ( data )
        {
            Serialize( data, field->m_Name );

            // might be useful to cache the data object here
            data->Disconnect();
        }
    }
}

void ArchiveXML::Deserialize(ObjectPtr& object)
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
        Log::Debug(TXT("Deserializing %s\n"), object->GetClass()->m_Name);
        m_Indent.Push();
#endif

        object->PreDeserialize( NULL );

        Data* data = SafeCast<Data>(object);

        if ( data )
        {
#pragma TODO("Make sure this string copy goes away when replace the stl stream APIs")
            tstringstream stringStream ( m_Iterator.GetCurrent()->m_Body.GetData() );
            TCharStream stream ( &stringStream, false );
            m_Body = &stream;

            data->Deserialize(*this);
            m_Body = NULL;

            m_Iterator.Advance();
        }
        else
        {
            DeserializeFields(object);
        }

        object->PostDeserialize( NULL );

#ifdef REFLECT_ARCHIVE_VERBOSE
        m_Indent.Pop();
#endif
    }
}

void ArchiveXML::Deserialize( void* structure, const Structure* type )
{
#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Get(stdout);
    Log::Debug(TXT("Deserializing %s\n"), type->m_Name);
    m_Indent.Push();
#endif

    DeserializeFields(structure, type);

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Pop();
#endif
}

void ArchiveXML::Deserialize( std::vector< ObjectPtr >& objects, uint32_t flags )
{
    Deserialize( StlVectorPusher( objects ), flags );
}

void ArchiveXML::Deserialize( DynArray< ObjectPtr >& objects, uint32_t flags )
{
    Deserialize( DynArrayPusher( objects ), flags );
}

ObjectPtr ArchiveXML::Allocate()
{
    ObjectPtr object;

    // find type
    const String* typeStr = m_Iterator.GetCurrent()->GetAttributeValue( Name( TXT("Type") ) );
    uint32_t typeCrc = typeStr ? Crc32( typeStr->GetData() ) : 0x0;

    // A null type name CRC indicates that a null reference was serialized, so no type lookup needs to be performed.
    const Class* type = NULL;
    if ( typeCrc != 0 )
    {
        type = Reflect::Registry::GetInstance()->GetClass( typeCrc );
    }

    if (type)
    {
        // allocate instance by name
        object = Registry::GetInstance()->CreateInstance( type );
    }

    // if we failed
    if (!object.ReferencesObject())
    {
        // if you see this, then data is being lost because:
        //  1 - a type was completely removed from the codebase
        //  2 - a type was not found because its type library is not registered
        Log::Debug( TXT( "Unable to create object of type %s, skipping...\n" ), type ? type->m_Name : TXT("Unknown") );

        // skip past this object, skipping our children
        m_Iterator.Advance( true );
#pragma TODO("Support blind data")
    }

    return object;
}

template< typename ArrayPusher >
void ArchiveXML::Deserialize( ArrayPusher& push, uint32_t flags )
{
#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Get(stdout);
    Log::Debug(TXT("Deserializing objects\n"));
    m_Indent.Push();
#endif

    for ( XMLElement* sibling = m_Iterator.GetCurrent(); sibling != NULL; sibling = sibling->GetNextSibling() )
    {
        HELIUM_ASSERT( m_Iterator.GetCurrent() == sibling );

        ObjectPtr object;
        Deserialize(object);

        if (object.ReferencesObject())
        {
            if ( object->IsClass( m_SearchClass ) )
            {
                m_Skip = true;
            }

            if ( flags & ArchiveFlags::Status )
            {
                ArchiveStatus info( *this, ArchiveStates::ObjectProcessed );
#pragma TODO("Update progress value for inter-array processing")
                //info.m_Progress = (int)(((float)(current - start_offset) / (float)m_Size) * 100.0f);
                e_Status.Raise( info );

                m_Abort |= info.m_Abort;
            }
        }

        push( object );
    }

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Pop();
#endif

    if ( flags & ArchiveFlags::Status )
    {
        ArchiveStatus info( *this, ArchiveStates::ObjectProcessed );
        info.m_Progress = 100;
        e_Status.Raise( info );
    }
}

void ArchiveXML::DeserializeFields(Object* object)
{
    for ( XMLElement* sibling = m_Iterator.GetCurrent(); sibling != NULL; sibling = sibling->GetNextSibling() )
    {
        HELIUM_ASSERT( m_Iterator.GetCurrent() == sibling );

        const String* fieldNameStr = sibling->GetAttributeValue( Name( TXT("Field") ) );
        uint32_t fieldNameCrc = fieldNameStr ? Crc32( fieldNameStr->GetData() ) : 0x0;

        const Class* type = object->GetClass();
        HELIUM_ASSERT( type );

        ObjectPtr unknown;
        const Field* field = type->FindFieldByName(fieldNameCrc);
        if ( field )
        {
#ifdef REFLECT_ARCHIVE_VERBOSE
            m_Indent.Get(stdout);
            Log::Debug(TXT("Deserializing field %s\n"), field->m_Name);
            m_Indent.Push();
#endif

            // pull and object and downcast to data
            DataPtr latentData = SafeCast<Data>( Allocate() );
            if (!latentData.ReferencesObject())
            {
                // this should never happen, the type id read from the file is bogus
                throw Reflect::TypeInformationException( TXT( "Unknown data for field %s (%s)" ), field->m_Name, m_Path.c_str() );
#pragma TODO("Support blind data")
            }

            // if the types match we are a natural fit to just deserialize directly into the field data
            if ( field->m_DataClass == field->m_DataClass )
            {
                // set data pointer
                latentData->ConnectField( object, field );

                // process natively
                object->PreDeserialize( field );
                Deserialize( (ObjectPtr&)latentData );
                object->PostDeserialize( field );

                // disconnect
                latentData->Disconnect();
            }
            else // else the type does not match, deserialize it into temp data then attempt to cast it into the field data
            {
                REFLECT_SCOPE_TIMER(("Casting"));

                // construct current serialization object
                ObjectPtr currentObject = Registry::GetInstance()->CreateInstance( field->m_DataClass );

                // downcast to data
                DataPtr currentData = SafeCast<Data>(currentObject);
                if (!currentData.ReferencesObject())
                {
                    // this should never happen, the type id in the rtti data is bogus
                    throw Reflect::TypeInformationException( TXT( "Invalid type id for field %s (%s)" ), field->m_Name, m_Path.c_str() );
                }

                // process into temporary memory
                currentData->ConnectField(object, field);

                // process natively
                object->PreDeserialize( field );
                Deserialize( (ObjectPtr&)latentData );

                // attempt cast data into new definition
                if ( !Data::CastValue( latentData, currentData, DataFlags::Shallow ) )
                {
                    // handle as unknown
                    unknown = latentData;
                }
                else
                {
                    // post process
                    object->PostDeserialize( field );
                }

                // disconnect
                currentData->Disconnect();
            }
        }
        else // else the field does not exist in the current class anymore
        {
            try
            {
                Deserialize( unknown );
            }
            catch (Reflect::LogisticException& ex)
            {
                Log::Debug( TXT( "Unable to deserialize %s::%s, discarding: %s\n" ), type->m_Name, field->m_Name, ex.What());
            }
        }

        if ( unknown.ReferencesObject() )
        {
            // attempt processing
            object->ProcessUnknown( unknown, field ? Crc32( field->m_Name ) : 0 );
        }

#ifdef REFLECT_ARCHIVE_VERBOSE
        m_Indent.Pop();
#endif
    }
}

void ArchiveXML::DeserializeFields( void* structure, const Structure* type )
{
    for ( XMLElement* sibling = m_Iterator.GetCurrent(); sibling != NULL; sibling = sibling->GetNextSibling() )
    {
        HELIUM_ASSERT( m_Iterator.GetCurrent() == sibling );

        const String* fieldNameStr = sibling->GetAttributeValue( Name( TXT("Field") ) );
        uint32_t fieldNameCrc = fieldNameStr ? Crc32( fieldNameStr->GetData() ) : 0x0;

        const Field* field = type->FindFieldByName(fieldNameCrc);
        if ( field )
        {
#ifdef REFLECT_ARCHIVE_VERBOSE
            m_Indent.Get(stdout);
            Log::Debug(TXT("Deserializing field %s\n"), field->m_Name);
            m_Indent.Push();
#endif

            // pull and structure and downcast to data
            DataPtr latentData = SafeCast<Data>( Allocate() );
            if (!latentData.ReferencesObject())
            {
                // this should never happen, the type id read from the file is bogus
                throw Reflect::TypeInformationException( TXT( "Unknown data for field %s (%s)" ), field->m_Name, m_Path.c_str() );
#pragma TODO("Support blind data")
            }

            // if the types match we are a natural fit to just deserialize directly into the field data
            if ( field->m_DataClass == field->m_DataClass )
            {
                // set data pointer
                latentData->ConnectField( structure, field );

                // process natively
                Deserialize( (ObjectPtr&)latentData );

                // disconnect
                latentData->Disconnect();
            }
            else // else the type does not match, deserialize it into temp data then attempt to cast it into the field data
            {
                REFLECT_SCOPE_TIMER(("Casting"));

                // construct current serialization structure
                ObjectPtr currentObject = Registry::GetInstance()->CreateInstance( field->m_DataClass );

                // downcast to data
                DataPtr currentData = SafeCast<Data>(currentObject);
                if (!currentData.ReferencesObject())
                {
                    // this should never happen, the type id in the rtti data is bogus
                    throw Reflect::TypeInformationException( TXT( "Invalid type id for field %s (%s)" ), field->m_Name, m_Path.c_str() );
                }

                // process into temporary memory
                currentData->ConnectField(structure, field);

                // process natively
                Deserialize( (ObjectPtr&)latentData );

                // attempt cast data into new definition
                Data::CastValue( latentData, currentData, DataFlags::Shallow );

                // disconnect
                currentData->Disconnect();
            }
        }

#ifdef REFLECT_ARCHIVE_VERBOSE
        m_Indent.Pop();
#endif
    }
}

void ArchiveXML::ToString( Object* object, tstring& xml )
{
    std::vector< ObjectPtr > objects(1);
    objects[0] = object;
    return ToString( objects, xml );
}

ObjectPtr ArchiveXML::FromString( const tstring& xml, const Class* searchClass )
{
    if ( searchClass == NULL )
    {
        searchClass = Reflect::GetClass<Object>();
    }

    ArchiveXML archive;
    archive.m_SearchClass = searchClass;

    tstringstream strStream;
    strStream << xml;
    archive.m_Stream = new Reflect::TCharStream(&strStream, false); 
    archive.Read();

    std::vector< ObjectPtr >::iterator itr = archive.m_Objects.begin();
    std::vector< ObjectPtr >::iterator end = archive.m_Objects.end();
    for ( ; itr != end; ++itr )
    {
        if ((*itr)->IsClass(searchClass))
        {
            return *itr;
        }
    }

    return NULL;
}

void ArchiveXML::ToString( const std::vector< ObjectPtr >& objects, tstring& xml )
{
    ArchiveXML archive;
    tstringstream strStream;

    archive.m_Stream = new Reflect::TCharStream( &strStream, false ); 
    archive.m_Objects = objects;
    archive.Write();

    xml = strStream.str();
}

void ArchiveXML::FromString( const tstring& xml, std::vector< ObjectPtr >& objects )
{
    ArchiveXML archive;
    tstringstream strStream;
    strStream << xml;

    archive.m_Stream = new Reflect::TCharStream( &strStream, false );
    archive.Read();

    objects = archive.m_Objects;
}
