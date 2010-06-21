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
    // This is a simple manager for a C-style array allocated with new [].
    //  We don't use auto_ptr because of the delete semantics. 
    //  This class is probably only good for the simplest use cases.
    // 
    // example: 
    //   ArrayPtr<char> array( new char(24) ); 
    // 

    template <typename T>
    struct ArrayPtr
    {
    private: 
        T* m_Ptr; 

    public: 
        ArrayPtr(T* ptr) : m_Ptr(ptr)
        {

        }

        ~ArrayPtr()
        {
            delete [] m_Ptr; 
        }

        T* Ptr()
        {
            return m_Ptr; 
        }
    };
}
