/// Initialize this reference count proxy object.
///
/// @param[in] pObject           Object for which to manage reference counting.
/// @param[in] pDestroyCallback  Callback to execute when the object needs to be destroyed.
template< typename BaseT >
void Helium::RefCountProxy< BaseT >::Initialize( BaseT* pObject )
{
    HELIUM_ASSERT( pObject );

    m_pObject = pObject;
    m_refCounts = 0;
}

/// Get the pointer to the object managed by this proxy.
///
/// @return  Pointer to the managed object.
template< typename BaseT >
BaseT* Helium::RefCountProxy< BaseT >::GetObject() const
{
    return m_pObject;
}

/// Increment the strong reference count.
///
/// @see RemoveStrongRef(), GetStrongRefCount(), AddWeakRef(), RemoveWeakRef(), GetWeakRefCount()
template< typename BaseT >
void Helium::RefCountProxy< BaseT >::AddStrongRef()
{
    AtomicIncrementAcquire( m_refCounts );
}

/// Decrement the strong reference count.
///
/// @return  True if there are no more strong or weak references, false otherwise.
///
/// @see AddStrongRef(), GetStrongRefCount(), AddWeakRef(), RemoveWeakRef(), GetWeakRefCount()
template< typename BaseT >
bool Helium::RefCountProxy< BaseT >::RemoveStrongRef()
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
template< typename BaseT >
uint16_t Helium::RefCountProxy< BaseT >::GetStrongRefCount() const
{
    return static_cast< uint16_t >( static_cast< uint32_t >( m_refCounts ) & 0xffff );
}

/// Increment the weak reference count.
///
/// @see RemoveWeakRef(), GetWeakRefCount(), AddStrongRef(), RemoveStrongRef(), GetStrongRefCount()
template< typename BaseT >
void Helium::RefCountProxy< BaseT >::AddWeakRef()
{
    AtomicAddAcquire( m_refCounts, 0x10000 );
}

/// Decrement the weak reference count.
///
/// @return  True if there are no more strong or weak references, false otherwise.
///
/// @see AddWeakRef(), GetWeakRefCount(), AddStrongRef(), RemoveStrongRef(), GetStrongRefCount()
template< typename BaseT >
bool Helium::RefCountProxy< BaseT >::RemoveWeakRef()
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
template< typename BaseT >
uint16_t Helium::RefCountProxy< BaseT >::GetWeakRefCount() const
{
    return static_cast< uint16_t >( static_cast< uint32_t >( m_refCounts ) >> 16 );
}

/// Helper function for performing proper object destruction upon its strong reference count reaching zero.
template< typename BaseT >
void Helium::RefCountProxy< BaseT >::DestroyObject()
{
    BaseT* pObject = m_pObject;
    HELIUM_ASSERT( pObject );
    typename BaseT::RefCountSupportType::PreDestroy( pObject );

    BaseT* pAtomicObjectOld = AtomicExchangeRelease< BaseT >( m_pObject, NULL );
    HELIUM_ASSERT( pAtomicObjectOld == pObject );
    HELIUM_UNREF( pAtomicObjectOld );

    typename BaseT::RefCountSupportType::Destroy( pObject );
}

/// Constructor.
template< typename BaseT >
Helium::RefCountProxyContainer< BaseT >::RefCountProxyContainer()
    : m_pProxy( NULL )
{
}

/// Get the reference count proxy, initializing it if necessary.
///
/// @param[in] pObject  Object to which the proxy should be initialized if needed.
///
/// @return  Pointer to the reference count proxy instance.
template< typename BaseT >
Helium::RefCountProxy< BaseT >* Helium::RefCountProxyContainer< BaseT >::Get( BaseT* pObject )
{
    RefCountProxy< BaseT >* pProxy = m_pProxy;
    if( !pProxy )
    {
        pProxy = SupportType::Allocate();
        HELIUM_ASSERT( pProxy );
        pProxy->Initialize( pObject );

        // Atomically set the existing proxy, making sure the proxy is still null.  If another proxy was swapped in
        // first, release the proxy we just tried to allocate.
        RefCountProxy< BaseT >* pExistingProxy = AtomicCompareExchangeRelease< RefCountProxy< BaseT > >(
            m_pProxy,
            pProxy,
            NULL );
        if( pExistingProxy )
        {
            SupportType::Release( pProxy );
            pProxy = pExistingProxy;
        }
    }

    return pProxy;
}

/// Constructor.
template< typename T >
Helium::StrongPtr< T >::StrongPtr()
    : m_pVoidProxy( NULL )
{
}

/// Constructor.
///
/// @param[in] pObject  Object to initially assign.
template< typename T >
Helium::StrongPtr< T >::StrongPtr( T* pObject )
    : m_pVoidProxy( NULL )
{
    if( pObject )
    {
        m_pVoidProxy = pObject->GetRefCountProxy();
        HELIUM_ASSERT( m_pVoidProxy );
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy )->AddStrongRef();
    }
}

/// Constructor.
///
/// @param[in] rPointer  Weak pointer from which to copy.
template< typename T >
Helium::StrongPtr< T >::StrongPtr( const WeakPtr< T >& rPointer )
    : m_pVoidProxy( rPointer.m_pVoidProxy )
{
    // Note that a weak pointer can have a reference count proxy set to null, so we need to check for and handle
    // that case as well.
    if( m_pVoidProxy )
    {
        RefCountProxy< typename T::RefCountSupportType::BaseType >* pProxy =
            static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy );
        if( pProxy->GetObject() )
        {
            pProxy->AddStrongRef();
        }
        else
        {
            m_pVoidProxy = NULL;
        }
    }
}

/// Copy constructor.
///
/// @param[in] rPointer  Strong pointer from which to copy.
template< typename T >
Helium::StrongPtr< T >::StrongPtr( const StrongPtr& rPointer )
    : m_pVoidProxy( rPointer.m_pVoidProxy )
{
    if( m_pVoidProxy )
    {
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy )->AddStrongRef();
    }
}

/// Destructor.
template< typename T >
Helium::StrongPtr< T >::~StrongPtr()
{
    Release();
}

/// Get the object referenced by this smart pointer.
///
/// @return  Pointer to the referenced object.
///
/// @see Set(), Release()
template< typename T >
T* Helium::StrongPtr< T >::Get() const
{
    return ( m_pVoidProxy
        ? static_cast< T* >( static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy )->GetObject() )
        : NULL );
}

/// Get the object referenced by this smart pointer.
///
/// @return  Pointer to the referenced object.
///
/// @see Set(), Release()
template< typename T >
T* Helium::StrongPtr< T >::Ptr() const
{
    return Get();
}

/// Set the object referenced by this smart pointer.
///
/// @param[in] pObject  Object to reference.
///
/// @see Get(), Release()
template< typename T >
void Helium::StrongPtr< T >::Set( T* pObject )
{
    RefCountProxy< typename T::RefCountSupportType::BaseType >* pProxy =
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy );
    if( pProxy )
    {
        if( pProxy->GetObject() == pObject )
        {
            return;
        }

        m_pVoidProxy = NULL;

        if( pProxy->RemoveStrongRef() )
        {
            typename T::RefCountSupportType::Release( pProxy );
        }
    }

    if( pObject )
    {
        m_pVoidProxy = pObject->GetRefCountProxy();
        HELIUM_ASSERT( m_pVoidProxy );
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy )->AddStrongRef();
    }
}

/// Release any object referenced by this smart pointer.
///
/// @see Get(), Set()
template< typename T >
void Helium::StrongPtr< T >::Release()
{
    RefCountProxy< typename T::RefCountSupportType::BaseType >* pProxy =
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy );
    m_pVoidProxy = NULL;

    if( pProxy && pProxy->RemoveStrongRef() )
    {
        typename T::RefCountSupportType::Release( pProxy );
    }
}

/// Get whether this smart pointer references an object.
///
/// @return  True if this smart pointer is set to a non-null pointer, false if it is null.
template< typename T >
bool Helium::StrongPtr< T >::ReferencesObject() const
{
    // Proxy object should never be holding a null reference for strong pointers, so we should only have to check
    // whether we have a proxy object set.
    HELIUM_ASSERT(
        !m_pVoidProxy ||
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy )->GetObject() );

    return ( m_pVoidProxy != NULL );
}

/// Directly write an object link table index to this smart pointer.
///
/// This is intended specifically for Data objects that perform object linking.
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
void Helium::StrongPtr< T >::SetLinkIndex( uint32_t index )
{
    reinterpret_cast< uintptr_t& >( m_pVoidProxy ) = index;
}

/// Get the object link table index stored in this smart pointer.
///
/// This is intended specifically for Data objects that perform object linking.
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
uint32_t Helium::StrongPtr< T >::GetLinkIndex() const
{
    return static_cast< uint32_t >( reinterpret_cast< const uintptr_t& >( m_pVoidProxy ) );
}

/// Clear out the link table index stored in this smart pointer.
///
/// This is intended specifically for Data objects that perform object linking.
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
void Helium::StrongPtr< T >::ClearLinkIndex()
{
    m_pVoidProxy = NULL;
}

/// Get the object referenced by this smart pointer.
///
/// @return  Pointer to the referenced object.
///
/// @see Set(), Release()
template< typename T >
Helium::StrongPtr< T >::operator T*() const
{
    return Get();
}

/// Implicit cast to a strong pointer of a base type.
///
/// @return  Constant reference to the cast strong pointer.
template< typename T >
template< typename BaseT >
Helium::StrongPtr< T >::operator const Helium::StrongPtr< BaseT >&() const
{
    return ImplicitUpCast< BaseT >( boost::is_base_of< BaseT, T >() );
}

/// Dereference this pointer.
///
/// @return  Reference to the actual object.
template< typename T >
T& Helium::StrongPtr< T >::operator*() const
{
    T* pObject = Get();
    HELIUM_ASSERT( pObject );

    return *pObject;
}

/// Dereference this pointer.
///
/// @return  Pointer to the actual object.
template< typename T >
T* Helium::StrongPtr< T >::operator->() const
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
Helium::StrongPtr< T >& Helium::StrongPtr< T >::operator=( T* pObject )
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
Helium::StrongPtr< T >& Helium::StrongPtr< T >::operator=( const WeakPtr< T >& rPointer )
{
    // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check for
    // and handle that case as well.
    RefCountProxy< typename T::RefCountSupportType::BaseType >* pOtherProxy =
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( rPointer.m_pVoidProxy );
    if( pOtherProxy && !pOtherProxy->GetObject() )
    {
        pOtherProxy = NULL;
    }

    if( m_pVoidProxy != pOtherProxy )
    {
        Release();

        m_pVoidProxy = pOtherProxy;
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
Helium::StrongPtr< T >& Helium::StrongPtr< T >::operator=( const StrongPtr& rPointer )
{
    RefCountProxy< typename T::RefCountSupportType::BaseType >* pOtherProxy =
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( rPointer.m_pVoidProxy );
    if( m_pVoidProxy != pOtherProxy )
    {
        Release();

        m_pVoidProxy = pOtherProxy;
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
bool Helium::StrongPtr< T >::operator==( const WeakPtr< T >& rPointer ) const
{
    // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check
    // for and handle that case as well.
    RefCountProxy< typename T::RefCountSupportType::BaseType >* pOtherProxy =
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( rPointer.m_pVoidProxy );
    if( pOtherProxy && !pOtherProxy->GetObject() )
    {
        pOtherProxy = NULL;
    }

    return ( m_pVoidProxy == pOtherProxy );
}

/// Equality comparison operator.
///
/// @param[in] rPointer  Smart pointer with which to compare.
///
/// @return  True if this smart pointer and the given smart pointer reference the same object, false if not.
template< typename T >
bool Helium::StrongPtr< T >::operator==( const StrongPtr& rPointer ) const
{
    return ( m_pVoidProxy == rPointer.m_pVoidProxy );
}

/// Inequality comparison operator.
///
/// @param[in] rPointer  Smart pointer with which to compare.
///
/// @return  True if this smart pointer and the given smart pointer do not reference the same object, false if they do.
template< typename T >
bool Helium::StrongPtr< T >::operator!=( const WeakPtr< T >& rPointer ) const
{
    // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check for
    // and handle that case as well.
    RefCountProxy< typename T::RefCountSupportType::BaseType >* pOtherProxy =
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType > >( rPointer.m_pVoidProxy );
    if( pOtherProxy && !pOtherProxy->GetObject() )
    {
        pOtherProxy = NULL;
    }

    return ( m_pVoidProxy != pOtherProxy );
}

/// Inequality comparison operator.
///
/// @param[in] rPointer  Smart pointer with which to compare.
///
/// @return  True if this smart pointer and the given smart pointer do not reference the same object, false if they do.
template< typename T >
bool Helium::StrongPtr< T >::operator!=( const StrongPtr& rPointer ) const
{
    return ( m_pVoidProxy != rPointer.m_pVoidProxy );
}

/// Helper function for performing a compile-time verified up-cast of a StrongPtr.
///
/// @param[in] rIsProperBase  Instance of boost::is_base_of< BaseT, T > if it inherits from boost::true_type.
///
/// @return  Constant reference to the cast strong pointer.
template< typename T >
template< typename BaseT >
const Helium::StrongPtr< BaseT >& Helium::StrongPtr< T >::ImplicitUpCast(
    const boost::true_type& /*rIsProperBase*/ ) const
{
    return *reinterpret_cast< const StrongPtr< BaseT >* >( this );
}

/// Constructor.
template< typename T >
Helium::WeakPtr< T >::WeakPtr()
    : m_pVoidProxy( NULL )
{
}

/// Constructor.
///
/// @param[in] pObject  Object to initially assign.
template< typename T >
Helium::WeakPtr< T >::WeakPtr( T* pObject )
    : m_pVoidProxy( NULL )
{
    if( pObject )
    {
        m_pVoidProxy = pObject->GetRefCountProxy();
        HELIUM_ASSERT( m_pVoidProxy );
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy )->AddWeakRef();
    }
}

/// Constructor.
///
/// @param[in] rPointer  Strong pointer from which to copy.
template< typename T >
Helium::WeakPtr< T >::WeakPtr( const StrongPtr< T >& rPointer )
    : m_pVoidProxy( rPointer.m_pVoidProxy )
{
    if( m_pVoidProxy )
    {
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy )->AddWeakRef();
    }
}

/// Copy constructor.
///
/// @param[in] rPointer  Weak pointer from which to copy.
template< typename T >
Helium::WeakPtr< T >::WeakPtr( const WeakPtr& rPointer )
    : m_pVoidProxy( rPointer.m_pVoidProxy )
{
    // Note that a weak pointer can have a reference count proxy set to null, so we need to check for and handle
    // that case as well.
    if( m_pVoidProxy )
    {
        RefCountProxy< typename T::RefCountSupportType::BaseType >* pProxy =
            static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy );
        if( pProxy->GetObject() )
        {
            pProxy->AddWeakRef();
        }
        else
        {
            m_pVoidProxy = NULL;
        }
    }
}

/// Destructor.
template< typename T >
Helium::WeakPtr< T >::~WeakPtr()
{
    Release();
}

/// Get the object referenced by this smart pointer.
///
/// @return  Pointer to the referenced object.
///
/// @see Set(), Release()
template< typename T >
T* Helium::WeakPtr< T >::Get() const
{
    return ( m_pVoidProxy
        ? static_cast< T* >( static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy )->GetObject() )
        : NULL );
}

/// Get the object referenced by this smart pointer.
///
/// @return  Pointer to the referenced object.
///
/// @see Set(), Release()
template< typename T >
T* Helium::WeakPtr< T >::Ptr() const
{
    return Get();
}

/// Set the object referenced by this smart pointer.
///
/// @param[in] pObject  Object to reference.
///
/// @see Get(), Release()
template< typename T >
void Helium::WeakPtr< T >::Set( T* pObject )
{
    RefCountProxy< typename T::RefCountSupportType::BaseType >* pProxy =
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy );
    if( pProxy )
    {
        // Note that a weak pointer can have a reference count proxy set to null, so we need to check for and handle
        // that case as well.
        if( pObject && pProxy->GetObject() == pObject )
        {
            return;
        }

        m_pVoidProxy = NULL;

        if( pProxy->RemoveWeakRef() )
        {
            typename T::RefCountSupportType::Release( pProxy );
        }
    }

    if( pObject )
    {
        m_pVoidProxy = pObject->GetRefCountProxy();
        HELIUM_ASSERT( m_pVoidProxy );
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy )->AddWeakRef();
    }
}

/// Release any object referenced by this smart pointer.
///
/// @see Get(), Set()
template< typename T >
void Helium::WeakPtr< T >::Release()
{
    RefCountProxy< typename T::RefCountSupportType::BaseType >* pProxy =
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy );
    m_pVoidProxy = NULL;

    if( pProxy && pProxy->RemoveWeakRef() )
    {
        typename T::RefCountSupportType::Release( pProxy );
    }
}

/// Get whether this smart pointer references an object.
///
/// @return  True if this smart pointer is set to a non-null pointer, false if it is null.
template< typename T >
bool Helium::WeakPtr< T >::ReferencesObject() const
{
    // Proxy object can be holding a null reference for weak pointers, so make sure the actual object reference exists.
    return ( Get() != NULL );
}

/// Get whether this weak pointer is holding onto the reference counting proxy object for the given object.
///
/// @return  True if this object is holding onto the given object's reference counting proxy, false if not.
template< typename T >
bool Helium::WeakPtr< T >::HasObjectProxy( const T* pObject ) const
{
    HELIUM_ASSERT( pObject );

    return ( m_pVoidProxy == pObject->GetRefCountProxy() );
}

/// Get the object referenced by this smart pointer.
///
/// @return  Pointer to the referenced object.
///
/// @see Set(), Release()
template< typename T >
Helium::WeakPtr< T >::operator T*() const
{
    return Get();
}

/// Implicit cast to a weak pointer of a base type.
///
/// @return  Constant reference to the cast weak pointer.
template< typename T >
template< typename BaseT >
Helium::WeakPtr< T >::operator const Helium::WeakPtr< BaseT >&() const
{
    return ImplicitUpCast< BaseT >( boost::is_base_of< BaseT, T >() );
}

/// Dereference this pointer.
///
/// @return  Reference to the actual object.
template< typename T >
T& Helium::WeakPtr< T >::operator*() const
{
    T* pObject = Get();
    HELIUM_ASSERT( pObject );

    return *pObject;
}

/// Dereference this pointer.
///
/// @return  Pointer to the actual object.
template< typename T >
T* Helium::WeakPtr< T >::operator->() const
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
Helium::WeakPtr< T >& Helium::WeakPtr< T >::operator=( T* pObject )
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
Helium::WeakPtr< T >& Helium::WeakPtr< T >::operator=( const StrongPtr< T >& rPointer )
{
    RefCountProxy< typename T::RefCountSupportType::BaseType >* pOtherProxy =
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( rPointer.m_pVoidProxy );
    if( m_pVoidProxy != pOtherProxy )
    {
        Release();

        m_pVoidProxy = pOtherProxy;
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
Helium::WeakPtr< T >& Helium::WeakPtr< T >::operator=( const WeakPtr& rPointer )
{
    // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check for
    // and handle that case as well.
    RefCountProxy< typename T::RefCountSupportType::BaseType >* pOtherProxy =
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( rPointer.m_pVoidProxy );
    if( pOtherProxy && !pOtherProxy->GetObject() )
    {
        pOtherProxy = NULL;
    }

    if( m_pVoidProxy != pOtherProxy )
    {
        Release();

        m_pVoidProxy = pOtherProxy;
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
bool Helium::WeakPtr< T >::operator==( const StrongPtr< T >& rPointer ) const
{
    // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check for
    // and handle that case as well.
    RefCountProxy< typename T::RefCountSupportType::BaseType >* pThisProxy =
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy );
    if( pThisProxy && !pThisProxy->GetObject() )
    {
        pThisProxy = NULL;
    }

    return ( pThisProxy == rPointer.m_pVoidProxy );
}

/// Equality comparison operator.
///
/// @param[in] rPointer  Smart pointer with which to compare.
///
/// @return  True if this smart pointer and the given smart pointer reference the same object, false if not.
template< typename T >
bool Helium::WeakPtr< T >::operator==( const WeakPtr& rPointer ) const
{
    // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check for
    // and handle that case as well.
    return ( Get() == rPointer.Get() );
}

/// Inequality comparison operator.
///
/// @param[in] rPointer  Smart pointer with which to compare.
///
/// @return  True if this smart pointer and the given smart pointer do not reference the same object, false if they do.
template< typename T >
bool Helium::WeakPtr< T >::operator!=( const StrongPtr< T >& rPointer ) const
{
    // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check for
    // and handle that case as well.
    RefCountProxy< typename T::RefCountSupportType::BaseType >* pThisProxy =
        static_cast< RefCountProxy< typename T::RefCountSupportType::BaseType >* >( m_pVoidProxy );
    if( pThisProxy && !pThisProxy->GetObject() )
    {
        pThisProxy = NULL;
    }

    return ( pThisProxy != rPointer.m_pVoidProxy );
}

/// Inequality comparison operator.
///
/// @param[in] rPointer  Smart pointer with which to compare.
///
/// @return  True if this smart pointer and the given smart pointer do not reference the same object, false if they do.
template< typename T >
bool Helium::WeakPtr< T >::operator!=( const WeakPtr& rPointer ) const
{
    // Note that a weak pointer can have a reference count proxy whose object is set to null, so we need to check for
    // and handle that case as well.
    return ( Get() != rPointer.Get() );
}

/// Helper function for performing a compile-time verified up-cast of a WeakPtr.
///
/// @param[in] rIsProperBase  Instance of boost::is_base_of< BaseT, T > if it inherits from boost::true_type.
///
/// @return  Constant reference to the cast weak pointer.
template< typename T >
template< typename BaseT >
const Helium::WeakPtr< BaseT >& Helium::WeakPtr< T >::ImplicitUpCast(
    const boost::true_type& /*rIsProperBase*/ ) const
{
    return *reinterpret_cast< const WeakPtr< BaseT >* >( this );
}
