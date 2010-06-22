#pragma once

#include "Platform/Types.h"

#include "Foundation/API.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/File/Path.h"

namespace Nocturnal
{
    namespace DirectoryFlags
    {
        enum Flags
        {
            SkipFiles         = 1 << 0,          // Skip files
            SkipDirectories   = 1 << 1,          // Skip directories
            RelativePath      = 1 << 3,          // Don't preped each file with the root path
        };

        const u32 Default = 0;
    }

    typedef void* DirectoryHandle;

    namespace DirectoryItemFlags
    {
        enum Flags
        {
            Directory   = 1 << 1,                 // It's actually a directory
        };

        const u32 Default = 0;
    }

    struct FOUNDATION_API DirectoryItem
    {
        DirectoryItem()
            : m_CreateTime ( 0x0 )
            , m_ModTime ( 0x0 )
            , m_Size( 0x0 )
            , m_Flags ( 0x0 )
        {

        }

        void Clear()
        {
            m_Path.clear();
            m_CreateTime = 0x0;
            m_ModTime = 0x0;
            m_Size = 0x0;
        }

        std::string         m_Path;
        u64                 m_CreateTime;
        u64                 m_ModTime;
        u64                 m_Size;
        u32                 m_Flags;
    };

    class FOUNDATION_API Directory
    {
    public:
        Directory();
        Directory(const std::string &path, const std::string &spec = "*.*", u32 flags = DirectoryFlags::Default);
        ~Directory();

        bool IsDone();
        bool Next();
        const DirectoryItem& GetItem();

        void Reset();
        bool Open(const std::string &path, const std::string &spec = "*.*", u32 flags = DirectoryFlags::Default);

        static void GetFiles( const std::string& path, std::set< Nocturnal::Path >& paths, const std::string& spec = "*.*", bool recursive = false );
        void GetFiles( std::set< Nocturnal::Path >& paths, const std::string& spec = "*.*", bool recursive = false );

    private:
        bool Find(const std::string& query = "");
        void Close();

        std::string         m_Path;
        std::string         m_Spec;
        u32                 m_Flags;
        DirectoryHandle     m_Handle;
        DirectoryItem       m_Item;
        bool                m_Done;
    };

    typedef Nocturnal::Signature<void, const DirectoryItem&> DirectoryItemSignature;

    FOUNDATION_API void RecurseDirectories( DirectoryItemSignature::Delegate delegate, const std::string &path, const std::string &spec = "*.*", u32 flags = DirectoryFlags::Default);
}