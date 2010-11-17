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
///
/// @param[in] CLASS         Class type.
/// @param[in] SUPPORT_TYPE  Reference counting support type.
#define L_DECLARE_REF_COUNT( CLASS, SUPPORT_TYPE ) \
    public: \
        typedef SUPPORT_TYPE RefCountSupportType; \
        Lunar::RefCountProxy< CLASS >* GetRefCountProxy() const; \
    private: \
        mutable Lunar::RefCountProxyContainer< CLASS > _m_refCountProxyContainer;

/// Utility macro for implementing common functions and variables for an object with reference counting support.
///
/// @param[in] CLASS  Class type.
#define L_IMPLEMENT_REF_COUNT( CLASS ) \
    Lunar::RefCountProxy< CLASS >* CLASS::GetRefCountProxy() const \
    { \
        return _m_refCountProxyContainer.Get( const_cast< CLASS* >( this ), DestroyCallback ); \
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

    template< typename BaseT > class RefCountProxy;
    template< typename T > class StrongPtr;
    template< typename T > class WeakPtr;

    /// Reference counting support for Object types.
    class LUNAR_ENGINE_API ObjectRefCountSupport
    {
    public:
        /// Base type of reference counted object.
        typedef Object BaseType;

        /// Number of proxy objects to allocate per block for the proxy pool.
        static const size_t POOL_BLOCK_SIZE = 1024;

        /// @name Allocation Interface
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

    /// Reference counting object proxy.
    template< typename BaseT >
    class RefCountProxy
    {
    public:
        /// Object destruction callback type.
        typedef void ( *DESTROY_CALLBACK )( BaseT* pObject );

        /// @name Initialization
        //@{
        void Initialize( BaseT* pObject, DESTROY_CALLBACK pDestroyCallback );
        //@}

        /// @name Object Access
        //@{
        BaseT* GetObject() const;
        //@}

        /// @name Reference Counting
        //@{
        void AddStrongRef();
        bool RemoveStrongRef();
        uint16_t GetStrongRefCount() const;

        void AddWeakRef();
        bool RemoveWeakRef();
        uint16_t GetWeakRefCount() const;
        //@}

    private:
        /// Reference-counted object.
        BaseT* volatile m_pObject;
        /// Callback to destroy the reference-counted object.
        DESTROY_CALLBACK m_pDestroyCallback;

        /// Reference counts (strong references in lower 16-bits, weak references in upper 16-bits).
        volatile int32_t m_refCounts;

        /// @name Private Utility Functions
        //@{
        void DestroyObject();
        //@}
    };

    /// Reference counting object proxy container.  This is provided to ease the management of a reference count proxy
    /// for an object (i.e. don't need to initialize in the constructor).
    template< typename BaseT >
    class RefCountProxyContainer
    {
    public:
        /// Reference count support type.
        typedef typename BaseT::RefCountSupportType SupportType;

        /// @name Construction/Destruction
        //@{
        RefCountProxyContainer();
        //@}

        /// @name Access
        //@{
        RefCountProxy< BaseT >* Get(
            BaseT* pObject, typename RefCountProxy< BaseT >::DESTROY_CALLBACK pDestroyCallback );
        //@}

    private:
        /// Reference counting proxy instance.
        RefCountProxy< BaseT >* volatile m_pProxy;
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
        /// Proxy object (cast to a void pointer to avoid the need for knowledge about the template type in order to
        /// simply hold an instance of a StrongPtr).
        void* m_pVoidProxy;

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
        /// Proxy object (cast to a void pointer to avoid the need for knowledge about the template type in order to
        /// simply hold an instance of a WeakPtr).
        void* m_pVoidProxy;

        /// @name Conversion Utility Functions, Private
        //@{
        template< typename BaseT > const WeakPtr< BaseT >& ImplicitUpCast(
            const boost::true_type& rIsProperBase ) const;
        //@}
    };
}

#include "ReferenceCounting.inl"

#endif  // LUNAR_ENGINE_REFERENCE_COUNTING_H
