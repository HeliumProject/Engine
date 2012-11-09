#pragma once

#include "Platform/File.h"

#include "Foundation/Stream.h"
#include "Foundation/String.h"

namespace Helium
{
    /// File stream base class.
    class HELIUM_FOUNDATION_API FileStream : public Stream
    {
    public:
        /// @name Convenience
        //@{
        static FileStream* OpenFileStream( const tchar_t* pPath, uint32_t modeFlags, bool bTruncate = true );
        static FileStream* OpenFileStream( const String& rPath, uint32_t modeFlags, bool bTruncate = true );
        //@}

        /// File access mode flags.
        enum EMode
        {
            MODE_READ  = ( 1 << 0 ),  ///< Read access.
            MODE_WRITE = ( 1 << 1 ),  ///< Write access.
        };

        /// @name Construction/Destruction
        //@{
        FileStream();
        virtual ~FileStream();
        //@}

        /// Open a file.
        ///
        /// @param[in] pPath      FilePath name of the file to open.
        /// @param[in] modeFlags  Combination of EMode flags specifying the mode in which to open the file.
        /// @param[in] bTruncate  If the MODE_WRITE flag is set, true to truncate any existing file, false to append to any
        ///                       existing file.  This is ignored if MODE_WRITE is not set.
        ///
        /// @return  True if the file was successfully opened, false if not.
        ///
        /// @see Close(), IsOpen()
        bool Open( const tchar_t* pPath, uint32_t modeFlags, bool bTruncate = true );

        /// @copydoc Stream::Close()
        virtual void Close();

        /// @copydoc Stream::IsOpen()
        virtual bool IsOpen() const;

        /// @copydoc Stream::Read()
        virtual size_t Read( void* pBuffer, size_t size, size_t count );

        /// @copydoc Stream::Write()
        virtual size_t Write( const void* pBuffer, size_t size, size_t count );

        /// @copydoc Stream::Flush()
        virtual void Flush();

        /// @copydoc Stream::Seek()
        virtual int64_t Seek( int64_t offset, SeekOrigin origin );

        /// @copydoc Stream::Tell()
        virtual int64_t Tell() const;

        /// @copydoc Stream::GetSize()
        virtual int64_t GetSize() const;

        /// @name Stream Capabilities
        //@{
        
        /// @copydoc Stream::CanRead()
        virtual bool FileStream::CanRead() const
        {
            return( IsOpen() && ( m_modeFlags & MODE_READ ) != 0 );
        }

        /// @copydoc Stream::CanWrite()
        virtual bool FileStream::CanWrite() const
        {
            return( IsOpen() && ( m_modeFlags & MODE_WRITE ) != 0 );
        }

        /// @copydoc Stream::CanSeek()
        virtual bool FileStream::CanSeek() const
        {
            return IsOpen();
        }
        //@}

    protected:
        /// Access mode flags.
        uint32_t m_modeFlags;
        
        /// handle to the stream
        File m_File;
    };
}
