#pragma once

#include "Indent.h"
#include "Archive.h"

#include "Foundation/File/Path.h"

//  
//    Reflect Binary Format:
//  
//    struct Serializer
//    {
//      int32_t type;             // string pool index of the short name of the serializer
//      byte[] data;          // serialized data
//    };
//  
//    struct Field
//    {
//      int32_t field_id;         // latent RTTI field index (id)
//      Serializer ser;       // serializer instance data
//    };
//  
//    struct Element
//    {
//      int32_t type;             // string pool index of the short name of the element
//      int32_t field_count;      // number of serialized fields
//      Field[] fields;       // field instance data
//      int32_t term;             // -1
//    };
//  
//    struct Array
//    {
//      int32_t count;            // count of contained elements
//      Element[] elements;   // element instance data
//      int32_t term;             // -1
//    };
//  
//    struct File
//    {
//      char file_id;         // '!'
//  
//      uint32_t crc;              // crc of all bytes following the crc value itself
//    |-int32_t type_offet;       // offset into file for the beginning of the rtti block
//  |-+-int32_t string_offset;    // offset into file for the beginning of the global string pool
//  | |
//  | | Array spool;          // spooled data from client
//  | | Array append;         // appended session data
//  | |
//  | ->int32_t type_count;       // number of types stored
//  |   Structure[] types;    // see Class.h for details
//  |   int32_t type_term;        // -1
//  |
//  --->StringPool strings;   // see StringPool.h for details
//    };
//  

namespace Helium
{
    namespace Reflect
    {
        //
        // Binary Archive Class
        //

        typedef std::map< int, Helium::SmartPtr<const Class> > M_IDToClass;
        typedef std::map< tstring, Helium::SmartPtr<const Class> > M_StrToClass;

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
            Indent<tchar> m_Indent;
#endif

            // The strings to cache for binary modes
            StringPool m_Strings;

            // Latent types by latent ids
            M_IDToClass m_ClassesByID;

            // Latent types by latent short name
            M_StrToClass m_ClassesByShortName;

            // Mapping from CURRENT short name to LEGACY short name
            std::map< tstring, tstring > m_ShortNameMapping;

            // File format version
            uint32_t m_Version;

            // File size
            long m_Size;

            // Skip flag
            bool m_Skip;

            // Data for the current field we are writing
            struct WriteFields
            {
                int32_t            m_Count;
                std::streamoff m_CountOffset;
            };

            // The stack of fields we are writing
            std::stack<WriteFields> m_FieldStack;

        public:
            ArchiveBinary( const Path& path, ByteOrder byteOrder = ByteOrders::Unknown );

        private:
            ArchiveBinary();

        public:
            // Stream access
            CharStream& GetStream()
            {
                return *m_Stream;
            }

            // Strings access
            StringPool& GetStrings()
            {
                return m_Strings;
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

            // Write the file header
            virtual void Start();

            // Write the file footer
            virtual void Finish();

        public:
            // Serialize
            virtual void Serialize( const ElementPtr& element );
            virtual void Serialize( const V_Element& elements, uint32_t flags = 0 );

        protected:
            // Helpers
            void SerializeFields( const ElementPtr& element );
            void SerializeField( const ElementPtr& element, const Field* field );

        private:
            // pulls an element from the head of the stream
            ElementPtr Allocate();

        public:
            // pulls from the stream, or deserializes into a freshly allocated instance
            virtual void Deserialize( ElementPtr& element );
            virtual void Deserialize( V_Element& elements, uint32_t flags = 0 );

        protected:
            // Helpers
            void DeserializeFields( const ElementPtr& element );
            void DeserializeField( const ElementPtr& element, const Field* latent_field );

            // Reflection Helpers
            void SerializeComposite( const Composite* composite );
            bool DeserializeComposite( Composite* composite );
            void SerializeField( const Field* field );
            bool DeserializeField( Field* field );

        public:
            // Reading and writing single element via binary
            static void       ToStream( const ElementPtr& element, std::iostream& stream );
            static ElementPtr FromStream( std::iostream& stream, int searchType = Reflect::ReservedTypes::Any );

            // Reading and writing multiple elements via binary
            static void       ToStream( const V_Element& elements, std::iostream& stream );
            static void       FromStream( std::iostream& stream, V_Element& elements );
        };
    }
}