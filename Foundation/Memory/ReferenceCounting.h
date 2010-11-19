#pragma once

#include "Foundation/API.h"

#include "Platform/Atomic.h"
#include "Platform/Utility.h"

/// Utility macro for declaring common functions and variables for an object with strong/weak reference counting
/// support.
///
/// @param[in] CLASS         Class type being declared.
/// @param[in] SUPPORT_TYPE  Reference counting support type.
#define HELIUM_DECLARE_REF_COUNT( CLASS, SUPPORT_TYPE ) \
    public: \
        typedef SUPPORT_TYPE RefCountSupportType; \
        Helium::RefCountProxy< CLASS >* GetRefCountProxy() const \
        { \
            return _m_refCountProxyContainer.Get( const_cast< CLASS* >( this ) ); \
        } \
    private: \
        mutable Helium::RefCountProxyContainer< CLASS > _m_refCountProxyContainer;

/// Forward declare a strong pointer type.
///
/// @param[in] CLASS  Class for which to declare the strong pointer type.
///
/// @see HELIUM_DECLARE_WPTR()
#define HELIUM_DECLARE_PTR( CLASS ) class CLASS; typedef Helium::StrongPtr< CLASS > CLASS##Ptr;

/// Forward declare a weak pointer type.
///
/// @param[in] CLASS  Class for which to declare the weak pointer type.
///
/// @see HELIUM_DECLARE_PTR()
#define HELIUM_DECLARE_WPTR( CLASS ) class CLASS; typedef Helium::WeakPtr< CLASS > CLASS##WPtr;

namespace Helium
{
    template< typename BaseT > class RefCountProxy;
    template< typename T > class StrongPtr;
    template< typename T > class WeakPtr;

    /// Reference counting object proxy.
    template< typename BaseT >
    class RefCountProxy
    {
    public:
        /// @name Initialization
        //@{
        void Initialize( BaseT* pObject );
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
        RefCountProxy< BaseT >* Get( BaseT* pObject );
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
        StrongPtr( T* pObject );
        explicit StrongPtr( const WeakPtr< T >& rPointer );
        StrongPtr( const StrongPtr& rPointer );
        ~StrongPtr();
        //@}

        /// @name Object Referencing
        //@{
        T* Get() const;
        T* Ptr() const;
        void Set( T* pObject );
        void Release();

        bool ReferencesObject() const;
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
        WeakPtr( T* pObject );
        explicit WeakPtr( const StrongPtr< T >& rPointer );
        WeakPtr( const WeakPtr& rPointer );
        ~WeakPtr();
        //@}

        /// @name Object Referencing
        //@{
        T* Get() const;
        T* Ptr() const;
        void Set( T* pObject );
        void Release();

        bool ReferencesObject() const;
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

#include "Foundation/Memory/ReferenceCounting.inl"
