#pragma once

#include "Platform/Types.h"
#include "Platform/Exception.h"

#include "Foundation/API.h"
#include "Foundation/Memory/SmartPtr.h"

#include "Path.h"

namespace Helium
{
    class FileOperationException : public Helium::Exception
    {
    public:
        FileOperationException( const tchar_t *msgFormat, ... ) : Exception()
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

    typedef int64_t FileLocation;

    class HELIUM_FOUNDATION_API FileHandle : public Helium::RefCountBase< FileHandle >
    {
    public:
        FileHandle( const tstring& path, const tchar_t* mode = TXT( "r" ) );

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

        uint32_t Read( uint8_t* buffer, size_t amount );
        void Write( uint8_t* buffer, size_t amount );

        FileLocation Tell();
        void Seek( FileLocation location, FileOffset offsetType = FileOffsets::Beginning );

        const Path& GetPath() const
        {
            return m_Path;
        }

        const tchar_t* GetMode() const
        {
            return m_Mode;
        }

        FILE* Get() const
        {
            return m_FileHandle;
        }

    private:
        uint32_t  m_OpenCount;
        Path m_Path;
        const tchar_t* m_Mode;
        FILE* m_FileHandle;
    };

    typedef Helium::SmartPtr< FileHandle > FileHandlePtr;
}
