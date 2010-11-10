#pragma once

#include "Foundation/API.h"

#if HELIUM_OS_WIN

#include "Foundation/Stream/FileStream.h"

#pragma TODO( "LUNAR MERGE - Address dependency on Windows.h" )
#include "Platform/Windows/Windows.h"

namespace Helium
{
    /// Windows file stream implementation.
    class FOUNDATION_API FileStreamWin : public FileStream
    {
    public:
        /// @name Construction/Destruction
        //@{
        FileStreamWin();
        virtual ~FileStreamWin();
        //@}

        /// @name Stream Interface
        //@{
        void Close();
        bool IsOpen() const;

        size_t Read( void* pBuffer, size_t size, size_t count );
        size_t Write( const void* pBuffer, size_t size, size_t count );

        void Flush();

        int64_t Seek( int64_t offset, ESeekOrigin origin );
        int64_t Tell() const;
        int64_t GetSize() const;
        //@}

    private:
        /// Open file handle.
        HANDLE m_hFile;

        /// @name File Opening Implementation
        //@{
        bool OpenActual( const tchar_t* pPath, uint32_t modeFlags, bool bTruncate );
        //@}
    };
}

#endif  // HELIUM_OS_WIN
