//----------------------------------------------------------------------------------------------------------------------
// ReferenceCounting.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Initialize this reference count proxy object.
    ///
    /// @param[in] pObject           Object for which to manage reference counting.
    /// @param[in] pDestroyCallback  Callback to execute when the object needs to be destroyed.
    void RefCountProxy::Initialize( Object* pObject, DESTROY_CALLBACK pDestroyCallback )
    {
        HELIUM_ASSERT( pObject );
        HELIUM_ASSERT( pDestroyCallback );

        m_pObject = pObject;
        m_pDestroyCallback = pDestroyCallback;

        m_refCounts = 0;
    }

    /// Get the pointer to the object managed by this proxy.
    ///
    /// @return  Pointer to the managed object.
    Object* RefCountProxy::GetObject() const
    {
        return m_pObject;
    }

    /// Increment the strong reference count.
    ///
    /// @see RemoveStrongRef(), GetStrongRefCount(), AddWeakRef(), RemoveWeakRef(), GetWeakRefCount()
    void RefCountProxy::AddStrongRef()
    {
        AtomicIncrementAcquire( m_refCounts );
    }

    /// Decrement the strong reference count.
    ///
    /// @return  True if there are no more strong or weak references, false otherwise.
    ///
    /// @see AddStrongRef(), GetStrongRefCount(), AddWeakRef(), RemoveWeakRef(), GetWeakRefCount()
    bool RefCountProxy::RemoveStrongRef()
    {
        int32_t newRefCounts = AtomicDecrementRelease( m_refCounts );
        HELIUM_ASSERT( ( static_cast< uint32_t >( newRefCounts ) & 0xffff ) != 0xffff );
        if( ( static_cast< uint32_t >( newRefCounts ) & 0xffff ) == 0 )
        {
            DestroyObject();
        }

        return ( newRefCounts == 0 );
    }

    /// Get the current number of strong references for this proxy.
    ///
    /// @return  Strong reference count.
    ///
    /// @see AddStrongRef(), RemoveStrongRef(), AddWeakRef(), RemoveWeakRef(), GetWeakRefCount()
    uint16_t RefCountProxy::GetStrongRefCount() const
    {
        return static_cast< uint16_t >( static_cast< uint32_t >( m_refCounts ) & 0xffff );
    }

    /// Increment the weak reference count.
    ///
    /// @see RemoveWeakRef(), GetWeakRefCount(), AddStrongRef(), RemoveStrongRef(), GetStrongRefCount()
    void RefCountProxy::AddWeakRef()
    {
        AtomicAddAcquire( m_refCounts, 0x10000 );
    }

    /// Decrement the weak reference count.
    ///
    /// @return  True if there are no more strong or weak references, false otherwise.
    ///
    /// @see AddWeakRef(), GetWeakRefCount(), AddStrongRef(), RemoveStrongRef(), GetStrongRefCount()
    bool RefCountProxy::RemoveWeakRef()
    {
        // Remember: AtomicSubtractRelease() returns the original value, not the new value.
        int32_t oldRefCounts = AtomicSubtractRelease( m_refCounts, 0x10000 );
        HELIUM_ASSERT( ( static_cast< uint32_t >( oldRefCounts ) >> 16 ) != 0 );

        return ( oldRefCounts == 0x10000 );
    }

    /// Get the current number of weak references for this proxy.
    ///
    /// @return  Weak reference count.
    ///
    /// @see AddWeakRef(), RemoveWeakRef(), AddStrongRef(), RemoveStrongRef(), GetStrongRefCount()
    uint16_t RefCountProxy::GetWeakRefCount() const
    {
        return static_cast< uint16_t >( static_cast< uint32_t >( m_refCounts ) >> 16 );
    }

    /// Constructor.
    RefCountProxyContainer::RefCountProxyContainer()
        : m_pProxy( NULL )
    {
    }

    /// Get the reference count proxy, initializing it if necessary.
    ///
    /// @param[in] pObject           Object to which the proxy should be initialized if needed.
    /// @param[in] pDestroyCallback  Callback to use to destroy the object if needed.
    ///
    /// @return  Pointer to the reference count proxy instance.
    RefCountProxy* RefCountProxyContainer::Get( Object* pObject, RefCountProxy::DESTROY_CALLBACK pDestroyCallback )
    {
        RefCountProxy* pProxy = m_pProxy;
        if( !pProxy )
        {
            pProxy = RefCountProxy::Allocate();
            HELIUM_ASSERT( pProxy );
            pProxy->Initialize( pObject, pDestroyCallback );

            // Atomically set the existing proxy, making sure the proxy is still null.  If another proxy was swapped in
            // first, release the proxy we just tried to allocate.
            RefCountProxy* pExistingProxy = AtomicCompareExchangeRelease< RefCountProxy >( m_pProxy, pProxy, NULL );
            if( pExistingProxy )
            {
                RefCountProxy::Release( pProxy );
                pProxy = pExistingProxy;
            }
        }

        return pProxy;
    }

    /// Constructor.
    template< typename T >
    StrongPtr< T >::StrongPtr()
        : m_pProxy( NULL )
    {
    }

    /// Constructor.
    ///
    /// @param[in] pObject  Object to initially assign.
    template< typename T >
    StrongPtr< T >::StrongPtr( T* pObject )
        : m_pProxy( NULL )
    {
        if( pObject )
        {
            m_pProxy = pObject->GetRefCountProxy();
            HELIUM_ASSERT( m_pProxy );
            m_pProxy->AddStrongRef();
        }
    }

    /// Constructor.
    ///
    /// @param[in] rPointer  Weak pointer from which to copy.
    template< typename T >
    StrongPtr< T >::StrongPtr( const WeakPtr< T >& rPointer )
        : m_pProxy( rPointer.m_pProxy )
    {
        // Note that a weak pointer can have a reference count proxy set to null, so we need to check for and handle
        // that case as well.
        if( m_pProxy )
        {
            if( m_pProxy->GetObject() )
            {
                m_pProxy->AddStrongRef();
            }
            else
            {
                m_pProxy = NULL;
            }
        }
    }

    /// Copy constructor.
    ///
    /// @param[in] rPointer  Strong pointer from which to copy.
    template< typename T >
    StrongPtr< T >::StrongPtr( const StrongPtr& rPointer )
        : m_pProxy( rPointer.m_pProxy )
    {
        if( m_pProxy )
        {
            m_pProxy->AddStrongRef();
        }
    }

    /// Destructor.
    template< typename T >
    StrongPtr< T >::~StrongPtr()
    {
        Release();
    }

    /// Get the object referenced by this smart pointer.
    ///
    /// @return  Pointer to the referenced object.
    ///
    /// @see Set(), Release()
    template< typename T >
    T* StrongPtr< T >::Get() const
    {
        return( m_pProxy ? static_cast< T* >( m_pProxy->GetObject() ) : NULL );
    }

    /// Set the object referenced by this smart pointer.
    ///
    /// @param[in] pObject  Object to reference.
    ///
    /// @see Get(), Release()
    template< typename T >
    void StrongPtr< T >::Set( T* pObject )
    {
        RefCountProxy* pProxy = m_pProxy;
        if( pProxy )
        {
            if( pProxy->GetObject() == pObject )
            {
                return;
            }

            m_pProxy = NULL;

            if( pProxy->RemoveStrongRef() )
            {
                RefCountProxy::Release( pProxy );
            }
        }

        if( pObject )
        {
            m_pProxy = pObject->GetRefCountProxy();
            HELIUM_ASSERT( m_pProxy );
            m_pProxy->AddStrongRef();
        }
    }

    /// Release any object referenced by this smart pointer.
    ///
    /// @see Get(), Set()
    template< typename T >
    void StrongPtr< T >::Release()
    {
        RefCountProxy* pProxy = m_pProxy;
        m_pProxy = NULL;

        if( pProxy && pProxy->RemoveStrongRef() )
        {
            RefCountProxy::Release( pProxy );
        }
    }

    /// Directly write an object link table index to this smart pointer.
    ///
    /// This is intended specifically for Serializer objects that perform object linking.
    ///
    /// Be careful when using the object linking interface for a smart pointer, as it is used to read and write integer
    /// values directly to internal pointer variables.  Release() should always be called prior to the first time
    /// SetLinkIndex() is called, and ClearLinkIndex() should always be called prior to re-using the smart pointer for
    /// actual object referencing.
    ///
    /// @param[in] index  Link table index to set.
    ///
    /// @see GetLinkIndex(), ClearLinkIndex()
    template< typename T >
    void StrongPtr< T >::SetLinkIndex( uint32_t index )
    {
        reinterpret_cast< uintptr_t& >( m_pProxy ) = index;
    }

    /// Get the object link table index stored in this smart pointer.
    ///
    /// This is intended specifically for Serializer objects that perform object linking.
    ///
    /// Be careful when using the object linking interface for a smart pointer, as it is used to read and write integer
    /// values directly to internal pointer variables.  Release() should always be called prior to the first time
    /// SetLinkIndex() is called, and ClearLinkIndex() should always be called prior to re-using the smart pointer for
    /// actual object referencing.
    ///
    /// @return  Link table index.
    ///
    /// @see SetLinkIndex(), ClearLinkIndex()
    template< typename T >
    uint32_t StrongPtr< T >::GetLinkIndex() const
    {
        return static_cast< uint32_t >( reinterpret_cast< const uintptr_t& >( m_pProxy ) );
    }

    /// Clear out the link table index stored in this smart pointer.
    ///
    /// This is intended specifically for Serializer objects that perform object linking.
    ///
    /// Be careful when using the object linking interface for a smart pointer, as it is used to read and write integer
    /// values directly to internal pointer variables.  Release() should always be called prior to the first time
    /// SetLinkIndex() is called, and ClearLinkIndex() should always be called prior to re-using the smart pointer for
    /// actual object referencing.
    ///
    /// @return  Link table index.
    ///
    /// @see SetLinkIndex(), GetLinkIndex()
    template< typename T >
    void StrongPtr< T >::ClearLinkIndex()
    {
        m_pProxy = NULL;
    }

    /// Get the object referenced by this smart pointer.
    ///
    /// @return  Pointer to the referenced object.
    ///
    /// @see Set(), Release()
    template< typename T >
    StrongPtr< T >::operator T*() const
    {
        return Get();
    }

    /// Implicit cast to a strong pointer of a base type.
    ///
    /// @return  Constant reference to the cast strong pointer.
    template< typename T >
    template< typename BaseT >
    StrongPtr< T >::operator const StrongPtr< BaseT >&() const
    {
        return ImplicitUpCast< BaseT >( boost::is_base_of< BaseT, T >() );
    }

    /// Dereference this pointer.
    ///
    /// @return  Reference to the actual object.
    template< typename T >
    T& StrongPtr< T >::operator*() const
    {
        T* pObject = Get();
        HELIUM_ASSERT( pObject );

        return *pObject;
    }

    /// Dereference this pointer.
    ///
    /// @return  Pointer to the actual object.
    template< typename T >
    T* StrongPtr< T >::operator->() const
    {
        T* pObject = Get();
        HELIUM_ASSERT( pObject );

        return pObject;
    }

    /// Assignment operator.
    ///
    /// @param[in] pObject  Object to assign.
    ///
    /// @return  Reference to this object.
    template< typename T >
    StrongPtr< T >& StrongPtr< T >::operator=( T* pObject )
    {
        Set( pObject );

        return *this;
    }

    /// Assignment operator.
    ///
    /// @param[in] rPointer  Smart pointer from which to copy.
    ///
    /// @return  Reference to this object.
    template< typename T >
    StrongPtr< T >& StrongPtr< T >::operator=( const WeakPtr< T >& rPointer )
    {
        // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check
        // for and handle that case as well.
        RefCountProxy* pOtherProxy = rPointer.m_pProxy;
        if( pOtherProxy && !pOtherProxy->GetObject() )
        {
            pOtherProxy = NULL;
        }

        if( m_pProxy != pOtherProxy )
        {
            Release();

            m_pProxy = pOtherProxy;
            if( pOtherProxy )
            {
                pOtherProxy->AddStrongRef();
            }
        }

        return *this;
    }

    /// Assignment operator.
    ///
    /// @param[in] rPointer  Smart pointer from which to copy.
    ///
    /// @return  Reference to this object.
    template< typename T >
    StrongPtr< T >& StrongPtr< T >::operator=( const StrongPtr& rPointer )
    {
        RefCountProxy* pOtherProxy = rPointer.m_pProxy;
        if( m_pProxy != pOtherProxy )
        {
            Release();

            m_pProxy = pOtherProxy;
            if( pOtherProxy )
            {
                pOtherProxy->AddStrongRef();
            }
        }

        return *this;
    }

    /// Equality comparison operator.
    ///
    /// @param[in] rPointer  Smart pointer with which to compare.
    ///
    /// @return  True if this smart pointer and the given smart pointer reference the same object, false if not.
    template< typename T >
    bool StrongPtr< T >::operator==( const WeakPtr< T >& rPointer ) const
    {
        // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check
        // for and handle that case as well.
        RefCountProxy* pOtherProxy = rPointer.m_pProxy;
        if( pOtherProxy && !pOtherProxy->GetObject() )
        {
            pOtherProxy = NULL;
        }

        return( m_pProxy == pOtherProxy );
    }

    /// Equality comparison operator.
    ///
    /// @param[in] rPointer  Smart pointer with which to compare.
    ///
    /// @return  True if this smart pointer and the given smart pointer reference the same object, false if not.
    template< typename T >
    bool StrongPtr< T >::operator==( const StrongPtr& rPointer ) const
    {
        return( m_pProxy == rPointer.m_pProxy );
    }

    /// Inequality comparison operator.
    ///
    /// @param[in] rPointer  Smart pointer with which to compare.
    ///
    /// @return  True if this smart pointer and the given smart pointer do not reference the same object, false if they
    ///          do.
    template< typename T >
    bool StrongPtr< T >::operator!=( const WeakPtr< T >& rPointer ) const
    {
        // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check
        // for and handle that case as well.
        RefCountProxy* pOtherProxy = rPointer.m_pProxy;
        if( pOtherProxy && !pOtherProxy->GetObject() )
        {
            pOtherProxy = NULL;
        }

        return( m_pProxy != pOtherProxy );
    }

    /// Inequality comparison operator.
    ///
    /// @param[in] rPointer  Smart pointer with which to compare.
    ///
    /// @return  True if this smart pointer and the given smart pointer do not reference the same object, false if they
    ///          do.
    template< typename T >
    bool StrongPtr< T >::operator!=( const StrongPtr& rPointer ) const
    {
        return( m_pProxy != rPointer.m_pProxy );
    }

    /// Helper function for performing a compile-time verified up-cast of a StrongPtr.
    ///
    /// @param[in] rIsProperBase  Instance of boost::is_base_of< BaseT, T > if it inherits from boost::true_type.
    ///
    /// @return  Constant reference to the cast strong pointer.
    template< typename T >
    template< typename BaseT >
    const StrongPtr< BaseT >& StrongPtr< T >::ImplicitUpCast( const boost::true_type& /*rIsProperBase*/ ) const
    {
        return *reinterpret_cast< const StrongPtr< BaseT >* >( this );
    }

    /// Equality comparison operator.
    ///
    /// @param[in] pObject   Object with which to compare.
    /// @param[in] rPointer  Smart pointer with which to compare.
    ///
    /// @return  True if the smart pointer references the given object, false if not.
    template< typename T >
    bool operator==( const T* pObject, const StrongPtr< T >& rPointer )
    {
        return( pObject == rPointer.Get() );
    }

    /// Inequality comparison operator.
    ///
    /// @param[in] pObject   Object with which to compare.
    /// @param[in] rPointer  Smart pointer with which to compare.
    ///
    /// @return  True if the smart pointer does not reference the given object, false if it does.
    template< typename T >
    bool operator!=( const T* pObject, const StrongPtr< T >& rPointer )
    {
        return( pObject != rPointer.Get() );
    }

    /// Constructor.
    template< typename T >
    WeakPtr< T >::WeakPtr()
        : m_pProxy( NULL )
    {
    }

    /// Constructor.
    ///
    /// @param[in] pObject  Object to initially assign.
    template< typename T >
    WeakPtr< T >::WeakPtr( T* pObject )
        : m_pProxy( NULL )
    {
        if( pObject )
        {
            m_pProxy = pObject->GetRefCountProxy();
            HELIUM_ASSERT( m_pProxy );
            m_pProxy->AddWeakRef();
        }
    }

    /// Constructor.
    ///
    /// @param[in] rPointer  Strong pointer from which to copy.
    template< typename T >
    WeakPtr< T >::WeakPtr( const StrongPtr< T >& rPointer )
        : m_pProxy( rPointer.m_pProxy )
    {
        if( m_pProxy )
        {
            m_pProxy->AddWeakRef();
        }
    }

    /// Copy constructor.
    ///
    /// @param[in] rPointer  Weak pointer from which to copy.
    template< typename T >
    WeakPtr< T >::WeakPtr( const WeakPtr& rPointer )
        : m_pProxy( rPointer.m_pProxy )
    {
        // Note that a weak pointer can have a reference count proxy set to null, so we need to check for and handle
        // that case as well.
        if( m_pProxy )
        {
            if( m_pProxy->GetObject() )
            {
                m_pProxy->AddWeakRef();
            }
            else
            {
                m_pProxy = NULL;
            }
        }
    }

    /// Destructor.
    template< typename T >
    WeakPtr< T >::~WeakPtr()
    {
        Release();
    }

    /// Get the object referenced by this smart pointer.
    ///
    /// @return  Pointer to the referenced object.
    ///
    /// @see Set(), Release()
    template< typename T >
    T* WeakPtr< T >::Get() const
    {
        return( m_pProxy ? static_cast< T* >( m_pProxy->GetObject() ) : NULL );
    }

    /// Set the object referenced by this smart pointer.
    ///
    /// @param[in] pObject  Object to reference.
    ///
    /// @see Get(), Release()
    template< typename T >
    void WeakPtr< T >::Set( T* pObject )
    {
        RefCountProxy* pProxy = m_pProxy;
        if( pProxy )
        {
            // Note that a weak pointer can have a reference count proxy set to null, so we need to check for and handle
            // that case as well.
            if( pObject && pProxy->GetObject() == pObject )
            {
                return;
            }

            m_pProxy = NULL;

            if( pProxy->RemoveWeakRef() )
            {
                RefCountProxy::Release( pProxy );
            }
        }

        if( pObject )
        {
            m_pProxy = pObject->GetRefCountProxy();
            HELIUM_ASSERT( m_pProxy );
            m_pProxy->AddWeakRef();
        }
    }

    /// Release any object referenced by this smart pointer.
    ///
    /// @see Get(), Set()
    template< typename T >
    void WeakPtr< T >::Release()
    {
        RefCountProxy* pProxy = m_pProxy;
        m_pProxy = NULL;

        if( pProxy && pProxy->RemoveWeakRef() )
        {
            RefCountProxy::Release( pProxy );
        }
    }

    /// Get whether this weak pointer is holding onto the reference counting proxy object for the given object.
    ///
    /// @return  True if this object is holding onto the given object's reference counting proxy, false if not.
    template< typename T >
    bool WeakPtr< T >::HasObjectProxy( const T* pObject ) const
    {
        HELIUM_ASSERT( pObject );

        return ( m_pProxy == pObject->GetRefCountProxy() );
    }

    /// Get the object referenced by this smart pointer.
    ///
    /// @return  Pointer to the referenced object.
    ///
    /// @see Set(), Release()
    template< typename T >
    WeakPtr< T >::operator T*() const
    {
        return Get();
    }

    /// Implicit cast to a weak pointer of a base type.
    ///
    /// @return  Constant reference to the cast weak pointer.
    template< typename T >
    template< typename BaseT >
    WeakPtr< T >::operator const WeakPtr< BaseT >&() const
    {
        return ImplicitUpCast< BaseT >( boost::is_base_of< BaseT, T >() );
    }

    /// Dereference this pointer.
    ///
    /// @return  Reference to the actual object.
    template< typename T >
    T& WeakPtr< T >::operator*() const
    {
        T* pObject = Get();
        HELIUM_ASSERT( pObject );

        return *pObject;
    }

    /// Dereference this pointer.
    ///
    /// @return  Pointer to the actual object.
    template< typename T >
    T* WeakPtr< T >::operator->() const
    {
        T* pObject = Get();
        HELIUM_ASSERT( pObject );

        return pObject;
    }

    /// Assignment operator.
    ///
    /// @param[in] pObject  Object to assign.
    ///
    /// @return  Reference to this object.
    template< typename T >
    WeakPtr< T >& WeakPtr< T >::operator=( T* pObject )
    {
        Set( pObject );

        return *this;
    }

    /// Assignment operator.
    ///
    /// @param[in] rPointer  Smart pointer from which to copy.
    ///
    /// @return  Reference to this object.
    template< typename T >
    WeakPtr< T >& WeakPtr< T >::operator=( const StrongPtr< T >& rPointer )
    {
        RefCountProxy* pOtherProxy = rPointer.m_pProxy;
        if( m_pProxy != pOtherProxy )
        {
            Release();

            m_pProxy = pOtherProxy;
            if( pOtherProxy )
            {
                pOtherProxy->AddWeakRef();
            }
        }

        return *this;
    }

    /// Assignment operator.
    ///
    /// @param[in] rPointer  Smart pointer from which to copy.
    ///
    /// @return  Reference to this object.
    template< typename T >
    WeakPtr< T >& WeakPtr< T >::operator=( const WeakPtr& rPointer )
    {
        // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check
        // for and handle that case as well.
        RefCountProxy* pOtherProxy = rPointer.m_pProxy;
        if( pOtherProxy && !pOtherProxy->GetObject() )
        {
            pOtherProxy = NULL;
        }

        if( m_pProxy != pOtherProxy )
        {
            Release();

            m_pProxy = pOtherProxy;
            if( pOtherProxy )
            {
                pOtherProxy->AddWeakRef();
            }
        }

        return *this;
    }

    /// Equality comparison operator.
    ///
    /// @param[in] rPointer  Smart pointer with which to compare.
    ///
    /// @return  True if this smart pointer and the given smart pointer reference the same object, false if not.
    template< typename T >
    bool WeakPtr< T >::operator==( const StrongPtr< T >& rPointer ) const
    {
        // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check
        // for and handle that case as well.
        RefCountProxy* pThisProxy = m_pProxy;
        if( pThisProxy && !pThisProxy->GetObject() )
        {
            pThisProxy = NULL;
        }

        return( pThisProxy == rPointer.m_pProxy );
    }

    /// Equality comparison operator.
    ///
    /// @param[in] rPointer  Smart pointer with which to compare.
    ///
    /// @return  True if this smart pointer and the given smart pointer reference the same object, false if not.
    template< typename T >
    bool WeakPtr< T >::operator==( const WeakPtr& rPointer ) const
    {
        // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check
        // for and handle that case as well.
        return( Get() == rPointer.Get() );
    }

    /// Inequality comparison operator.
    ///
    /// @param[in] rPointer  Smart pointer with which to compare.
    ///
    /// @return  True if this smart pointer and the given smart pointer do not reference the same object, false if they
    ///          do.
    template< typename T >
    bool WeakPtr< T >::operator!=( const StrongPtr< T >& rPointer ) const
    {
        // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check
        // for and handle that case as well.
        RefCountProxy* pThisProxy = m_pProxy;
        if( pThisProxy && !pThisProxy->GetObject() )
        {
            pThisProxy = NULL;
        }

        return( pThisProxy != rPointer.m_pProxy );
    }

    /// Inequality comparison operator.
    ///
    /// @param[in] rPointer  Smart pointer with which to compare.
    ///
    /// @return  True if this smart pointer and the given smart pointer do not reference the same object, false if they
    ///          do.
    template< typename T >
    bool WeakPtr< T >::operator!=( const WeakPtr& rPointer ) const
    {
        // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check
        // for and handle that case as well.
        return( Get() != rPointer.Get() );
    }

    /// Equality comparison operator.
    ///
    /// @param[in] pObject   Object with which to compare.
    /// @param[in] rPointer  Smart pointer with which to compare.
    ///
    /// @return  True if the smart pointer references the given object, false if not.
    template< typename T >
    bool operator==( const T* pObject, const WeakPtr< T >& rPointer )
    {
        return( pObject == rPointer.Get() );
    }

    /// Inequality comparison operator.
    ///
    /// @param[in] pObject   Object with which to compare.
    /// @param[in] rPointer  Smart pointer with which to compare.
    ///
    /// @return  True if the smart pointer does not reference the given object, false if it does.
    template< typename T >
    bool operator!=( const T* pObject, const WeakPtr< T >& rPointer )
    {
        return( pObject != rPointer.Get() );
    }

    /// Helper function for performing a compile-time verified up-cast of a WeakPtr.
    ///
    /// @param[in] rIsProperBase  Instance of boost::is_base_of< BaseT, T > if it inherits from boost::true_type.
    ///
    /// @return  Constant reference to the cast weak pointer.
    template< typename T >
    template< typename BaseT >
    const WeakPtr< BaseT >& WeakPtr< T >::ImplicitUpCast( const boost::true_type& /*rIsProperBase*/ ) const
    {
        return *reinterpret_cast< const WeakPtr< BaseT >* >( this );
    }
}
