//----------------------------------------------------------------------------------------------------------------------
// PngImageLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_EDITOR_SUPPORT_PNG_IMAGE_LOADER_H
#define HELIUM_EDITOR_SUPPORT_PNG_IMAGE_LOADER_H

#include "EditorSupport/EditorSupport.h"

#if HELIUM_TOOLS

namespace Helium
{
    class Stream;
}

namespace Helium
{
    class Image;

    /// PNG image loading support.
    class HELIUM_EDITOR_SUPPORT_API PngImageLoader
    {
    public:
        /// @name Loading
        //@{
        static bool Load( Image& rImage, Stream* pSourceStream );
        //@}
    };
}

#endif  // HELIUM_TOOLS

#endif  // HELIUM_EDITOR_SUPPORT_PNG_IMAGE_LOADER_H
