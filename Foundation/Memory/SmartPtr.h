#pragma once

#include "Foundation/API.h"

#include "Platform/Utility.h"

//
// Types
//

#ifndef NULL
#define NULL (0)
#endif

namespace Helium
{
    //
    // The base class
    //  Derive from this to enable tracking and cleanup of objects automagically
    //

    class FOUNDATION_API RefCountBase
    {
    private:
        mutable int32_t m_RefCount;

    public:
        RefCountBase()
            : m_RefCount (0)
        {
        }

        RefCountBase(const RefCountBase& rhs)
            : m_RefCount (0)
        {
        }

        virtual ~RefCountBase();

        int32_t GetRefCount() const
        {
            return m_RefCount;
        }

        void IncrRefCount() const
        {
            m_RefCount++;
        }

        void DecrRefCount() const
        {
            m_RefCount--;

            HELIUM_ASSERT( m_RefCount >= 0 );

            if (m_RefCount == 0)
            {
                delete this;
            }
        }

        RefCountBase& operator=(const RefCountBase& rhs)
        {
            // do NOT copy the refcount
            return *this;
        }
    };


    //
    // The aggregator
    //  Use this to aggregate data that cannot be derived from the base class above (such as compiler or library types)
    //

    template<typename T>
    class RefCountAggregator : public RefCountBase
    {
    public:
        T m_Object;

        RefCountAggregator()
        {

        }

        RefCountAggregator(const T& rhs)
            : m_Object (rhs)
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

    template <typename T>
    class SmartPtr
    {
        // Allow access to other template's internal pointer
        template <typename U>
        friend class SmartPtr;

    public:
        SmartPtr() : m_Pointer (NULL)
        {

        }

        inline SmartPtr(const T *p) : m_Pointer(const_cast<T*>(p))
        {
            if (m_Pointer)
            {
                m_Pointer->IncrRefCount();
            }
        }

        inline SmartPtr(const SmartPtr &r) : m_Pointer ( NULL )
        {
            m_Pointer = r.m_Pointer;

            if (m_Pointer)
            {
                m_Pointer->IncrRefCount();
            }
        }

        template<typename U>
        inline SmartPtr(const SmartPtr<U> &r) : m_Pointer ( NULL )
        {
            m_Pointer = r.m_Pointer;

            if (m_Pointer)
            {
                m_Pointer->IncrRefCount();
            }
        }

        ~SmartPtr()
        {
            operator=(NULL);
        }

        inline SmartPtr& operator=(const T* p)
        {
            // just in case, we do the increment before the decrement, and prior to the assignment
            T* new_pointer = const_cast<T*>(p);

            // notice the validity check
            if (new_pointer != NULL)
            {
                // and increase a reference on the new guy we look at
                new_pointer->IncrRefCount();
            }

            if ( m_Pointer != NULL )
            {
                // forget one reference on who we look at...
                m_Pointer->DecrRefCount();
            }

            // do the assignment
            m_Pointer = new_pointer;

            return *this;
        }

        inline SmartPtr& operator=(const SmartPtr& p)
        {
            return operator=(p.m_Pointer);
        }

        template<typename U>
        inline SmartPtr& operator=(const SmartPtr<U> &r)
        {
            return operator=(r.m_Pointer);
        }

        inline bool operator==(const T* p) const
        {
            return m_Pointer == p;
        }

        inline bool operator== (const SmartPtr& p) const
        {
            return operator==(p.m_Pointer);
        }

        template<typename U>
        inline bool operator==(const SmartPtr<U>& r)
        {
            return operator==(r.m_Pointer);
        }

        inline bool operator!=(const T* p) const
        {
            return !operator==(p);
        }

        inline bool operator!=(const SmartPtr& p) const
        {
            return operator!=(p.m_Pointer);
        }

        template<typename U>
        inline bool operator!=(const SmartPtr<U> &r)
        {
            return operator!=(r.m_Pointer);
        }

        inline T* operator->() const
        {
            return m_Pointer;
        }

        inline operator T*() const
        {
            return m_Pointer;
        }

        inline bool ReferencesObject() const
        {
            return m_Pointer != NULL;
        }

        inline T* Ptr() const
        {
            return m_Pointer;
        }

    private:
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
