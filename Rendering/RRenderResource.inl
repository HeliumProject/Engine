//----------------------------------------------------------------------------------------------------------------------
// RRenderResource.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Constructor.
    ///
    /// Note that render resources are always constructed with an initial reference count of 0.  Render resources should
    /// always be assigned to an RSmartPtr upon creation in order to begin the reference counting life cycle.
    RRenderResource::RRenderResource()
        : m_referenceCount( 0 )
    {
    }

    /// Increment the reference count for the specified render resource.
    ///
    /// Note that reference count updating is thread-safe (references can be added and removed concurrently from
    /// multiple threads), although the actual process of freeing a resource once the reference count reaches zero may
    /// not be.
    ///
    /// @param[in] pResource  Render resource to update.
    ///
    /// @return  Reference count after incrementing.
    ///
    /// @see Release()
    uint32_t RReferenceCountProxy::AddRef( RRenderResource* pResource )
    {
        uint32_t referenceCountNew = AtomicIncrementAcquire( pResource->m_referenceCount );

        return referenceCountNew;
    }

    /// Decrement the reference count for the specified render resource, destroying it immediately once the reference
    /// count reaches zero.
    ///
    /// Note that reference count updating is thread-safe (references can be added and removed concurrently from
    /// multiple threads), although the actual process of freeing a resource once the reference count reaches zero may
    /// not be.
    ///
    /// @param[in] pResource  Render resource to update.
    ///
    /// @return  Reference count after decrementing.
    ///
    /// @see AddRef()
    uint32_t RReferenceCountProxy::Release( RRenderResource* pResource )
    {
        uint32_t referenceCountNew = AtomicDecrementRelease( pResource->m_referenceCount );
        if( referenceCountNew == 0 )
        {
            delete pResource;
        }

        return referenceCountNew;
    }

    /// Constructor.
    ///
    /// @param[in] pResource  Render resource to which this reference should be initialized.
    template< typename T >
    RSmartPtr< T >::RSmartPtr( T* pResource )
        : m_pResource( pResource )
    {
        if( pResource )
        {
            RReferenceCountProxy::AddRef( pResource );
        }
    }

    /// Copy constructor.
    ///
    /// @param[in] rSource  Render resource reference to which this reference should be initialized.
    template< typename T >
    RSmartPtr< T >::RSmartPtr( const RSmartPtr& rSource )
        : m_pResource( rSource.m_pResource )
    {
        if( m_pResource )
        {
            RReferenceCountProxy::AddRef( m_pResource );
        }
    }

    /// Destructor.
    template< typename T >
    RSmartPtr< T >::~RSmartPtr()
    {
        if( m_pResource )
        {
            RReferenceCountProxy::Release( m_pResource );
        }
    }

    /// Get the render resource currently referenced by this smart pointer.
    ///
    /// @return  Pointer to the referenced render resource.
    ///
    /// @see Set()
    template< typename T >
    T* RSmartPtr< T >::Get() const
    {
        return m_pResource;
    }

    /// Set the render resource referenced by this smart pointer.
    ///
    /// @param[in] pResource  Render resource to reference.
    ///
    /// @see Get()
    template< typename T >
    void RSmartPtr< T >::Set( T* pResource )
    {
        T* pResourceOld = m_pResource;
        if( pResourceOld != pResource )
        {
            m_pResource = pResource;

            if( pResourceOld )
            {
                RReferenceCountProxy::Release( pResourceOld );
            }

            if( pResource )
            {
                RReferenceCountProxy::AddRef( pResource );
            }
        }
    }

    /// Release any currently set render resource reference, decrementing its reference count.
    ///
    /// @see Set(), Get()
    template< typename T >
    void RSmartPtr< T >::Release()
    {
        T* pResource = m_pResource;
        if( pResource )
        {
            RReferenceCountProxy::Release( pResource );
            m_pResource = NULL;
        }
    }

    /// Dereference this render resource reference.
    ///
    /// @return  Reference to the currently referenced render resource.
    template< typename T >
    T& RSmartPtr< T >::operator*() const
    {
        HELIUM_ASSERT( m_pResource );

        return *m_pResource;
    }

    /// Dereference this render resource reference.
    ///
    /// @return  Pointer to the currently referenced render resource.
    template< typename T >
    T* RSmartPtr< T >::operator->() const
    {
        HELIUM_ASSERT( m_pResource );

        return m_pResource;
    }

    /// Cast this reference to a bare render resource pointer.
    ///
    /// @return  Pointer to the currently referenced render resource.
    template< typename T >
    RSmartPtr< T >::operator T* const&() const
    {
        return m_pResource;
    }


    /// Assignment operator.
    ///
    /// @param[in] pResource  Render resource reference to set.
    ///
    /// @return  Reference to this object.
    template< typename T >
    RSmartPtr< T >& RSmartPtr< T >::operator=( T* pResource )
    {
        Set( pResource );

        return *this;
    }

    /// Assignment operator.
    ///
    /// @param[in] rSource  Render resource reference to set.
    ///
    /// @return  Reference to this object.
    template< typename T >
    RSmartPtr< T >& RSmartPtr< T >::operator=( const RSmartPtr& rSource )
    {
        Set( rSource.m_pResource );

        return *this;
    }
}
