#pragma once

#include "EditorSupport/EditorSupport.h"

#if HELIUM_TOOLS

namespace Helium
{
    class Stream;
}

namespace Helium
{
    class Image;

    /// TGA image loading support.
    class HELIUM_EDITOR_SUPPORT_API TgaImageLoader
    {
    public:
        /// @name Loading
        //@{
        static bool Load( Image& rImage, Stream* pSourceStream );
        //@}
    };
}

#endif  // HELIUM_TOOLS
