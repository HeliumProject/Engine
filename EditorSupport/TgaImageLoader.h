//----------------------------------------------------------------------------------------------------------------------
// TgaImageLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_EDITOR_SUPPORT_TGA_IMAGE_LOADER_H
#define LUNAR_EDITOR_SUPPORT_TGA_IMAGE_LOADER_H

#include "EditorSupport/EditorSupport.h"

namespace Helium
{
    class Stream;
}

namespace Lunar
{
    class Image;

    /// TGA image loading support.
    class LUNAR_EDITOR_SUPPORT_API TgaImageLoader
    {
    public:
        /// @name Loading
        //@{
        static bool Load( Image& rImage, Stream* pSourceStream );
        //@}
    };
}

#endif  // LUNAR_EDITOR_SUPPORT_TGA_IMAGE_LOADER_H
