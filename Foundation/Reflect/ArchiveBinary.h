#pragma once

#include "Archive.h"

//  
//    Reflect Binary Format:
//  
//    struct Serializer
//    {
//      i32 type;             // string pool index of the short name of the serializer
//      byte[] data;          // serialized data
//    };
//  
//    struct Field
//    {
//      i32 field_id;         // latent RTTI field index (id)
//      Serializer ser;       // serializer instance data
//    };
//  
//    struct Element
//    {
//      i32 type;             // string pool index of the short name of the element
//      i32 field_count;      // number of serialized fields
//      Field[] fields;       // field instance data
//      i32 term;             // -1
//    };
//  
//    struct Array
//    {
//      i32 count;            // count of contained elements
//      Element[] elements;   // element instance data
//      i32 term;             // -1
//    };
//  
//    struct File
//    {
//      char file_id;         // '!'
//  
//      u32 crc;              // crc of all bytes following the crc value itself
//    |-i32 type_offet;       // offset into file for the beginning of the rtti block
//  |-+-i32 string_offset;    // offset into file for the beginning of the global string pool
//  | |
//  | | Array spool;          // spooled data from client
//  | | Array append;         // appended session data
//  | |
//  | ->i32 type_count;       // number of types stored
//  |   Structure[] types;    // see Class.h for details
//  |   i32 type_term;        // -1
//  |
//  --->StringPool strings;   // see StringPool.h for details
//    };
//  

namespace Reflect
{
    //
    // Binary Archive Class
    //

    typedef std::map< int, Nocturnal::SmartPtr<const Class> > M_IDToClass;
    typedef std::map< std::string, Nocturnal::SmartPtr<const Class> > M_StrToClass;

    class FOUNDATION_API ArchiveBinary : public Archive
    {
    public: 
        static const u32 CURRENT_VERSION; 
        static const u32 FIRST_VERSION_WITH_ARRAY_COMPRESSION; 
        static const u32 FIRST_VERSION_WITH_STRINGPOOL_COMPRESSION; 
        static const u32 FIRST_VERSION_WITH_POINTER_SERIALIZER; 

    private:
        friend class Archive;

        // The stream to use
        CharStreamPtr m_Stream;

        // The strings to cache for binary modes
        StringPool m_Strings;

        // Latent types by latent ids
        M_IDToClass m_ClassesByID;

        // Latent types by latent short name
        M_StrToClass m_ClassesByShortName;

        // Mapping from CURRENT short name to LEGACY short name
        std::map< std::string, std::string > m_ShortNameMapping;

        // File format version
        u32 m_Version;

        // File size
        long m_Size;

        // Skip flag
        bool m_Skip;

        // Data for the current field we are writing
        struct WriteFields
        {
            i32            m_Count;
            std::streamoff m_CountOffset;
        };

        // The stack of fields we are writing
        std::stack<WriteFields> m_FieldStack;

    private:
        ArchiveBinary (StatusHandler* status = NULL);

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

        u32 GetVersion()
        {
            return m_Version; 
        }

    protected:
        // The type
        virtual ArchiveType GetType() const
        {
            return ArchiveTypes::Binary;
        }

        virtual void OpenFile(const tstring& file, bool write = false);
        void OpenStream(CharStream* stream, bool write = false);
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
        virtual void Serialize(const ElementPtr& element);
        virtual void Serialize(const V_Element& elements, u32 flags = 0);

    protected:
        // Helpers
        void SerializeFields(const ElementPtr& element);
        void SerializeField(const ElementPtr& element, const Field* field);

    private:
        // pulls an element from the head of the stream
        ElementPtr Allocate();

    public:
        // pulls from the stream, or deserializes into a freshly allocated instance
        virtual void Deserialize(ElementPtr& element);
        virtual void Deserialize(V_Element& elements, u32 flags = 0);

    protected:
        // Helpers
        void DeserializeFields(const ElementPtr& element);
        void DeserializeField(const ElementPtr& element, const Field* latent_field);

        // Reflection Helpers
        void SerializeComposite(const Composite* composite);
        bool DeserializeComposite(Composite* composite);
        void SerializeField(const Field* field);
        bool DeserializeField(Field* field);

    public:
        // Reading and writing single element via binary
        static void       ToStream(const ElementPtr& element, std::iostream& stream, StatusHandler* status = NULL);
        static ElementPtr FromStream(std::iostream& stream, int searchType = Reflect::ReservedTypes::Any, StatusHandler* status = NULL);

        // Reading and writing multiple elements via binary
        static void       ToStream(const V_Element& elements, std::iostream& stream, StatusHandler* status = NULL);
        static void       FromStream(std::iostream& stream, V_Element& elements, StatusHandler* status = NULL);

    };
}
