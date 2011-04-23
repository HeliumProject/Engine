#pragma once

#include "Indent.h"
#include "Archive.h"

#include "Foundation/File/Path.h"

//  
//    Reflect Binary Format:
//  
//    struct Data
//    {
//      int32_t type;           // string pool index of the name of the data
//      byte[] data;            // serialized data
//    };
//  
//    struct Field
//    {
//      int32_t field_id;       // latent type field index (name crc)
//      Data ser;               // data instance data
//    };
//  
//    struct Object
//    {
//      int32_t type;           // string pool index of the name of the object
//      int32_t field_count;    // number of serialized fields
//      Field[] fields;         // field instance data
//      int32_t term;           // -1
//    };
//  
//    struct ObjectArray
//    {
//      int32_t count;          // count of contained objects
//      Object[] objects;       // object instance data
//      int32_t term;           // -1
//    };
//  
//    struct File
//    {
//      uint8_t byte_order;     // BOM
//      uint8_t encoding;       // character encoding
//      uint32_t version;       // file format version
//
//      ObjectArray objects;    // client objects
//    };
//

namespace Helium
{
    namespace Reflect
    {
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
            std::streamoff m_Size;

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
            virtual void Close() HELIUM_OVERRIDE; 

            // Begins parsing the InputStream
            virtual void Read() HELIUM_OVERRIDE;

            // Write to the OutputStream
            virtual void Write() HELIUM_OVERRIDE;

        public:
            // Serialize
            void SerializeInstance( Object* object );
            void SerializeInstance( void* structure, const Structure* type );
            void SerializeFields( Object* object );
            void SerializeFields( void* structure, const Structure* type );
            void SerializeArray( const std::vector< ObjectPtr >& objects, uint32_t flags = 0 );
            void SerializeArray( const DynArray< ObjectPtr >& objects, uint32_t flags = 0 );

        protected:
            // Helpers
            template< typename ConstIteratorType >
            void SerializeArray( ConstIteratorType begin, ConstIteratorType end, uint32_t flags );

        public:
            // pulls from the stream, or deserializes into a freshly allocated instance
            void DeserializeInstance( ObjectPtr& object );
            void DeserializeInstance( void* structure, const Structure* type );
            void DeserializeFields( Object* object );
            void DeserializeFields( void* object, const Structure* type );
            void DeserializeArray( std::vector< ObjectPtr >& objects, uint32_t flags = 0 );
            void DeserializeArray( DynArray< ObjectPtr >& objects, uint32_t flags = 0 );

        protected:
            // Helpers
            template< typename ArrayPusher >
            void DeserializeArray( ArrayPusher& push, uint32_t flags );
            ObjectPtr Allocate();

		public:
            // Reading and writing single object via binary
            static void       ToStream( Object* object, std::iostream& stream );
            static ObjectPtr FromStream( std::iostream& stream, const Class* searchClass = NULL );

            // Reading and writing multiple objects via binary
            static void       ToStream( const std::vector< ObjectPtr >& objects, std::iostream& stream );
            static void       FromStream( std::iostream& stream, std::vector< ObjectPtr >& objects );
        };
    }
}