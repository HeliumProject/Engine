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
            : m_Parent( NULL )
            , m_FirstChild( NULL )
            , m_NextSibling( NULL )
        {

        }

        // Element Info
        Name            m_Name;
        String          m_Body;
        AttributeMap    m_Attributes;

        // Element Hierarchy
        XMLElement*     m_Parent;
        XMLElement*     m_FirstChild;
        XMLElement*     m_NextSibling;
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
        DynArray< XMLElement* > m_Stack;
        DynArray< XMLElement >  m_Elements;
    };
}