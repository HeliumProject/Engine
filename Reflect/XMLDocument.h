#pragma once

#include "Foundation/Name.h"
#include "Foundation/String.h"
#include "Foundation/Map.h"

#include "Reflect/API.h"

// expat fwds
struct XML_ParserStruct;
typedef struct XML_ParserStruct *XML_Parser;

namespace Helium
{
    class XMLDocument;

    class HELIUM_REFLECT_API XMLElement
    {
    public:
        typedef Map< Name, String >             AttributeMap;
        typedef Map< Name, String >::ValueType  AttributeValueType;

        inline XMLElement();

        // Data helpers
        inline const String* GetAttributeValue( const Name& name );
        inline XMLDocument* GetDocument();
        inline XMLElement* GetParent();
        inline XMLElement* GetFirstChild();
        inline XMLElement* GetNextSibling();
        inline XMLElement* GetNext( bool skipChildren = false );

        // Element Info
        Name            m_Name;
        String          m_Body;
        AttributeMap    m_Attributes;

        // Element Hierarchy
        XMLDocument*    m_Document;
        int32_t         m_Index;
        int32_t         m_Parent;
        int32_t         m_FirstChild;
        int32_t         m_NextSibling;
    };

    class HELIUM_REFLECT_API XMLDocument
    {
    public:
        class Iterator
        {
        public:
            inline Iterator();
            inline Iterator( XMLDocument* document );

            inline bool IsDone();
            inline XMLElement* Advance( bool skipChildren = false );
            inline XMLElement* GetCurrent();
            inline void SetCurrent( XMLElement* element );

        private:
            XMLElement* m_Current;
        };

        XMLDocument();
        ~XMLDocument();

        inline XMLElement* GetRoot();
        inline XMLElement* GetElement( int32_t index );

        void ParseBuffer( const void* buffer, uint32_t length, bool finalize );

    private:
        void OnStartElement(const tchar_t *pszName, const tchar_t **papszAttrs);
        void OnCharacterData(const tchar_t *pszData, int nLength);
        void OnEndElement(const tchar_t *pszName);

        static void StartElementHandler(void *pUserData, const tchar_t* pszName, const tchar_t **papszAttrs);
        static void CharacterDataHandler(void *pUserData, const tchar_t* pszData, int nLength);
        static void EndElementHandler(void *pUserData, const tchar_t* pszName);

        XML_Parser              m_Parser;
        DynamicArray< int32_t >     m_Stack;
        DynamicArray< XMLElement >  m_Elements;
    };
}

#include "Reflect/XMLDocument.inl"