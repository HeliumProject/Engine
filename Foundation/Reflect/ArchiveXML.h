#pragma once

#include "Indent.h"
#include "Archive.h"

struct XML_ParserStruct;
typedef struct XML_ParserStruct *XML_Parser;

namespace Helium
{
    namespace Reflect
    {
        //
        // XML Archive Class
        //

        class FOUNDATION_API ArchiveXML : public Archive
        {
        public: 
            static const uint32_t CURRENT_VERSION; 

        private:
            friend class Archive;

            class State
            {
            public:
#if HELIUM_DEBUG
                // the name of the type being processed
                tstring m_Type;
#endif

                // the body of the element, cdata section
                tstring m_Body;

                // the current serializing field
                const Field* m_Field;

                // the type of the instance being processed
                const Composite* m_Composite;

                // the instance being processed (for structure support)
                void* m_Instance;

                // the object being processed (could be the same as instance)
                ObjectPtr m_Object;

                // objects stashed at this parsing data
                std::vector< ObjectPtr > m_Stash;

                State()
                    : m_Field( NULL )
                    , m_Composite( NULL )
                    , m_Instance( NULL )
                {

                }
            };

            // File format version
            uint32_t m_Version;

            // The expat parser object
            XML_Parser m_Parser;

            // The stream to use
            TCharStreamPtr m_Stream;

            // Indent helper
            Indent<tchar_t> m_Indent;

            // The nesting stack of parsing state
            std::stack< State > m_States;

        public:
            ArchiveXML( const Path& path, ByteOrder byteOrder = Helium::PlatformByteOrder );
            ~ArchiveXML();

        private:
            ArchiveXML();

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

            virtual void Open( bool write = false) HELIUM_OVERRIDE;
            void OpenStream(TCharStream* stream, bool write = false);
            virtual void Close(); 

            // Begins parsing the InputStream
            virtual void Read();

            // Write to the OutputStream
            virtual void Write();

        public:
            // Access indentation
            Indent<tchar_t>& GetIndent()
            {
                return m_Indent;
            }

        public:
            // Serialize
            virtual void Serialize( Object* object );
            void Serialize( Object* object, const tchar_t* fieldName );
            virtual void Serialize( void* structure, const Structure* type );
            void Serialize( void* structure, const Structure* type, const tchar_t* fieldName );
            virtual void Serialize( const std::vector< ObjectPtr >& elements, uint32_t flags = 0 );
            virtual void Serialize( const DynArray< ObjectPtr >& elements, uint32_t flags = 0 );

        protected:
            // Helpers
            template< typename ConstIteratorType > void Serialize( ConstIteratorType begin, ConstIteratorType end, uint32_t flags );
            void SerializeFields( Object* object );
            void SerializeFields( void* structure, const Structure* type );

        public:
            // For handling components
            virtual void Deserialize( ObjectPtr& object );
            virtual void Deserialize( void* structure, const Structure* type );
            virtual void Deserialize( std::vector< ObjectPtr >& elements, uint32_t flags = 0 );
            virtual void Deserialize( DynArray< ObjectPtr >& elements, uint32_t flags = 0 );

        private:
            static void StartElementHandler(void *pUserData, const tchar_t* pszName, const tchar_t **papszAttrs)
            {
                ArchiveXML *archive = (ArchiveXML *)pUserData;
                archive->OnStartElement(pszName, papszAttrs);
            }

            static void EndElementHandler(void *pUserData, const tchar_t* pszName)
            {
                ArchiveXML *archive = (ArchiveXML *)pUserData;
                archive->OnEndElement(pszName);
            }

            static void CharacterDataHandler(void *pUserData, const tchar_t* pszData, int nLength)
            {
                ArchiveXML *archive = (ArchiveXML *)pUserData;
                archive->OnCharacterData(pszData, nLength);
            }

            // Called on <object>
            void OnStartElement(const tchar_t *pszName, const tchar_t **papszAttrs);

            // Called between <object> and </object>
            void OnCharacterData(const tchar_t *pszData, int nLength);

            // Called after </object>
            void OnEndElement(const tchar_t *pszName);

        public:
            // Reading and writing single object from string data
            static void       ToString( Object* object, tstring& xml );
            static ObjectPtr FromString( const tstring& xml, const Class* searchClass = NULL );

            // Reading and writing multiple elements from string data
            static void       ToString( const std::vector< ObjectPtr >& elements, tstring& xml );
            static void       FromString( const tstring& xml, std::vector< ObjectPtr >& elements );
        };
    }
}