#include "ReflectPch.h"
#include "XMLDocument.h"

#include "Foundation/Exception.h"

#include <expat.h>

using namespace Helium;

XMLDocument::XMLDocument()
{
    m_Parser = XML_ParserCreate( NULL );

    // set the user data used in callbacks
    XML_SetUserData(m_Parser, (void*)this);

    // attach callbacks, will call back to 'this' via user data pointer
    XML_SetStartElementHandler(m_Parser, &StartElementHandler);
    XML_SetEndElementHandler(m_Parser, &EndElementHandler);
    XML_SetCharacterDataHandler(m_Parser, &CharacterDataHandler);
}

XMLDocument::~XMLDocument()
{
    XML_ParserFree( m_Parser );
    m_Parser = NULL;
}

void XMLDocument::ParseBuffer( const void* buffer, uint32_t lengthInBytes, bool finalize )
{
    if ( XML_STATUS_OK != XML_Parse(m_Parser, static_cast< const char* >( buffer ), lengthInBytes, finalize) )
    {
        throw Helium::Exception( TXT( "XML parsing failure" ) );
    }
}

void XMLDocument::OnStartElement(const tchar_t *pszName, const tchar_t **papszAttrs)
{
    int32_t index = static_cast< int32_t >( m_Elements.GetSize() );
    m_Elements.Push( XMLElement () );
    XMLElement* element = &m_Elements.GetLast();
    element->m_Index = index;
    element->m_Document = this;

    // copy data
    element->m_Name.Set( pszName );
    for (int i=0; papszAttrs[i]; i+=2)
    {
        element->m_Attributes.Insert( XMLElement::AttributeValueType ( Name( papszAttrs[i] ), String( papszAttrs[i+1] ) ) );
    }

    // fetch parent
    XMLElement* parent = NULL;
    if ( !m_Stack.IsEmpty() )
    {
        parent = &m_Elements[ m_Stack.GetLast() ];
    }

    // setup hierarchy
    if ( parent )
    {
        element->m_Parent = parent->m_Index;

        if ( parent->m_FirstChild == -1 )
        {
            parent->m_FirstChild = element->m_Index;
        }
        else
        {
            XMLElement* sibling = &m_Elements[ parent->m_FirstChild ];
            XMLElement* previousSibling = sibling;
            while ( sibling = ( sibling->m_NextSibling != -1 ? &m_Elements[ sibling->m_NextSibling ] : NULL ) )
            {
                previousSibling = sibling;
            }

            HELIUM_ASSERT( previousSibling->m_NextSibling == -1 );
            previousSibling->m_NextSibling = element->m_Index;
        }
    }

    m_Stack.Push( element->m_Index );
}

void XMLDocument::OnCharacterData(const tchar_t *pszData, int nLength)
{
    m_Elements[ m_Stack.GetLast() ].m_Body.Add( pszData, nLength );
}

void XMLDocument::OnEndElement(const tchar_t *pszName)
{
    m_Stack.Pop();
}

void XMLDocument::StartElementHandler(void *pUserData, const tchar_t* pszName, const tchar_t **papszAttrs)
{
    XMLDocument* document = (XMLDocument*)pUserData;
    document->OnStartElement(pszName, papszAttrs);
}

void XMLDocument::CharacterDataHandler(void *pUserData, const tchar_t* pszData, int nLength)
{
    XMLDocument* document = (XMLDocument*)pUserData;
    document->OnCharacterData(pszData, nLength);
}

void XMLDocument::EndElementHandler(void *pUserData, const tchar_t* pszName)
{
    XMLDocument* document = (XMLDocument*)pUserData;
    document->OnEndElement(pszName);
}
