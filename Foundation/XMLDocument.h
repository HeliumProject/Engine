#pragma once

#include "Foundation/Name.h"
#include "Foundation/String.h"
#include "Foundation/Container/Map.h"

// expat fwds
struct XML_ParserStruct;
typedef struct XML_ParserStruct *XML_Parser;

namespace Helium
{
    class FOUNDATION_API XMLElement
    {
    public:
        typedef Map< Name, String >             AttributeMap;
        typedef Map< Name, String >::ValueType  AttributeValueType;

        XMLElement()
            : m_Index( -1 )
            , m_Parent( -1 )
            , m_FirstChild( -1 )
            , m_NextSibling( -1 )
        {

        }

        const String* GetAttributeValue( const Name& name )
        {
            AttributeMap::ConstIterator found = m_Attributes.Find( name );
            if ( found != m_Attributes.End() )
            {
                return &found->Second();
            }

            return NULL;
        }

        // Element Info
        Name            m_Name;
        String          m_Body;
        AttributeMap    m_Attributes;

        // Element Hierarchy
        int32_t         m_Index;
        int32_t         m_Parent;
        int32_t         m_FirstChild;
        int32_t         m_NextSibling;

    };

    class FOUNDATION_API XMLDocument
    {
    public:
        XMLDocument();
        ~XMLDocument();

        XMLElement* GetRoot()
        {
            return m_Root;
        }

        void ParseBuffer( const void* buffer, uint32_t length, bool finalize );

    private:
        void OnStartElement(const tchar_t *pszName, const tchar_t **papszAttrs);
        void OnCharacterData(const tchar_t *pszData, int nLength);
        void OnEndElement(const tchar_t *pszName);

        static void StartElementHandler(void *pUserData, const tchar_t* pszName, const tchar_t **papszAttrs);
        static void CharacterDataHandler(void *pUserData, const tchar_t* pszData, int nLength);
        static void EndElementHandler(void *pUserData, const tchar_t* pszName);

        XML_Parser              m_Parser;
        XMLElement*             m_Root;
        DynArray< int32_t >     m_Stack;
        DynArray< XMLElement >  m_Elements;
    };
}