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

char Indent<char>::m_Space = ' ';
wchar_t Indent<wchar_t>::m_Space = L' ';

// uncomment to display parse stack progress
//#define REFLECT_DISPLAY_PARSE_STACK

const uint32_t ArchiveXML::CURRENT_VERSION                               = 4;

ArchiveXML::ArchiveXML( const Path& path, ByteOrder byteOrder )
: Archive( path, byteOrder )
, m_Version( CURRENT_VERSION )
{
    m_Parser = XML_ParserCreate( NULL );

    // set the user data used in callbacks
    XML_SetUserData(m_Parser, (void*)this);

    // attach callbacks, will call back to 'this' via user data pointer
    XML_SetStartElementHandler(m_Parser, &StartElementHandler);
    XML_SetEndElementHandler(m_Parser, &EndElementHandler);
    XML_SetCharacterDataHandler(m_Parser, &CharacterDataHandler);
}

ArchiveXML::ArchiveXML()
: Archive()
, m_Version( CURRENT_VERSION )
{
    m_Parser = XML_ParserCreate( NULL );

    // set the user data used in callbacks
    XML_SetUserData(m_Parser, (void*)this);

    // attach callbacks, will call back to 'this' via user data pointer
    XML_SetStartElementHandler(m_Parser, &StartElementHandler);
    XML_SetEndElementHandler(m_Parser, &EndElementHandler);
    XML_SetCharacterDataHandler(m_Parser, &CharacterDataHandler);
}

ArchiveXML::~ArchiveXML()
{
    XML_ParserFree( m_Parser );
    m_Parser = NULL;
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
    long step = 0;
    const unsigned bufferSizeInBytes = 4096;
    while (!m_Stream->Fail() && !m_Abort)
    {
        m_Progress = (int)(((float)(step++ * bufferSizeInBytes) / (float)size) * 100.0f);

        tchar_t* pszBuffer = (tchar_t*)XML_GetBuffer(m_Parser, bufferSizeInBytes); // REQUEST
        HELIUM_ASSERT(pszBuffer != NULL);

        // divide by the character size so wide char builds don't override the allocation
        //  stream objects read characters, not byte-by-byte
        m_Stream->ReadBuffer(pszBuffer, bufferSizeInBytes / sizeof(tchar_t));

        int last_read = static_cast<int>(m_Stream->ObjectsRead());
        if (!XML_ParseBuffer(m_Parser, last_read * sizeof(tchar_t), last_read == 0) != 0)
        {
            throw Reflect::DataFormatException( TXT( "XML parsing failure, buffer contents:\n%s" ), (const tchar_t*)pszBuffer);
        }
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
    Serialize(m_Objects, ArchiveFlags::Status);

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

void ArchiveXML::Deserialize(ObjectPtr& object)
{
    if ( m_Objects.size() == 1 )
    {
        object = m_Objects.front();
        m_Objects.clear();
    }
    else
    {
        // xml doesn't work this way, this function is basically a pass-off point of object pointers
        //  from PointerSerializer... since XML parsing means we don't have flow control of the stream
        HELIUM_BREAK();
        throw Reflect::LogisticException( TXT( "Internal Error: Missing object" ) );
    }
}

void ArchiveXML::Deserialize( void* structure, const Structure* type )
{
    // this should probably just push a structure instance onto the state stack
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
    bool foundTypeAttribute = false;

    for (int i=0; papszAttrs[i]; i+=2)
    {
        if ( !_tcscmp( papszAttrs[i], TXT( "Type" ) ) )
        {
            foundTypeAttribute = true;

            bool converted = Helium::ConvertString( papszAttrs[ i + 1 ], objectClassName );
            HELIUM_ASSERT( converted );
        }
    }

    if ( !foundTypeAttribute )
    {
        HELIUM_BREAK();
        throw Reflect::DataFormatException( TXT( "Unable to find object type attribute" ) );
    }

    // 
    // We use a stack to track the state of parsing, this will be the new state
    //

    State startState;
    startState.m_Type = objectClassName.c_str();

    State* topState = NULL;
    if ( !m_States.empty() )
    {
        topState = &m_States.top();
    }

    //
    // First pass at creation:
    //  Check parent for a data matching this object... handles serializers and field objects
    //

    if ( topState && topState->m_Object )
    {
        // pointer to the parent object below which we are nested
        ObjectPtr parentObject = topState->m_Object;

        // retrieve the type information for our parent structure
        const Class* parentTypeDefinition = parentObject->GetClass();

        if ( parentTypeDefinition )
        {
            // look for the field name in the attributes
            const tchar_t* fieldName = NULL;
            for (int i=0; papszAttrs[i]; i+=2)
            {
                if ( !_tcscmp( papszAttrs[i], TXT( "Name" ) ) )
                {
                    fieldName = papszAttrs[i+1];
                }
            }

            if ( fieldName )
            {
                startState.m_Field = parentTypeDefinition->FindFieldByName( Crc32( fieldName ) );
            }

            // we have found a fieldinfo into our parent's definition
            if ( startState.m_Field != NULL )
            {
                // this is our new object
                ObjectPtr object = Registry::GetInstance()->CreateInstance( startState.m_Field->m_DataClass );

                Data* data = SafeCast<Data>(object);
                if ( data )
                {
                    // connect the current instance to the data
                    data->ConnectField(parentObject.Ptr(), startState.m_Field);
                }

                if (object.ReferencesObject())
                {
                    // use this object to parse with
                    startState.m_Object = object;
                }
            }
        }
    }

    //
    // Second pass at creation:
    //  Try and get a creator for a new object to store the data
    //

    if ( !startState.m_Object.ReferencesObject() && !objectClassName.empty() )
    {
        //
        // Attempt creation of object via name
        //

        const Class* type = Reflect::Registry::GetInstance()->GetClass( Crc32( objectClassName.c_str() ) );

        if ( type )
        {
            startState.m_Object = Registry::GetInstance()->CreateInstance( type );
        }

        if ( !startState.m_Object.ReferencesObject() )
        {
            Log::Debug( TXT( "Unable to create object with name: %s\n" ), objectClassName);
        }
    }

    //
    // Do callbacks
    //

    if (startState.m_Object)
    {
        startState.m_Object->PreDeserialize( NULL );
    }

    //
    // Push state
    //

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

    // this should never happen, an object just ended
    HELIUM_ASSERT( !m_States.empty() );
    State endState = m_States.top();

    // we own this state, do pop it off the stack
    m_States.pop();

#ifdef REFLECT_DISPLAY_PARSE_STACK
    m_Indent.Get(std::cout);
    Log::Print("</Object>\n");
    m_Indent.Pop();
#endif

    if ( endState.m_Object )
    {
        // are we nested within another object?
        State* parentState = m_States.empty() ? NULL : &m_States.top();

        // do callbacks
        if ( parentState != NULL )
        {
            parentState->m_Object->PreDeserialize( endState.m_Field );
        }

        Data* data = SafeCast< Data >( endState.m_Object );

        // do data logic
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

        // do callbacks
        if ( endState.m_Object )
        {
            if ( !TryObjectCallback( endState.m_Object, &Object::PostDeserialize, NULL ) )
            {
                endState.m_Object = NULL; // discard the object
            }

            // if we are we should see if it's being processed and perhaps be added as a field
            if ( parentState != NULL )
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

                    parentState->m_Object->PostDeserialize( endState.m_Field );
                }
                else
                {
                    // we are unknown, so send us up to be processed by the parent
                    if ( parentState->m_Object )
                    {
                        parentState->m_Object->ProcessUnknown(endState.m_Object, endState.m_Field ? Crc32( endState.m_Field->m_Name ) : 0 );
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
