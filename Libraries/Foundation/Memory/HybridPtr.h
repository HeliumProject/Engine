#pragma once

#include "Platform/Types.h"
#include "Platform/Assert.h"

//
// Types
//

#ifndef NULL
#define NULL (0)
#endif

namespace Nocturnal
{
    //
    // HybridPtr safely handles either a mutable OR a const pointer interchangably.
    //  The C++ 'const' keyword is great for compile-time error checking in APIs, however
    //  in order to support const and non-const (mutable) pointers you usually have to
    //  duplicate code.  This class stores the const-ness of the pointer and makes a
    //  runtime check if you try and reference it in an internal api as non-const
    //  if it was stored as const.  Its wierd, but better than const_cast<> IMO.
    //
    // -Geoff (12/07)
    //

    namespace PointerStates
    {
        enum PointerState
        {
            Mutable,
            Constant,
        };
    }
    typedef PointerStates::PointerState PointerState;

    template<class T>
    class HybridPtr
    {
        //
        // Allow access to other template's internal pointer
        //

        template <typename U> friend class HybridPtr;

    private:

        //
        // The pointers (unioned together because we don't need both)
        //

        union
        {
            T*        m_Mutable;
            const T*  m_Constant;
        };

        //
        // The const state of the pointer
        //

        PointerState m_State;

    public:
        HybridPtr()
            : m_Mutable (NULL)
            , m_State (PointerStates::Mutable)
        {

        }

        HybridPtr(PointerSizedUInt address, PointerState state)
            : m_State (state)
        {
            switch (m_State)
            {
            case PointerStates::Mutable:
                m_Mutable = (T*)address;
                break;

            case PointerStates::Constant:
                m_Constant = (const T*)address;
                break;
            }
        }

        HybridPtr(const HybridPtr& rhs)
            : m_Mutable (rhs.m_Mutable)
            , m_State (rhs.m_State)
        {

        }

        template<class U>
        HybridPtr(const HybridPtr<U>& rhs)
            : m_Mutable (rhs.m_Mutable)
            , m_State (rhs.m_State)
        {

        }

        HybridPtr(T* ptr)
        {
            operator=(ptr);
        }

        HybridPtr(const T* ptr)
        {
            operator=(ptr);
        }

        template<class U>
        HybridPtr(U* ptr)
            : m_Mutable (ptr)
            , m_State (PointerStates::Mutable)
        {

        }

        template<class U>
        HybridPtr(const U* ptr)
            : m_Constant (ptr)
            , m_State (PointerStates::Constant)
        {

        }

        HybridPtr& operator=(T* ptr)
        {
            m_Mutable = ptr;
            m_State = PointerStates::Mutable;
            return *this;
        }

        HybridPtr& operator=(const T* ptr)
        {
            m_Constant = ptr;
            m_State = PointerStates::Constant;
            return *this;
        }

        bool operator==(T* ptr)
        {
            return m_Mutable == ptr;
        }

        bool operator!=(T* ptr)
        {
            return !operator==(ptr);
        }

        bool operator==(const T* ptr)
        {
            return m_Constant == ptr;
        }

        bool operator!=(const T* ptr)
        {
            return !operator==(ptr);
        }

        T* operator->()
        {
            NOC_ASSERT(m_State == PointerStates::Mutable);
            return m_Mutable;
        }

        const T* operator->() const
        {
            return m_Constant;
        }

        operator T*()
        {
            NOC_ASSERT(m_State == PointerStates::Mutable);
            return m_Mutable;
        }

        operator const T*() const
        {
            return m_Constant;
        }

        T* Mutable()
        {
            NOC_ASSERT(m_State == PointerStates::Mutable);
            return m_Mutable;
        }

        const T* Constant() const
        {
            return m_Constant;
        }

        PointerSizedUInt Address()
        {
            return (PointerSizedUInt)m_Mutable;
        }

        PointerState State()
        {
            return m_State;
        }
    };
}
