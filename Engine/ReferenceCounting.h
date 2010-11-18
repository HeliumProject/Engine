//----------------------------------------------------------------------------------------------------------------------
// ReferenceCounting.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_REFERENCE_COUNTING_H
#define LUNAR_ENGINE_REFERENCE_COUNTING_H

#include "Engine/Engine.h"

#include "Foundation/Memory/ReferenceCounting.h"
#include "Foundation/Container/ObjectPool.h"
#include "Foundation/Container/ConcurrentHashSet.h"

namespace Lunar
{
    class Object;

    /// Reference counting support for Object types.
    class LUNAR_ENGINE_API ObjectRefCountSupport
    {
    public:
        /// Base type of reference counted object.
        typedef Object BaseType;

        /// @name Object Destruction Support
        //@{
        static void PreDestroy( Object* pObject );
        static void Destroy( Object* pObject );
        //@}

        /// @name Reference Count Proxy Allocation Interface
        //@{
        static RefCountProxy< Object >* Allocate();
        static void Release( RefCountProxy< Object >* pProxy );

        static void Shutdown();
        //@}

#if HELIUM_ENABLE_MEMORY_TRACKING
        /// @name Active Proxy Iteration
        //@{
        static size_t GetActiveProxyCount();
        static bool GetFirstActiveProxy(
            ConcurrentHashSet< RefCountProxy< Object >* >::ConstAccessor& rAccessor );
        //@}
#endif

    private:
        struct StaticData;

        /// Static proxy management data.
        static StaticData* sm_pStaticData;
    };
}

#endif  // LUNAR_ENGINE_REFERENCE_COUNTING_H
