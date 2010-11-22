#pragma once

#include "Platform/Types.h"
#include "Platform/Directory.h"

#include "Foundation/API.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/File/Path.h"

namespace Helium
{
    namespace DirectoryFlags
    {
        enum Flags
        {
            SkipFiles         = 1 << 0,          // Skip files
            SkipDirectories   = 1 << 1,          // Skip directories
            RelativePath      = 1 << 3,          // Don't preped each file with the root path
        };

        const uint32_t Default = 0;
    }

    struct FOUNDATION_API DirectoryItem
    {
        DirectoryItem()
            : m_CreateTime ( 0x0 )
            , m_ModTime ( 0x0 )
            , m_Size( 0x0 )
        {

        }

        void Clear()
        {
            m_Path.Clear();
            m_CreateTime = 0x0;
            m_ModTime = 0x0;
            m_Size = 0x0;
        }

        Path      m_Path;
        uint64_t  m_CreateTime;
        uint64_t  m_ModTime;
        uint64_t  m_Size;
    };

    class FOUNDATION_API Directory
    {
    public:
        Directory();
        Directory( const Path& path, uint32_t flags = DirectoryFlags::Default );
        ~Directory();

        bool IsDone();
        bool Next();
        const DirectoryItem& GetItem();

        void Reset();
        bool Open(const Path& path, uint32_t flags = DirectoryFlags::Default);

        static void GetFiles( const Path& path, std::set< Helium::Path >& paths, bool recursive = false );
        void GetFiles( std::set< Helium::Path >& paths, bool recursive = false );

    private:
        bool Find();
        void Close();

        Path             m_Path;
        uint32_t         m_Flags;
        DirectoryHandle  m_Handle;
        DirectoryItem    m_Item;
        bool             m_Done;
    };

    typedef Helium::Signature< const DirectoryItem& > DirectoryItemSignature;

    FOUNDATION_API void RecurseDirectories( DirectoryItemSignature::Delegate delegate, const Path& path, uint32_t flags = DirectoryFlags::Default);
}