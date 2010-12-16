#pragma once

#include "Indent.h"
#include "Archive.h"

#include "Foundation/File/Path.h"

//  
//    Reflect Binary Format:
//  
//    struct Data
//    {
//      int32_t type;           // string pool index of the name of the serializer
//      byte[] data;            // serialized data
//    };
//  
//    struct Field
//    {
//      int32_t field_id;       // latent type field index (id)
//      Data ser;               // serializer instance data
//    };
//  
//    struct Element
//    {
//      int32_t type;           // string pool index of the name of the element
//      int32_t field_count;    // number of serialized fields
//      Field[] fields;         // field instance data
//      int32_t term;           // -1
//    };
//  
//    struct ElementArray
//    {
//      int32_t count;          // count of contained elements
//      Element[] elements;     // element instance data
//      int32_t term;           // -1
//    };
//  
//    struct File
//    {
//      uint8_t byte_order;     // BOM
//      uint8_t encoding;       // character encoding
//      uint32_t version;       // file format version
//      uint32_t crc;           // crc of all bytes following the crc value itself
//
//      ElementArray elements;  // client objects
//    };
//

namespace Helium
{
    namespace Reflect
    {
        //
        // Binary Archive Class
        //

        typedef std::map< Name, Helium::SmartPtr< const Class > > M_NameToClass;

        class FOUNDATION_API ArchiveBinary : public Archive
        {
        public: 
            static const uint32_t CURRENT_VERSION; 

        private:
            friend class Archive;

            // The stream to use
            CharStreamPtr m_Stream;

#ifdef REFLECT_ARCHIVE_VERBOSE
            // Indent helper
            Indent<tchar_t> m_Indent;
#endif

            // File format version
            uint32_t m_Version;

            // File size
            long m_Size;

            // Skip flag
            bool m_Skip;

            // Data for the current field we are writing
            struct WriteFields
            {
                int32_t         m_Count;
                std::streamoff  m_CountOffset;
            };

            // The stack of fields we are writing
            std::stack<WriteFields> m_FieldStack;

        public:
            ArchiveBinary( const Path& path, ByteOrder byteOrder = Helium::PlatformByteOrder );

        private:
            ArchiveBinary();

        public:
            CharStream& GetStream()
            {
                return *m_Stream;
            }

            uint32_t GetVersion()
            {
                return m_Version; 
            }

        protected:
            // The type
            virtual ArchiveType GetType() const
            {
                return ArchiveTypes::Binary;
            }

            virtual void Open( bool write = false ) HELIUM_OVERRIDE;
            void OpenStream( CharStream* stream, bool write = false );
            virtual void Close(); 

            // Begins parsing the InputStream
            virtual void Read();

            // Write to the OutputStream
            virtual void Write();

        public:
            // Serialize
            virtual void Serialize( const ElementPtr& element );
            virtual void Serialize( const std::vector< ElementPtr >& elements, uint32_t flags = 0 );

        protected:
            // Helpers
            void SerializeFields( const ElementPtr& element );

        private:
            // pulls an element from the head of the stream
            ElementPtr Allocate();

        public:
            // pulls from the stream, or deserializes into a freshly allocated instance
            virtual void Deserialize( ElementPtr& element );
            virtual void Deserialize( std::vector< ElementPtr >& elements, uint32_t flags = 0 );

        protected:
            // Helpers
            void DeserializeFields( const ElementPtr& element );

        public:
            // Reading and writing single element via binary
            static void       ToStream( const ElementPtr& element, std::iostream& stream );
            static ElementPtr FromStream( std::iostream& stream, const Class* searchClass = NULL );

            // Reading and writing multiple elements via binary
            static void       ToStream( const std::vector< ElementPtr >& elements, std::iostream& stream );
            static void       FromStream( std::iostream& stream, std::vector< ElementPtr >& elements );
        };
    }
}