#pragma once

#include "Archive.h"

struct XML_ParserStruct;
typedef struct XML_ParserStruct *XML_Parser;

namespace Reflect
{
    template< class C >
    class Indent
    {
    private:
        static C m_Space;

        // Indent spacing
        unsigned int m_Indent;

    public:
        Indent()
            : m_Indent (0)
        {

        }

        void Push()
        {
            m_Indent++;
        }

        void Pop()
        {
            if (m_Indent > 0)
                m_Indent--;
        }

        void Get(Stream<C>& stream)
        {
            if (m_Indent > 0)
            {
                int indent = m_Indent;

                while(indent > 0)
                {
                    stream << m_Space;
                    indent--;
                }
            }
        }

        void Get(FILE* file)
        {
            if (file != NULL && m_Indent > 0)
            {
                int indent = m_Indent;

                while(indent > 0)
                {
                    fputc( m_Space, file );
                    indent--;
                }
            }
        }
    };

    //
    // XML Archive Class
    //

    class FOUNDATION_API ArchiveXML : public Archive
    {
    public: 
        static const u32 CURRENT_VERSION; 
        static const u32 FIRST_VERSION_WITH_POINTER_SERIALIZER; 
        static const u32 FIRST_VERSION_WITH_NAMESPACE_SUPPORT;

    private:
        friend class Archive;

        class ParsingState : public Nocturnal::RefCountBase<ParsingState>
        {
        public:
            // the name of the short name being processed
            std::string m_ShortName;

            // the cdata section for xml files
            std::string m_Buffer;

            // the current serializing field
            const Field* m_Field;

            // the item being processed
            ElementPtr m_Element;

            // The collected components
            V_Element m_Components;

            // flags, as specified below
            unsigned int m_Flags;

            enum ProcessFlag
            {
                kField  = 0x1 << 0,
            };

            ParsingState(const char* shortName)
                : m_ShortName (shortName)
                , m_Field (NULL) 
                , m_Flags (0)
            {

            }

            void SetFlag( ProcessFlag flag, bool state )
            {
                if ( state )
                    m_Flags |= flag;
                else
                    m_Flags &= ~flag;
            }

            bool GetFlag( ProcessFlag flag )
            {
                return ((m_Flags & flag) != 0x0);
            }
        };

        typedef Nocturnal::SmartPtr<ParsingState> ParsingStatePtr;

        // The expat parser object
        XML_Parser m_Parser;

        // The stream to use
        TCharStreamPtr m_Stream;

        // Indent helper
        Indent<tchar> m_Indent;

        // File format version
        u32 m_Version;

        // The nesting stack of parsing state
        std::stack<ParsingStatePtr> m_StateStack;

        // The current name of the serializing field
        std::stack<std::string> m_FieldNames;

        // The current collection of components
        V_Element m_Components;

        // The append elements
        V_Element m_Append;

        // The container to decode elements to
        V_Element* m_Target;

    private:
        ArchiveXML(StatusHandler* status = NULL);
        ~ArchiveXML();

    public:
        // Stream access
        TCharStream& GetStream()
        {
            return *m_Stream;
        }

    protected:
        // The type
        virtual ArchiveType GetType() const
        {
            return ArchiveTypes::XML;
        }

        virtual void OpenFile(const tstring& file, bool write = false);
        void OpenStream(TCharStream* stream, bool write = false);
        virtual void Close(); 

        // Begins parsing the InputStream
        virtual void Read();

        // Write to the OutputStream
        virtual void Write();

        // Write the file header
        virtual void Start();

        // Write the file footer
        virtual void Finish();

    public:
        // Access indentation
        Indent<tchar>& GetIndent()
        {
            return m_Indent;
        }

    public:
        // Serialize
        virtual void Serialize(const ElementPtr& element);
        virtual void Serialize(const V_Element& elements, u32 flags = 0);

    protected:
        // Helpers
        void SerializeFields(const ElementPtr& element);
        void SerializeField(const ElementPtr& element, const Field* field);

        // <Element> and </Element>
        void SerializeHeader(const ElementPtr& element);
        void SerializeFooter(const ElementPtr& element);

    public:
        // For handling components
        virtual void Deserialize(ElementPtr& element);
        virtual void Deserialize(V_Element& elements, u32 flags = 0);

    private:
        static void StartElementHandler(void *pUserData, const tchar* pszName, const tchar **papszAttrs)
        {
            ArchiveXML *archive = (ArchiveXML *)pUserData;
            archive->OnStartElement(pszName, papszAttrs);
        }

        static void EndElementHandler(void *pUserData, const tchar* pszName)
        {
            ArchiveXML *archive = (ArchiveXML *)pUserData;
            archive->OnEndElement(pszName);
        }

        static void CharacterDataHandler(void *pUserData, const tchar* pszData, int nLength)
        {
            ArchiveXML *archive = (ArchiveXML *)pUserData;
            archive->OnCharacterData(pszData, nLength);
        }

        // Called on <element>
        void OnStartElement(const tchar *pszName, const tchar **papszAttrs);

        // Called between <element> and </element>
        void OnCharacterData(const tchar *pszData, int nLength);

        // Called after </element>
        void OnEndElement(const tchar *pszName);

    public:
        // Reading and writing single element from string data
        static void       ToString(const ElementPtr& element, std::string& xml, StatusHandler* status = NULL);
        static ElementPtr FromString(const std::string& xml, int searchType = Reflect::ReservedTypes::Any, StatusHandler* status = NULL);

        // Reading and writing multiple elements from string data
        static void       ToString(const V_Element& elements, std::string& xml, StatusHandler* status = NULL);
        static void       FromString(const std::string& xml, V_Element& elements, StatusHandler* status = NULL);

    };
}
