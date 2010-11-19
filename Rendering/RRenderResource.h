//----------------------------------------------------------------------------------------------------------------------
// RRenderResource.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_R_RENDER_RESOURCE_H
#define LUNAR_RENDERING_R_RENDER_RESOURCE_H

#include "Rendering/Rendering.h"
#include "Foundation/Memory/SmartPtr.h"

/// Forward declare a render resource smart pointer type.
///
/// @param[in] CLASS  Class for which to declare the smart pointer type.
#define L_DECLARE_RPTR( CLASS ) class CLASS; typedef Helium::SmartPtr< CLASS > CLASS##Ptr;

namespace Lunar
{
    /// Base class for all reference-counted render resources.
    class LUNAR_RENDERING_API RRenderResource : public AtomicRefCountBase< RRenderResource >, NonCopyable
    {
        friend class AtomicRefCountBase< RRenderResource >;

    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RRenderResource() = 0;
        //@}
    };
}

#endif  // LUNAR_RENDERING_R_RENDER_RESOURCE_H
