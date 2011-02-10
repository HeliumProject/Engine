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

// uncomment to display parse stack progress
//#define REFLECT_DISPLAY_PARSE_STACK

const uint32_t ArchiveXML::CURRENT_VERSION                               = 4;

ArchiveXML::ArchiveXML( const Path& path, ByteOrder byteOrder )
: Archive( path, byteOrder )
, m_Version( CURRENT_VERSION )
{

}

ArchiveXML::ArchiveXML()
: Archive()
, m_Version( CURRENT_VERSION )
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
    std::streamsize size = m_Stream->TellRead();
    m_Stream->SeekRead(0, std::ios_base::beg);

    // fail on an empty input stream
    if ( size == 0 )
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
            m_Progress = (int)(((float)(step++ * bufferSizeInBytes) / (float)size) * 100.0f);

            // divide by the character size so wide char builds don't override the allocation
            //  stream objects read characters, not byte-by-byte
            m_Stream->ReadBuffer(buffer, bufferSizeInBytes / sizeof(tchar_t));
            int bytesRead = static_cast<int>(m_Stream->ElementsRead());

            m_Document.ParseBuffer(buffer, bytesRead * sizeof(tchar_t), bytesRead == 0);
        }
    }

    m_Current = m_Document.GetRoot();

    // read file format version attribute
    const String* version = m_Current->GetAttributeValue( Name( TXT( "FileFormatVersion" ) ) );
    if ( version )
    {
        tstringstream str ( version->GetData() );
        str >> m_Version;
    }

    m_Current = m_Current->m_FirstChild;

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

void ArchiveXML::Serialize(Object* object)
{
    Serialize( object, NULL );
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

void ArchiveXML::Serialize( void* structure, const Structure* type )
{
    Serialize( structure, type, NULL );
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

void ArchiveXML::Serialize(const std::vector< ObjectPtr >& objects, uint32_t flags)
{
    Serialize( objects.begin(), objects.end(), flags );
}

void ArchiveXML::Serialize( const DynArray< ObjectPtr >& objects, uint32_t flags )
{
    Serialize( objects.Begin(), objects.End(), flags );
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

void ArchiveXML::SerializeFields(Object* object)
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

void ArchiveXML::Deserialize( ObjectPtr& object )
{

}

void ArchiveXML::Deserialize( void* structure, const Structure* type )
{

}

void ArchiveXML::Deserialize(std::vector< ObjectPtr >& objects, uint32_t flags)
{
    if (!m_Objects.empty())
    {
        if ( !(flags & ArchiveFlags::Sparse) )
        {
            m_Objects.erase( std::remove( m_Objects.begin(), m_Objects.end(), ObjectPtr () ), m_Objects.end() );
        }

        objects = m_Objects;

        m_Objects.clear();
    }
}

void ArchiveXML::Deserialize( DynArray< ObjectPtr >& objects, uint32_t flags )
{
    objects.Clear();

    if (!m_Objects.empty())
    {
        if ( !(flags & ArchiveFlags::Sparse) )
        {
            m_Objects.erase( std::remove( m_Objects.begin(), m_Objects.end(), ObjectPtr () ), m_Objects.end() );
        }

        size_t size = m_Objects.size();
        objects.Reserve( size );
        for( size_t index = 0; index < size; ++index )
        {
            objects.Push( m_Objects[ index ] );
        }

        m_Objects.clear();
    }
}

#if 0

void ArchiveXML::OnStartElement(const XML_Char *pszName, const XML_Char **papszAttrs)
{
    if (m_Abort)
    {
        return;
    }

    if ( !_tcscmp(pszName, TXT( "Reflect" ) ) )
    {
        if ( papszAttrs[0] && papszAttrs[1] && papszAttrs[0] && _tcsicmp(papszAttrs[0], TXT( "FileFormatVersion" ) ) == 0 )
        {
            tistringstream str ( papszAttrs[1] );
            str >> m_Version;
            if (str.fail())
            {
                throw Reflect::DataFormatException( TXT( "Unable to read file format version" ) );
            }
        }
        else
        {
            m_Version = 1;
        }

        return;
    }

    //
    // Find object type
    //

    tstring objectClassName;
    for (int i=0; papszAttrs[i]; i+=2)
    {
        if ( !_tcscmp( papszAttrs[i], TXT( "Type" ) ) )
        {
            objectClassName = papszAttrs[ i + 1 ];
            break;
        }
    }

    if ( objectClassName.empty() )
    {
        HELIUM_BREAK();
        throw Reflect::DataFormatException( TXT( "Unable to find object type attribute" ) );
    }

    // 
    // We use a stack to track the state of parsing, this will be the new state
    //

    State startState;

#if HELIUM_DEBUG
    startState.m_Type = objectClassName;
#endif

    State* topState = NULL;
    if ( !m_States.empty() )
    {
        topState = &m_States.top();
    }

    //
    // First pass at creation:
    //  Check parent for a data matching this object... handles serializers and field objects
    //

    if ( topState && topState->m_Instance && topState->m_Composite )
    {
        // look for the field name in the attributes
        const tchar_t* fieldName = NULL;
        for (int i=0; papszAttrs[i]; i+=2)
        {
            if ( !_tcscmp( papszAttrs[i], TXT( "Name" ) ) )
            {
                fieldName = papszAttrs[i+1];
                break;
            }
        }

        // if we found the field name in the xml, find the actual field in reflection data
        if ( fieldName )
        {
            startState.m_Field = topState->m_Composite->FindFieldByName( Crc32( fieldName ) );
        }

        // if we have found a field in our parent's definition
        if ( startState.m_Field )
        {
            // create the data object, if this fails then we have something besides a data class associated w/ this field (bad)
            DataPtr data = SafeCast<Data>( Registry::GetInstance()->CreateInstance( startState.m_Field->m_DataClass ) );
            HELIUM_ASSERT( data );

            // connect the current instance to the data
            data->ConnectField( topState->m_Instance, startState.m_Field );

            // use this object to parse with
            startState.m_Instance = startState.m_Object = data;
        }
    }

    //
    // Second pass at creation:
    //  Try and get a creator for a new object to store the data
    //

    if ( !startState.m_Object.ReferencesObject() && !objectClassName.empty() )
    {
        startState.m_Instance = startState.m_Object = Registry::GetInstance()->CreateInstance( Crc32( objectClassName.c_str() ) );

        if ( startState.m_Object.ReferencesObject() )
        {
            startState.m_Composite = startState.m_Object->GetClass();
            startState.m_Object->PreDeserialize( NULL );
        }
        else
        {
            Log::Debug( TXT( "Unable to create object with name: %s\n" ), objectClassName);
        }
    }

    m_States.push( startState );

#ifdef REFLECT_DISPLAY_PARSE_STACK
    m_Indent.Push();
    m_Indent.Get(std::cout);
    Log::Print("<Object>\n");
#endif 
}

void ArchiveXML::OnCharacterData(const XML_Char *pszData, int nLength)
{
    if (m_Abort)
    {
        return;
    }

    State* topState = m_States.empty() ? NULL : &m_States.top();
    if ( topState && topState->m_Object )
    {
        topState->m_Body.append( pszData, nLength );
    }
}

void ArchiveXML::OnEndElement(const XML_Char *pszName)
{
    if (m_Abort)
    {
        return;
    }

    if (!_tcscmp(pszName, TXT( "Reflect" ) ))
    {
        return;
    }

    // we own this state, do pop it off the stack
    State endState = m_States.top();
    m_States.pop();

#ifdef REFLECT_DISPLAY_PARSE_STACK
    m_Indent.Get(std::cout);
    Log::Print("</Object>\n");
    m_Indent.Pop();
#endif

    if ( endState.m_Object )
    {
        // are we nested within another object?
        State* topState = m_States.empty() ? NULL : &m_States.top();
        if ( topState && endState.m_Field )
        {
            topState->m_Object->PreDeserialize( endState.m_Field );
        }

        Data* data = SafeCast< Data >( endState.m_Object );
        if ( data )
        {
            ArchiveXML xml;
            tstringstream stream( endState.m_Body );
            Reflect::TCharStream archiveStream( &stream, false );
            archiveStream.IncrRefCount();
            xml.m_Stream = &archiveStream;
            xml.m_Objects = endState.m_Stash;
            data->Deserialize( xml );
        }

        if ( endState.m_Object )
        {
            endState.m_Object->PostDeserialize( NULL );

            // if we are we should see if it's being processed and perhaps be added as a field
            if ( topState != NULL )
            {
                // see if we should process this object as a field, or as unknown
                if ( endState.m_Field && endState.m_Object )
                {
                    DataPtr data = SafeCast<Data>( endState.m_Object );
                    if ( data.ReferencesObject() )
                    {
                        data->Disconnect();
                        
                        // might be useful to cache the data object here
                    }

                    topState->m_Object->PostDeserialize( endState.m_Field );
                }
                else
                {
                    // we are unknown, so send us up to be processed by the parent
                    if ( topState->m_Object )
                    {
                        topState->m_Object->ProcessUnknown(endState.m_Object, endState.m_Field ? Crc32( endState.m_Field->m_Name ) : 0 );
                    }
                }
            }
        }
    }

    // if this is a top level object push the result into the target (even if its null)
    if ( !m_States.empty() )
    {
        m_States.top().m_Stash.push_back( endState.m_Object );
    }
    else
    {
        // we've reached the top of the processed stack, send off to client for processing
        m_Objects.push_back( endState.m_Object );

        ArchiveStatus info( *this, ArchiveStates::ObjectProcessed );
        info.m_Progress = m_Progress;
        e_Status.Raise( info );

        m_Abort |= info.m_Abort;
    }
}

#endif

void ArchiveXML::ToString(Object* object, tstring& xml )
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
