#pragma once

#include "Platform/Types.h"
#include "Platform/Exception.h"

#include "Foundation/API.h"
#include "Foundation/Memory/SmartPtr.h"

#include "Path.h"

namespace Nocturnal
{
    class FileOperationException : public Nocturnal::Exception
    {
    public:
        FileOperationException( const tchar *msgFormat, ... ) : Exception()
        {
            va_list msgArgs;
            va_start( msgArgs, msgFormat );
            SetMessage( msgFormat, msgArgs );
            va_end( msgArgs );
        }
    };

    namespace FileOffsets
    {
        enum FileOffset
        {
            Beginning,
            Current,
            End,
        };
    }
    typedef FileOffsets::FileOffset FileOffset;

    typedef i64 FileLocation;

    class FOUNDATION_API FileHandle : public Nocturnal::RefCountBase< FileHandle >
    {
    public:
        FileHandle( const tstring& path, const tchar* mode = TXT( "r" ) );

        virtual ~FileHandle()
        {
            // we don't just call Close() because it is linked to Open() calls
            if ( m_FileHandle )
            {
                fclose( m_FileHandle );
            }
        }

        bool IsValid();

        bool Open();
        void Close();

        u32 Read( u8* buffer, size_t amount );
        void Write( u8* buffer, size_t amount );

        FileLocation Tell();
        void Seek( FileLocation location, FileOffset offsetType = FileOffsets::Beginning );

        const Path& GetPath() const
        {
            return m_Path;
        }

        const tchar* GetMode() const
        {
            return m_Mode;
        }

        FILE* Get() const
        {
            return m_FileHandle;
        }

    private:
        u32  m_OpenCount;
        Path m_Path;
        const tchar* m_Mode;
        FILE* m_FileHandle;
    };

    typedef Nocturnal::SmartPtr< FileHandle > FileHandlePtr;
}
