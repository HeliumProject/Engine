#pragma once

#include "Rendering/Rendering.h"
#include "Foundation/SmartPtr.h"

/// Forward declare a render resource smart pointer type.
///
/// @param[in] CLASS  MetaClass for which to declare the smart pointer type.
#define HELIUM_DECLARE_RPTR( CLASS ) class CLASS; typedef Helium::SmartPtr< CLASS > CLASS##Ptr;

namespace Helium
{
    /// Base class for all reference-counted render resources.
    class HELIUM_RENDERING_API RRenderResource : public AtomicRefCountBase< RRenderResource >, NonCopyable
    {
        friend class AtomicRefCountBase< RRenderResource >;

    protected:
        /// @name Construction/Destruction
        //@{
        virtual ~RRenderResource() = 0;
        //@}
    };
}
