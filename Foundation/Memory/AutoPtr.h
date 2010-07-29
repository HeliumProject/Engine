#pragma once

#include "Platform/Types.h"
#include "Platform/Assert.h"

namespace Helium
{
    //
    // This is a simple manager for a C-style array allocated with new [].
    //  We don't use auto_ptr because of the delete semantics. 
    //  This class is probably only good for the simplest use cases.
    // 
    // example: 
    //   AutoPtr<Foo> foo( new Foo ); 
    // 

    template <typename T>
    struct AutoPtr
    {
    public:
        AutoPtr( T* ptr = NULL ) : m_Ptr( ptr )
        {

        }

    private:
        AutoPtr( const AutoPtr& rhs )
        {

        }

    public:
        ~AutoPtr()
        {
            delete m_Ptr; 
        }

        T* Ptr()
        {
            return m_Ptr; 
        }

        const T* operator->() const
        {
            return m_Ptr;
        }

        T* operator->()
        {
            return m_Ptr;
        }

    private: 
        T* m_Ptr; 
    };
}
