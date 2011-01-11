#include "ArchiveXML.h"

#include "Element.h"
#include "Registry.h"
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
, m_Target( &m_Spool )
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
: Archive( TXT( "" ) )
, m_Version( CURRENT_VERSION )
, m_Target( &m_Spool )
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

    StatusInfo info( *this, ArchiveStates::Starting );
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

    // setup visitors
    PreDeserialize();

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

        int last_read = static_cast<int>(m_Stream->ElementsRead());
        if (!XML_ParseBuffer(m_Parser, last_read * sizeof(tchar_t), last_read == 0) != 0)
        {
            throw Reflect::DataFormatException( TXT( "XML parsing failure, buffer contents:\n%s" ), (const tchar_t*)pszBuffer);
        }
    }

    info.m_ArchiveState = ArchiveStates::ElementProcessed;
    info.m_Progress = 100;
    e_Status.Raise( info );

    info.m_ArchiveState = ArchiveStates::Complete;
    e_Status.Raise( info );
}

void ArchiveXML::Write()
{
    REFLECT_SCOPE_TIMER(( "Reflect - XML Write" ));

    StatusInfo info( *this, ArchiveStates::Starting );
    e_Status.Raise( info );

    // setup visitors
    PreSerialize();

#ifdef UNICODE
    uint16_t feff = 0xfeff;
    m_Stream->Write( &feff ); // byte order mark
#endif

    *m_Stream << TXT( "<?xml version=\"1.0\" encoding=\"" ) << Helium::GetEncoding() << TXT( "\"?>\n" );
    *m_Stream << TXT( "<Reflect FileFormatVersion=\"" ) << m_Version << TXT( "\">\n" );

    // serialize main file elements
    Serialize(m_Spool, ArchiveFlags::Status);

    *m_Stream << TXT( "</Reflect>\n\0" );

    info.m_ArchiveState = ArchiveStates::Complete;
    e_Status.Raise( info );
}

void ArchiveXML::Serialize(const ElementPtr& element)
{
    PreSerialize(element);

    element->PreSerialize();

    SerializeHeader(element);

    if (element->HasType(Reflect::GetType<Data>()))
    {
        Data* s = DangerousCast<Data>(element);

        s->Serialize(*this);
    }
    else
    {
        SerializeFields(element);
    }

    SerializeFooter(element);

    element->PostSerialize();
}

void ArchiveXML::Serialize(const std::vector< ElementPtr >& elements, uint32_t flags)
{
    m_FieldNames.push( NULL );

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

    m_FieldNames.pop();
}

void ArchiveXML::SerializeFields(const ElementPtr& element)
{
    //
    // Serialize fields
    //

    const Class* type = element->GetClass();
    HELIUM_ASSERT(type != NULL);

    DynArray< Field >::ConstIterator itr = type->m_Fields.Begin();
    DynArray< Field >::ConstIterator end = type->m_Fields.End();
    for ( ; itr != end; ++itr )
    {
        SerializeField(element, &*itr);
    }
}

void ArchiveXML::SerializeField(const ElementPtr& element, const Field* field)
{
    // don't write no write fields
    if ( field->m_Flags & FieldFlags::Discard )
    {
        return;
    }

    // set current field name
    m_FieldNames.push( field->m_Name );

    // construct serialization object
    ElementPtr e;
    m_Cache.Create( field->m_DataClass, e );

    HELIUM_ASSERT( e.ReferencesObject() );

    // downcast serializer
    DataPtr serializer = ObjectCast<Data>(e);

    if (!serializer.ReferencesObject())
    {
        // this should never happen, the type id in the rtti data is bogus
        throw Reflect::TypeInformationException( TXT( "Invalid type id for field %s" ), field->m_Name );
    }
    else
    {
        // set data pointer
        serializer->ConnectField(element.Ptr(), field);

        // bool for test results
        bool serialize = true;

        // check for equality
#ifdef REFLECT_REFACTOR
        if ( serialize && field->m_Default.ReferencesObject() )
        {
            bool force = (field->m_Flags & FieldFlags::Force) != 0;
            if (!force && field->m_Default->Equals(serializer))
            {
                serialize = false;
            }
        }
#endif

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

            // process
            Serialize( serializer );
        }

        // disconnect
        serializer->Disconnect();
    }

    m_FieldNames.pop();
}

void ArchiveXML::SerializeHeader(const ElementPtr& element)
{
    //
    // Start header
    //

    m_Indent.Push();
    m_Indent.Get( *m_Stream );
    *m_Stream << TXT( "<Element Type=\"" ) << element->GetClass()->m_Name << TXT( "\"" );

    //
    // Field name
    //

    if ( !m_FieldNames.empty() && m_FieldNames.top() )
    {
        tstring name;
        Helium::ConvertString( m_FieldNames.top(), name );

        // our link back to the field we are nested in
        *m_Stream << TXT( " Name=\"" ) << name << TXT( "\"" );
    }

    //
    // End header
    //

    if ( element->IsCompact() )
    {
        *m_Stream << TXT( ">" );
    }
    else
    {
        *m_Stream << TXT( ">\n" );
    }
}

void ArchiveXML::SerializeFooter(const ElementPtr& element)
{
    if ( !element->IsCompact() )
    {
        m_Indent.Get(*m_Stream);
    }

    *m_Stream << TXT( "</Element>\n" );

    m_Indent.Pop();
}

void ArchiveXML::Deserialize(ElementPtr& element)
{
    if (m_Components.size() == 1)
    {
        element = m_Components.front();
        m_Components.clear();
    }
    else
    {
        // xml doesn't work this way
        HELIUM_BREAK();
        throw Reflect::LogisticException( TXT( "Internal Error: Missing element" ) );
    }
}

void ArchiveXML::Deserialize(std::vector< ElementPtr >& elements, uint32_t flags)
{
    if (!m_Components.empty())
    {
        if ( !(flags & ArchiveFlags::Sparse) )
        {
            m_Components.erase( std::remove( m_Components.begin(), m_Components.end(), ElementPtr () ), m_Components.end() );
        }

        elements = m_Components;

        m_Components.clear();
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
    // Find element type
    //

    tstring elementType;

    if ( elementType.empty() )
    {
        for (int i=0; papszAttrs[i]; i+=2)
        {
            if ( !_tcscmp( papszAttrs[i], TXT( "Type" ) ) )
            {
                bool converted = Helium::ConvertString( papszAttrs[ i + 1 ], elementType );
                HELIUM_ASSERT( converted );
            }
        }
    }

    if ( elementType.empty() )
    {
        HELIUM_BREAK();
        throw Reflect::DataFormatException( TXT( "Unable to find element type attribute" ) );
    }

    // 
    // We use a stack to track the state of parsing, this will be the new state
    //

    ParsingStatePtr newState = new ParsingState (elementType.c_str());
    ParsingStatePtr topState = m_StateStack.empty() ? NULL : m_StateStack.top();

    //
    // First pass at creation:
    //  Check parent for a serializer matching this element... handles serializers and field elements
    //

    if ( topState && topState->m_Element )
    {
        // pointer to the parent element below which we are nested
        ElementPtr parentElement = topState->m_Element;

        // retrieve the type information for our parent structure
        const Class* parentTypeDefinition = parentElement->GetClass();

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
                // this is our new element
                ElementPtr element = NULL;

                // create the object
                m_Cache.Create(newState->m_Field->m_DataClass, element);

                // if we are a serializer
                if (element->HasType(Reflect::GetType<Data>()))
                {
                    // connect the current instance to the serializer
                    DangerousCast<Data>(element)->ConnectField(parentElement.Ptr(), newState->m_Field);
                }

                if (element.ReferencesObject())
                {
                    // flag this as a field
                    newState->SetFlag( ParsingState::kField, true );

                    // use this element to Parse with
                    newState->m_Element = element;
                }
            }
        }
    }

    //
    // Second pass at creation:
    //  Try and get a creator for a new element to store the data
    //

    if ( !newState->m_Element.ReferencesObject() )
    {
        //
        // Attempt creation of element via name
        //

        const Class* type = Reflect::Registry::GetInstance()->GetClass( Crc32( elementType.c_str() ) );

        if ( type )
        {
            m_Cache.Create( type, newState->m_Element );
        }

        if ( !newState->m_Element.ReferencesObject() )
        {
            Log::Debug( TXT( "Unable to create element with name: %s\n" ), elementType);
        }
    }

    //
    // Do callbacks
    //

    if (newState->m_Element)
    {
        newState->m_Element->PreDeserialize();
    }

    //
    // Push state
    //

    m_StateStack.push( newState );

#ifdef REFLECT_DISPLAY_PARSE_STACK
    m_Indent.Push();
    m_Indent.Get(std::cout);
    Log::Print("<Element>\n");
#endif 
}

void ArchiveXML::OnCharacterData(const XML_Char *pszData, int nLength)
{
    if (m_Abort)
    {
        return;
    }

    ParsingStatePtr topState = m_StateStack.empty() ? NULL : m_StateStack.top();
    if ( topState && topState->m_Element )
    {
        topState->m_Buffer.append( pszData, nLength );
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

    // this should never happen, an element just ended
    HELIUM_ASSERT( !m_StateStack.empty() );
    ParsingStatePtr topState = m_StateStack.top();

    // we own this state, do pop it off the stack
    m_StateStack.pop();

#ifdef REFLECT_DISPLAY_PARSE_STACK
    m_Indent.Get(std::cout);
    Log::Print("</Element>\n");
    m_Indent.Pop();
#endif

    if ( topState->m_Element )
    {
        // do Data logic
        if ( topState->m_Element->HasType(Reflect::GetType<Data>()) && !topState->m_Buffer.empty())
        {
            Data* serializer = DangerousCast<Data>(topState->m_Element);

            tstringstream stream (topState->m_Buffer);

            ArchiveXML xml;
            xml.m_Stream = new Reflect::TCharStream(&stream, false);
            xml.m_Components = topState->m_Components;
            serializer->Deserialize(xml);
        }

        // do callbacks
        if ( topState->m_Element )
        {
            if ( !TryElementCallback( topState->m_Element, &Element::PostDeserialize ) )
            {
                topState->m_Element = NULL; // discard the object
            }
        }

        if ( topState->m_Element )
        {
            PostDeserialize( topState->m_Element );

            // are we nested within another element?
            ParsingStatePtr parentState = m_StateStack.empty() ? NULL : m_StateStack.top();

            // if we are we should see if it's being processed and perhaps be added as a component
            if ( parentState != NULL )
            {
                // see if we should process this element as a as a field, or as a component
                if ( topState->GetFlag( ParsingState::kField ) )
                {
                    DataPtr serializer = ObjectCast<Data>(topState->m_Element);
                    if ( serializer.ReferencesObject() )
                    {
                        // disconnect our serializer for neatness
                        serializer->Disconnect();

                        // send it back to the free store
                        m_Cache.Free(topState->m_Element);
                    }

                    PostDeserialize( parentState->m_Element, topState->m_Field );
                }
                else
                {
                    ElementPtr container = parentState->m_Element;

                    // we are a component, so send us up to be processed by container
                    if ( container && !container->ProcessComponent(topState->m_Element, topState->m_Field ? topState->m_Field->m_Name : NULL ) )
                    {
                        Log::Debug( TXT( "%s did not process %s, discarding\n" ), container->GetClass()->m_Name, topState->m_Element->GetClass()->m_Name );
                    }
                }
            }
        }
    }

    // if this is a top level element push the result into the target (even if its null)
    if ( !m_StateStack.empty() )
    {
        ParsingStatePtr parentState = m_StateStack.top();

        parentState->m_Components.push_back(topState->m_Element);
    }
    else if ( topState->m_Element.ReferencesObject() )
    {
        // we've reached the top of the processed stack, send off to client for processing
        m_Target->push_back( topState->m_Element );

        StatusInfo info( *this, ArchiveStates::ElementProcessed );
        info.m_Progress = m_Progress;
        e_Status.Raise( info );

        m_Abort |= info.m_Abort;
    }
}

void ArchiveXML::ToString(const ElementPtr& element, tstring& xml )
{
    std::vector< ElementPtr > elements(1);
    elements[0] = element;
    return ToString( elements, xml );
}

ElementPtr ArchiveXML::FromString( const tstring& xml, const Class* searchClass )
{
    if ( searchClass == NULL )
    {
        searchClass = Reflect::GetClass<Element>();
    }

    ArchiveXML archive;
    archive.m_SearchClass = searchClass;

    tstringstream strStream;
    strStream << xml;
    archive.m_Stream = new Reflect::TCharStream(&strStream, false); 
    archive.Read();

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

void ArchiveXML::ToString( const std::vector< ElementPtr >& elements, tstring& xml )
{
    ArchiveXML archive;
    tstringstream strStream;

    archive.m_Stream = new Reflect::TCharStream( &strStream, false ); 
    archive.m_Spool  = elements;
    archive.Write();

    xml = strStream.str();
}

void ArchiveXML::FromString( const tstring& xml, std::vector< ElementPtr >& elements )
{
    ArchiveXML archive;
    tstringstream strStream;
    strStream << xml;

    archive.m_Stream = new Reflect::TCharStream( &strStream, false );
    archive.Read();

    elements = archive.m_Spool;
}
