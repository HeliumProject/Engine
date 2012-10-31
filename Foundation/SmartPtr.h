#pragma once

#include "Platform/Atomic.h"
#include "Platform/Utility.h"

namespace Helium
{
    /// Base class for non-atomic reference counting support.
    template< typename T >
    class RefCountBase
    {
    private:
        /// Reference count.
        mutable uint32_t m_RefCount;

    public:
        /// @name Construction/Destruction
        //@{
        RefCountBase();
        RefCountBase( const RefCountBase& rSource );
        //@}

        /// @name Reference Counting
        //@{
        uint32_t GetRefCount() const;
        uint32_t IncrRefCount() const;
        uint32_t DecrRefCount() const;
        //@}

        /// @name Overloaded Operators
        //@{
        RefCountBase& operator=( const RefCountBase& rSource );
        //@}
    };

    /// Base class for atomic reference counting support.
    template< typename T >
    class AtomicRefCountBase
    {
    private:
        /// Reference count.
        mutable volatile int32_t m_RefCount;

    public:
        /// @name Construction/Destruction
        //@{
        AtomicRefCountBase();
        AtomicRefCountBase( const AtomicRefCountBase& rSource );
        virtual ~AtomicRefCountBase();
        //@}

        /// @name Reference Counting
        //@{
        uint32_t GetRefCount() const;
        uint32_t IncrRefCount() const;
        uint32_t DecrRefCount() const;
        //@}

        /// @name Overloaded Operators
        //@{
        AtomicRefCountBase& operator=( const AtomicRefCountBase& rSource );
        //@}
    };


    //
    // The aggregator
    //  Use this to aggregate data that cannot be derived from the base class above (such as compiler or library types)
    //

    template<typename T>
    class RefCountAggregator : public RefCountBase<T>
    {
    public:
        T m_Object;

        RefCountAggregator()
        {
        }

        RefCountAggregator( const T& rhs )
            : m_Object( rhs )
        {
        }

        inline T* operator->() const
        {
            return &m_Object;
        }

        inline operator T*() const
        {
            return &m_Object;
        }
    };


    //
    // SmartPtr safely manages the reference count on the stack
    //

    template< typename T >
    class SmartPtr
    {
        template< typename U > friend class SmartPtr;

    public:
        /// @name Construction/Destruction
        //@{
        SmartPtr();
        SmartPtr( const T* pPointer );
        SmartPtr( const SmartPtr& rPointer );
        template< typename U > SmartPtr( const SmartPtr< U >& rPointer );
        ~SmartPtr();
        //@}

        /// @name Data Access
        //@{
        T* Get() const;
        T* Ptr() const;
        void Set( const T* pResource );
        void Release();

        bool ReferencesObject() const;
        //@}

        /// @name Overloaded Operators
        //@{
        T& operator*() const;
        T* operator->() const;

        operator T* const&() const;

        SmartPtr& operator=( const T* pPointer );
        SmartPtr& operator=( const SmartPtr& rPointer );
        template< typename U > SmartPtr& operator=( const SmartPtr< U >& rPointer );
        //@}

    private:
        /// Object referenced by this smart pointer.
        T* m_Pointer;
    };


    //
    // SmartPtrComparator
    //   Used in containers to compare what's being pointed to by the SmartPtrs rather than the pointers themselves
    //

    template <typename T>
    class SmartPtrComparator
    {
    public:
        SmartPtrComparator()
            : m_SmartPtr( NULL )
        {
        }

        SmartPtrComparator( const Helium::SmartPtr<T>& smartPtr )
            : m_SmartPtr( smartPtr )
        {
        }

        SmartPtrComparator( const T* smartPtr )
            : m_SmartPtr( smartPtr )
        {
        }

        inline bool operator<( const SmartPtrComparator& rhs ) const
        {
            return (*m_SmartPtr) < (*rhs.m_SmartPtr);
        }

        inline bool operator==( const SmartPtrComparator& rhs ) const
        {
            return (*m_SmartPtr) == (*rhs.m_SmartPtr);
        }

        const T* operator->() const
        {
            return m_SmartPtr.operator->();
        }

        T* operator->()
        {
            return m_SmartPtr.operator->();
        }

        operator const T* () const
        {
            return m_SmartPtr.operator T *();
        }

        operator T* ()
        {
            return m_SmartPtr.operator T *();
        }

        operator const Helium::SmartPtr<T> () const
        {
            return m_SmartPtr;
        }

        operator Helium::SmartPtr<T> ()
        {
            return m_SmartPtr;
        }

    private:
        Helium::SmartPtr<T> m_SmartPtr;
    };
}

#include "Foundation/SmartPtr.inl"
