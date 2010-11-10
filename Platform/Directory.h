#pragma once

#include "API.h"

#include "Platform/Types.h"

namespace Helium
{
    typedef void* DirectoryHandle;

    namespace FileAttributes
    {
        enum FileAttribute
        {
            ReadOnly     = 1 << 0,
            Hidden       = 1 << 1,
            System       = 1 << 2,
            Directory    = 1 << 3,
            Archive      = 1 << 4,
            Device       = 1 << 5,
            Normal       = 1 << 6,
            Temporary    = 1 << 7,
            Sparse       = 1 << 8,
            Redirect     = 1 << 9,
            Compressed   = 1 << 10,
            Offline      = 1 << 11,
            NonIndexed   = 1 << 12,
            Encrypted    = 1 << 13,
            Virtual      = 1 << 14
        };
    }
    typedef FileAttributes::FileAttribute FileAttribute;

    struct FileFindData
    {
        FileFindData()
            : m_FileAttributes( 0 )
            , m_CreationTime( 0 )
            , m_AccessTime( 0 )
            , m_ModificationTime( 0 )
            , m_FileSize( 0 )
        {
        }

        uint32_t m_FileAttributes;
        uint64_t m_CreationTime;
        uint64_t m_AccessTime;
        uint64_t m_ModificationTime;
        uint64_t m_FileSize;
        tstring  m_Filename;
    };

    PLATFORM_API DirectoryHandle FindFirst( const tstring& spec, FileFindData& data );
    PLATFORM_API bool FindNext( DirectoryHandle& handle, FileFindData& data );
    PLATFORM_API bool CloseFind( DirectoryHandle& handle );
}