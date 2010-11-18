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
#include "Platform/Atomic.h"

/// Forward declare a render resource smart pointer type.
///
/// @param[in] CLASS  Class for which to declare the smart pointer type.
#define L_DECLARE_RPTR( CLASS ) class CLASS; typedef Lunar::RSmartPtr< CLASS > CLASS##Ptr;

namespace Lunar
{
    /// Base class for all reference-counted render resources.
    class LUNAR_RENDERING_API RRenderResource : NonCopyable
    {
        friend class RReferenceCountProxy;

    protected:
        /// @name Construction/Destruction
        //@{
        inline RRenderResource();
        virtual ~RRenderResource() = 0;
        //@}

    private:
        /// Current reference count.
        volatile int32_t m_referenceCount;
    };

    /// Proxy interface for updating render resource reference counts.
    ///
    /// This class should only ever be used by RSmartPtr.
    class LUNAR_RENDERING_API RReferenceCountProxy
    {
    public:
        /// @name Static Utility Functions
        //@{
        inline static uint32_t AddRef( RRenderResource* pResource );
        inline static uint32_t Release( RRenderResource* pResource );
        //@}
    };

    /// Render resource smart pointer wrapper.
    ///
    /// This automatically increments the render resource reference count on assignment and decrements the reference
    /// count when the resource reference is cleared.
    ///
    /// Note that all references are strong references.  Weak reference counting is not supported by render resources.
    template< typename T >
    class RSmartPtr
    {
    public:
        /// @name Construction/Destruction
        //@{
        RSmartPtr( T* pResource = NULL );
        RSmartPtr( const RSmartPtr& rSource );
        ~RSmartPtr();
        //@}

        /// @name Data Access
        //@{
        T* Get() const;
        void Set( T* pResource );
        void Release();
        //@{

        /// @name Overloaded Operators
        //@{
        T& operator*() const;
        T* operator->() const;

        operator T* const&() const;

        RSmartPtr< T >& operator=( T* pResource );
        RSmartPtr< T >& operator=( const RSmartPtr& rSource );
        //@}

    private:
        /// Render resource instance.
        T* m_pResource;
    };
}

#include "Rendering/RRenderResource.inl"

#endif  // LUNAR_RENDERING_R_RENDER_RESOURCE_H
