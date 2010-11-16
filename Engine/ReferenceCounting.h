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

#include "Foundation/Container/ObjectPool.h"
#include "Foundation/Container/ConcurrentHashSet.h"

/// Utility macro for declaring common functions and variables for an object with reference counting support.
#define L_DECLARE_REF_COUNT() \
    private: \
        mutable Lunar::RefCountProxyContainer _m_refCountProxyContainer; \
        static void _DestroyCallback( Lunar::Object* pObject ); \
    public: \
        Lunar::RefCountProxy* GetRefCountProxy() const;

/// Utility macro for implementing common functions and variables for an object with reference counting support.
///
/// @param[in] CLASS  Class type.
#define L_IMPLEMENT_REF_COUNT( CLASS ) \
    void CLASS::_DestroyCallback( Lunar::Object* pObject ) \
    { \
        delete static_cast< CLASS* >( pObject ); \
    } \
    \
    Lunar::RefCountProxy* CLASS::GetRefCountProxy() const \
    { \
        return _m_refCountProxyContainer.Get( const_cast< CLASS* >( this ), _DestroyCallback ); \
    }

/// Forward declare a strong pointer type.
///
/// @param[in] CLASS  Class for which to declare the strong pointer type.
///
/// @see L_DECLARE_WPTR()
#define L_DECLARE_PTR( CLASS ) class CLASS; typedef Lunar::StrongPtr< CLASS > CLASS##Ptr;

/// Forward declare a weak pointer type.
///
/// @param[in] CLASS  Class for which to declare the weak pointer type.
///
/// @see L_DECLARE_PTR()
#define L_DECLARE_WPTR( CLASS ) class CLASS; typedef Lunar::WeakPtr< CLASS > CLASS##WPtr;

namespace Lunar
{
    class Object;

    template< typename T > class StrongPtr;
    template< typename T > class WeakPtr;

    /// Reference counting object proxy.
    class LUNAR_ENGINE_API RefCountProxy
    {
    public:
        /// Number of proxy objects to allocate per block for the proxy pool.
        static const size_t POOL_BLOCK_SIZE = 1024;

        /// Object destruction callback type.
        typedef void ( *DESTROY_CALLBACK )( Object* pObject );

        /// @name Initialization
        //@{
        inline void Initialize( Object* pObject, DESTROY_CALLBACK pDestroyCallback );
        //@}

        /// @name Object Access
        //@{
        inline Object* GetObject() const;
        //@}

        /// @name Reference Counting
        //@{
        inline void AddStrongRef();
        inline bool RemoveStrongRef();
        inline uint16_t GetStrongRefCount() const;

        inline void AddWeakRef();
        inline bool RemoveWeakRef();
        inline uint16_t GetWeakRefCount() const;
        //@}

        /// @name Allocation Interface
        //@{
        static RefCountProxy* Allocate();
        static void Release( RefCountProxy* pProxy );

        static void Shutdown();
        //@}

#if HELIUM_ENABLE_MEMORY_TRACKING
        /// @name Active Proxy Iteration
        //@{
        static size_t GetActiveProxyCount();
        static bool GetFirstActiveProxy( ConcurrentHashSet< RefCountProxy* >::ConstAccessor& rAccessor );
        //@}
#endif

    private:
        /// Static proxy management data.
        struct StaticData
        {
            /// Proxy object pool.
            ObjectPool< RefCountProxy > proxyPool;
#if HELIUM_ENABLE_MEMORY_TRACKING
            /// Active reference count proxies.
            ConcurrentHashSet< RefCountProxy* > activeProxySet;
#endif

            /// @name Construction/Destruction
            //@{
            StaticData();
            //@}
        };

        /// Reference-counted object.
        Object* volatile m_pObject;
        /// Callback to destroy the reference-counted object.
        DESTROY_CALLBACK m_pDestroyCallback;

        /// Reference counts (strong references in lower 16-bits, weak references in upper 16-bits).
        volatile int32_t m_refCounts;

        /// Static proxy management data.
        static StaticData* sm_pStaticData;

        /// @name Private Utility Functions
        //@{
        void DestroyObject();
        //@}
    };

    /// Reference counting object proxy container.  This is provided to ease the management of a reference count proxy
    /// for an object (i.e. don't need to initialize in the constructor).
    class LUNAR_ENGINE_API RefCountProxyContainer
    {
    public:
        /// @name Construction/Destruction
        //@{
        inline RefCountProxyContainer();
        //@}

        /// @name Access
        //@{
        inline RefCountProxy* Get( Object* pObject, RefCountProxy::DESTROY_CALLBACK pDestroyCallback );
        //@}

    private:
        /// Reference counting proxy instance.
        RefCountProxy* volatile m_pProxy;
    };

    /// Strong pointer for reference-counted objects.
    template< typename T >
    class StrongPtr
    {
        friend class WeakPtr< T >;

    public:
        /// @name Construction/Destruction
        //@{
        StrongPtr();
        explicit StrongPtr( T* pObject );
        explicit StrongPtr( const WeakPtr< T >& rPointer );
        StrongPtr( const StrongPtr& rPointer );
        ~StrongPtr();
        //@}

        /// @name Object Referencing
        //@{
        T* Get() const;
        void Set( T* pObject );
        void Release();
        //@}

        /// @name Object Linking Support
        //@{
        void SetLinkIndex( uint32_t index );
        uint32_t GetLinkIndex() const;
        void ClearLinkIndex();
        //@}

        /// @name Overloaded Operators
        //@{
        operator T*() const;
        template< typename BaseT > operator const StrongPtr< BaseT >&() const;

        T& operator*() const;
        T* operator->() const;

        StrongPtr& operator=( T* pObject );
        StrongPtr& operator=( const WeakPtr< T >& rOther );
        StrongPtr& operator=( const StrongPtr& rOther );

        bool operator==( const WeakPtr< T >& rPointer ) const;
        bool operator==( const StrongPtr& rPointer ) const;
        bool operator!=( const WeakPtr< T >& rPointer ) const;
        bool operator!=( const StrongPtr& rPointer ) const;
        //@}

        /// @name Friend Functions
        //@{
        friend bool operator==( const T* pObject, const StrongPtr& rPointer );
        friend bool operator!=( const T* pObject, const StrongPtr& rPointer );
        //@}

    private:
        /// Proxy object.
        RefCountProxy* m_pProxy;

        /// @name Conversion Utility Functions, Private
        //@{
        template< typename BaseT > const StrongPtr< BaseT >& ImplicitUpCast(
            const boost::true_type& rIsProperBase ) const;
        //@}
    };

    /// Weak pointer for reference-counted objects.
    template< typename T >
    class WeakPtr
    {
        friend class StrongPtr< T >;

    public:
        /// @name Construction/Destruction
        //@{
        WeakPtr();
        explicit WeakPtr( T* pObject );
        explicit WeakPtr( const StrongPtr< T >& rPointer );
        WeakPtr( const WeakPtr& rPointer );
        ~WeakPtr();
        //@}

        /// @name Object Referencing
        //@{
        T* Get() const;
        void Set( T* pObject );
        void Release();
        //@}

        /// @name Reference Count Proxy Matching
        //@{
        bool HasObjectProxy( const T* pObject ) const;
        //@}

        /// @name Overloaded Operators
        //@{
        operator T*() const;
        template< typename BaseT > operator const WeakPtr< BaseT >&() const;

        T& operator*() const;
        T* operator->() const;

        WeakPtr& operator=( T* pObject );
        WeakPtr& operator=( const StrongPtr< T >& rOther );
        WeakPtr& operator=( const WeakPtr& rOther );

        bool operator==( const StrongPtr< T >& rPointer ) const;
        bool operator==( const WeakPtr& rPointer ) const;
        bool operator!=( const StrongPtr< T >& rPointer ) const;
        bool operator!=( const WeakPtr& rPointer ) const;
        //@}

        /// @name Friend Functions
        //@{
        friend bool operator==( const T* pObject, const WeakPtr& rPointer );
        friend bool operator!=( const T* pObject, const WeakPtr& rPointer );
        //@}

    private:
        /// Proxy object.
        RefCountProxy* m_pProxy;

        /// @name Conversion Utility Functions, Private
        //@{
        template< typename BaseT > const WeakPtr< BaseT >& ImplicitUpCast(
            const boost::true_type& rIsProperBase ) const;
        //@}
    };
}

#include "ReferenceCounting.inl"

#endif  // LUNAR_ENGINE_REFERENCE_COUNTING_H
