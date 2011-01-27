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

    // serialize main file elements
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

void ArchiveXML::Serialize(const std::vector< ObjectPtr >& elements, uint32_t flags)
{
    Serialize( elements.begin(), elements.end(), flags );
}

void ArchiveXML::Serialize( const DynArray< ObjectPtr >& elements, uint32_t flags )
{
    Serialize( elements.Begin(), elements.End(), flags );
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
    if (m_Objects.size() == 1)
    {
        object = m_Objects.front();
        m_Objects.clear();
    }
    else
    {
        // xml doesn't work this way
        HELIUM_BREAK();
        throw Reflect::LogisticException( TXT( "Internal Error: Missing object" ) );
    }
}

void ArchiveXML::Deserialize( void* structure, const Structure* type )
{

}

void ArchiveXML::Deserialize(std::vector< ObjectPtr >& elements, uint32_t flags)
{
    if (!m_Objects.empty())
    {
        if ( !(flags & ArchiveFlags::Sparse) )
        {
            m_Objects.erase( std::remove( m_Objects.begin(), m_Objects.end(), ObjectPtr () ), m_Objects.end() );
        }

        elements = m_Objects;

        m_Objects.clear();
    }
}

void ArchiveXML::Deserialize( DynArray< ObjectPtr >& elements, uint32_t flags )
{
    elements.Clear();

    if (!m_Objects.empty())
    {
        if ( !(flags & ArchiveFlags::Sparse) )
        {
            m_Objects.erase( std::remove( m_Objects.begin(), m_Objects.end(), ObjectPtr () ), m_Objects.end() );
        }

        size_t size = m_Objects.size();
        elements.Reserve( size );
        for( size_t index = 0; index < size; ++index )
        {
            elements.Push( m_Objects[ index ] );
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

    tstring elementType;
    bool foundTypeAttribute = false;

    for (int i=0; papszAttrs[i]; i+=2)
    {
        if ( !_tcscmp( papszAttrs[i], TXT( "Type" ) ) )
        {
            foundTypeAttribute = true;

            bool converted = Helium::ConvertString( papszAttrs[ i + 1 ], elementType );
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

    ParsingStatePtr newState = new ParsingState (elementType.c_str());
    ParsingStatePtr topState = m_StateStack.empty() ? NULL : m_StateStack.top();

    //
    // First pass at creation:
    //  Check parent for a data matching this object... handles serializers and field elements
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
                newState->m_Field = parentTypeDefinition->FindFieldByName( Crc32( fieldName ) );
            }

            // we have found a fieldinfo into our parent's definition
            if ( newState->m_Field != NULL )
            {
                // this is our new object
                ObjectPtr object = Registry::GetInstance()->CreateInstance( newState->m_Field->m_DataClass );

                Data* data = SafeCast<Data>(object);
                if ( data )
                {
                    // connect the current instance to the data
                    data->ConnectField(parentObject.Ptr(), newState->m_Field);
                }

                if (object.ReferencesObject())
                {
                    // flag this as a field
                    newState->SetFlag( ParsingState::kField, true );

                    // use this object to Parse with
                    newState->m_Object = object;
                }
            }
        }
    }

    //
    // Second pass at creation:
    //  Try and get a creator for a new object to store the data
    //

    if ( !newState->m_Object.ReferencesObject() && !elementType.empty() )
    {
        //
        // Attempt creation of object via name
        //

        const Class* type = Reflect::Registry::GetInstance()->GetClass( Crc32( elementType.c_str() ) );

        if ( type )
        {
            newState->m_Object = Registry::GetInstance()->CreateInstance( type );
        }

        if ( !newState->m_Object.ReferencesObject() )
        {
            Log::Debug( TXT( "Unable to create object with name: %s\n" ), elementType);
        }
    }

    //
    // Do callbacks
    //

    if (newState->m_Object)
    {
        newState->m_Object->PreDeserialize( NULL );
    }

    //
    // Push state
    //

    m_StateStack.push( newState );

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

    ParsingStatePtr topState = m_StateStack.empty() ? NULL : m_StateStack.top();
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
    HELIUM_ASSERT( !m_StateStack.empty() );
    ParsingStatePtr topState = m_StateStack.top();

    // we own this state, do pop it off the stack
    m_StateStack.pop();

#ifdef REFLECT_DISPLAY_PARSE_STACK
    m_Indent.Get(std::cout);
    Log::Print("</Object>\n");
    m_Indent.Pop();
#endif

    if ( topState->m_Object )
    {
        // are we nested within another object?
        ParsingStatePtr parentState = m_StateStack.empty() ? NULL : m_StateStack.top();

        // do callbacks
        if ( parentState != NULL )
        {
            parentState->m_Object->PreDeserialize( topState->m_Field );
        }

        Data* data = SafeCast< Data >( topState->m_Object );

        // do data logic
        if ( data && !topState->m_Body.empty())
        {
            ArchiveXML xml;
            tstringstream stream (topState->m_Body);
            xml.m_Stream = new Reflect::TCharStream(&stream, false);
            xml.m_Objects = topState->m_Objects;

            data->Deserialize(xml);
        }

        // do callbacks
        if ( topState->m_Object )
        {
            if ( !TryObjectCallback( topState->m_Object, &Object::PostDeserialize, NULL ) )
            {
                topState->m_Object = NULL; // discard the object
            }

            // if we are we should see if it's being processed and perhaps be added as a field
            if ( parentState != NULL )
            {
                // see if we should process this object as a field, or as unknown
                if ( topState->GetFlag( ParsingState::kField ) )
                {
                    DataPtr data = SafeCast<Data>(topState->m_Object);
                    if ( data.ReferencesObject() )
                    {
                        data->Disconnect();
                        
                        // might be useful to cache the data object here
                    }

                    parentState->m_Object->PostDeserialize( topState->m_Field );
                }
                else
                {
                    // we are unknown, so send us up to be processed by the parent
                    if ( parentState->m_Object )
                    {
                        parentState->m_Object->ProcessUnknown(topState->m_Object, topState->m_Field ? Crc32( topState->m_Field->m_Name ) : 0 );
                    }
                }
            }
        }
    }

    // if this is a top level object push the result into the target (even if its null)
    if ( !m_StateStack.empty() )
    {
        ParsingStatePtr parentState = m_StateStack.top();

        parentState->m_Objects.push_back(topState->m_Object);
    }
    else
    {
        // we've reached the top of the processed stack, send off to client for processing
        m_Objects.push_back( topState->m_Object );

        ArchiveStatus info( *this, ArchiveStates::ObjectProcessed );
        info.m_Progress = m_Progress;
        e_Status.Raise( info );

        m_Abort |= info.m_Abort;
    }
}

void ArchiveXML::ToString(Object* object, tstring& xml )
{
    std::vector< ObjectPtr > elements(1);
    elements[0] = object;
    return ToString( elements, xml );
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

void ArchiveXML::ToString( const std::vector< ObjectPtr >& elements, tstring& xml )
{
    ArchiveXML archive;
    tstringstream strStream;

    archive.m_Stream = new Reflect::TCharStream( &strStream, false ); 
    archive.m_Objects  = elements;
    archive.Write();

    xml = strStream.str();
}

void ArchiveXML::FromString( const tstring& xml, std::vector< ObjectPtr >& elements )
{
    ArchiveXML archive;
    tstringstream strStream;
    strStream << xml;

    archive.m_Stream = new Reflect::TCharStream( &strStream, false );
    archive.Read();

    elements = archive.m_Objects;
}
