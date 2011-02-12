#pragma once

#include "Foundation/XMLDocument.h"
#include "Foundation/Reflect/Indent.h"
#include "Foundation/Reflect/Archive.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API ArchiveXML : public Archive
        {
        public: 
            static const uint32_t CURRENT_VERSION; 

        private:
            friend class Archive;

            // File format version
            uint32_t m_Version;

            // File size
            std::streamoff m_Size;

            // Skip flag
            bool m_Skip;

            // The xml data
            XMLDocument m_Document;

            // The current element
            XMLElement* m_Current;

            // The stream to use
            TCharStreamPtr m_Stream;

            // Indentation helper
            Indent<tchar_t> m_Indent;

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

            virtual void Open( bool write = false ) HELIUM_OVERRIDE;
            void OpenStream(TCharStream* stream, bool write = false );
            virtual void Close() HELIUM_OVERRIDE; 

            // Begins parsing the InputStream
            virtual void Read() HELIUM_OVERRIDE;

            // Write to the OutputStream
            virtual void Write() HELIUM_OVERRIDE;

        public:
            // Access indentation
            Indent<tchar_t>& GetIndent()
            {
                return m_Indent;
            }

        public:
            // Serialize
            virtual void Serialize( Object* object ) HELIUM_OVERRIDE;
            virtual void Serialize( void* structure, const Structure* type ) HELIUM_OVERRIDE;
            virtual void Serialize( const std::vector< ObjectPtr >& elements, uint32_t flags = 0 ) HELIUM_OVERRIDE;
            virtual void Serialize( const DynArray< ObjectPtr >& elements, uint32_t flags = 0 ) HELIUM_OVERRIDE;

        protected:
            // Helpers
            void Serialize( Object* object, const tchar_t* fieldName );
            void Serialize( void* structure, const Structure* type, const tchar_t* fieldName );

            template< typename ConstIteratorType >
            void Serialize( ConstIteratorType begin, ConstIteratorType end, uint32_t flags );
            void SerializeFields( Object* object );
            void SerializeFields( void* structure, const Structure* type );

        public:
            // Deserialize
            virtual void Deserialize( ObjectPtr& object ) HELIUM_OVERRIDE;
            virtual void Deserialize( void* structure, const Structure* type ) HELIUM_OVERRIDE;
            virtual void Deserialize( std::vector< ObjectPtr >& elements, uint32_t flags = 0 ) HELIUM_OVERRIDE;
            virtual void Deserialize( DynArray< ObjectPtr >& elements, uint32_t flags = 0 ) HELIUM_OVERRIDE;

        protected:
            // Helpers
            ObjectPtr Allocate();
            template< typename ArrayPusher >
            void Deserialize( ArrayPusher& push, uint32_t flags );
            void DeserializeFields( Object* object );
            void DeserializeFields( void* object, const Structure* type );

        public:
            // Reading and writing single object from string data
            static void       ToString( Object* object, tstring& xml );
            static ObjectPtr  FromString( const tstring& xml, const Class* searchClass = NULL );

            // Reading and writing multiple elements from string data
            static void       ToString( const std::vector< ObjectPtr >& elements, tstring& xml );
            static void       FromString( const tstring& xml, std::vector< ObjectPtr >& elements );
        };
    }
}