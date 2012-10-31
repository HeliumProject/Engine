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
    //   ArrayPtr<char> array( new char[24] ); 
    // 

    template <typename T>
    struct ArrayPtr
    {
    public: 
        ArrayPtr( T* ptr = NULL ) : m_Ptr( ptr )
        {

        }

    private:
        ArrayPtr( const ArrayPtr& rhs )
        {

        }

    public:
        ~ArrayPtr()
        {
            delete [] m_Ptr; 
        }

        T* Ptr()
        {
            return m_Ptr; 
        }

        const T& operator[]( size_t i ) const
        {
            return m_Ptr[i];
        }

        T& operator[]( size_t i )
        {
            return m_Ptr[i];
        }

    private: 
        T* m_Ptr; 
    };
}
