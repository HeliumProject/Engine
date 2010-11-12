#pragma once

#include "Foundation/Stream/Stream.h"

namespace Helium
{
    /// File stream base class.
    class FOUNDATION_API FileStream : public Stream
    {
    public:
        /// File access mode flags.
        enum EMode
        {
            MODE_READ  = ( 1 << 0 ),  ///< Read access.
            MODE_WRITE = ( 1 << 1 ),  ///< Write access.
        };

        /// @name Construction/Destruction
        //@{
        FileStream();
        virtual ~FileStream() = 0;
        //@}

        /// @name File Opening
        //@{
        bool Open( const tchar_t* pPath, uint32_t modeFlags, bool bTruncate = true );
        //@}

        /// @name Stream Capabilities
        //@{
        virtual bool CanRead() const;
        virtual bool CanWrite() const;
        virtual bool CanSeek() const;
        //@}

    protected:
        /// Access mode flags.
        uint32_t m_modeFlags;

        /// @name File Opening Implementation
        //@{
        virtual bool OpenActual( const tchar_t* pPath, uint32_t modeFlags, bool bTruncate ) = 0;
        //@}
    };
}
