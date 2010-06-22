#pragma once

#include <string>
#include <vector>

//
// The gist of things goes like this, we have debug dlls that need to pass data
//  back and forth between release dlls, and since stl containers are not binary
//  compatible, we cannot just overload the stl container allocator parameter.
//  These class are just intermediaries to hold Safe allocations across a call.
//
// The containers defined in here are merely to facilitate copies of data to/from
//  Safe::Arrays and stl container types (Vector, string, etc).  These classes
//  should NEVER EVER implement convenience logic, otherwise they will deteriorate
//  into our own string and Vector classes, which they are not intended to be
//
// Geoff (3/15/06)
//

namespace Safe
{
    //
    // Memory, frontend for the CRT-agnostic win32 process heap and c++ helpers for construct/destruct calls
    //

    template <typename T>
    class Memory
    {
    public:
        static T* Allocate(size_t count)
        {
            // #define HEAP_GENERATE_EXCEPTIONS 0x00000004      
            return (T*)HeapAlloc(GetProcessHeap(), 0x00000004, (unsigned long)sizeof(T)*count);
        }

        static T* Reallocate(T* ptr, size_t count)
        {
            // #define HEAP_GENERATE_EXCEPTIONS 0x00000004      
            return (T*)HeapReAlloc(GetProcessHeap(), 0x00000004, (void*)ptr, (unsigned long)sizeof(T)*count);
        }

        static void Deallocate(T*& ptr)
        {
            // free, #define HEAP_GENERATE_EXCEPTIONS 0x00000004      
            HeapFree(GetProcessHeap(), 0x00000004, (void*)ptr);

            // null out our ptr
            ptr = NULL;
        }

        static void Construct(T* ptr)
        {
            // placement new will handle builtin types vs. user types
            new ((void*)ptr) T;
        }

        static void Construct(T* ptr, size_t count)
        {
            for ( T* end = ptr + count; ptr != end; ++ptr )
            {
                construct(ptr);
            }
        }

        static void Destruct(T* ptr)
        {
            // note that "T" here HAS to be a template parameter, otherwise builtin types will generate compile error
            // apparently only the template compiler knows how to differentiate between builtin and user types (without
            // entering builtin type specialization hell)
            (ptr)->~T();
        }

        static void Destruct(T* ptr, size_t count)
        {
            for ( T* end = ptr + count; ptr != end; ++ptr )
            {
                destruct(ptr);
            }
        }
    };


    //
    // Array
    //

    template <typename T>
    class Array
    {
    protected:
        T*      m_Data;
        size_t  m_Size;

    public:
        Array()
            : m_Data (0x0)
            , m_Size (0)
        {

        }

        Array(size_t size)
            : m_Data (0x0)
            , m_Size (size)
        {
            if (m_Size > 0)
            {
                // alloc Memoryory
                m_Data = Memory<T>::Allocate(size);

                // run our constructors
                Memory<T>::Construct(m_Data, size);
            }
        }

        Array(const Array<T>& rhs)
            : m_Data (0x0)
            , m_Size (rhs.m_Size)
        {
            if (m_Size > 0)
            {
                // alloc Memoryory
                m_Data = Memory<T>::Allocate(m_Size);

                // copy
                const T* itr = rhs.m_Data;
                const T* end = rhs.m_Data + rhs.m_Size;
                for ( size_t index=0; itr != end; ++itr, ++index )
                {
                    // construct this element
                    Memory<T>::Construct(&m_Data[index]);

                    // copy data from rhs
                    m_Data[index] = *itr;
                }
            }
        }

        virtual ~Array()
        {
            if (m_Data != NULL)
            {
                // run our destructors
                Memory<T>::Destruct(m_Data, m_Size);

                // free our allocation
                Memory<T>::Deallocate(m_Data);
            }
        }

        Array<T>& operator=(const Array<T>& rhs)
        {
            // realloc
            resize(rhs.size());

            if (m_Size > 0)
            {
                // copy
                const T* itr = rhs.m_Data;
                const T* end = rhs.m_Data + rhs.m_Size;
                for ( size_t index=0; itr != end; ++itr, ++index )
                {
                    // copy data from rhs
                    m_Data[index] = *itr;
                }
            }

            return *this;
        }

        bool operator==(const Array<T>& rhs) const
        {
            // early out of counts don't match
            if (m_Size != rhs.m_Size)
            {
                return false;
            }

            // walk lhs
            const T* itrL = m_Data;
            const T* endL = m_Data + m_Size;

            // walk rhs
            const T* itrR = rhs.m_Data;
            const T* endR = rhs.m_Data + rhs.m_Size;

            // iterate
            for ( ; itrL != endL && itrR != endR; ++itrL, ++itrR )
            {
                // if any element does not match
                if (*itrL != *itrR)
                {
                    // fail
                    return false;
                }
            }

            return true;
        }

        bool operator!=(const Array<T>& rhs) const
        {
            return !operator==(rhs);
        }

        virtual T* data() const
        {
            return m_Data;
        }

        virtual bool empty() const
        {
            return size() == 0;
        }

        virtual size_t size() const
        {
            return m_Size;
        }

        virtual size_t length() const
        {
            return size();
        }

        virtual void reserve(size_t new_size)
        {
            if(new_size > m_Size)
            {
                resize(new_size);
            }
        }

        virtual void resize(size_t new_size)
        {
            if (m_Size == 0)
            {
                if (new_size == 0)
                {
                    // zero to zero, nothing to do
                    return;
                }
                else
                {
                    // zero to nonzero, alloc a new region
                    m_Data = Memory<T>::Allocate(new_size);

                    // run our constructors
                    Memory<T>::Construct(m_Data, new_size);
                }
            }
            else
            {
                if (new_size == 0)
                {
                    // run our destructors
                    Memory<T>::Destruct(m_Data, m_Size);

                    // nonzero to zero, free our allocation
                    Memory<T>::Deallocate(m_Data);
                }
                else if (new_size != m_Size)
                {
                    // nonzero to different nonzero, realloc to a new size
                    m_Data = Memory<T>::Reallocate(m_Data, new_size);

                    if (new_size > m_Size)
                    {
                        // run constructors for our new objects
                        Memory<T>::Construct(m_Data + m_Size, new_size - m_Size);
                    }
                }
            }

            m_Size = new_size;
        }
    };


    //
    // Vector
    //

    template <typename T>
    class Vector : public Array<T>
    {
    public:
        Vector ()
            : Array<T> ()
        {

        }

        Vector (const Vector<T>& rhs)
            : Array<T> (rhs)
        {

        }

        template <typename U>
        Vector (const std::vector<U>& vec)
        {
            operator=(vec);
        }

        template <typename U>
        Vector<T>& operator=(const std::vector<U>& vec)
        {
            resize(vec.size());

            if (m_Size > 0)
            {
                std::vector<U>::const_iterator itr = vec.begin();
                std::vector<U>::const_iterator end = vec.end();
                for ( size_t index=0; itr != end; ++itr, ++index )
                {
                    m_Data[index] = *itr;
                }
            }

            return *this;
        }

        template <typename U>
        inline operator std::vector<U>() const
        {
            std::vector<U> vec;

            vec.resize(m_Size);

            if (m_Size > 0)
            {
                const T* itr = m_Data;
                const T* end = m_Data + m_Size;
                for ( size_t index=0; itr != end; ++itr, ++index )
                {
                    vec[index] = *itr;
                }
            }

            return vec;
        }
    };


    //
    // String
    //

    template <typename C>
    class String
    {
    protected:
        Array<C> m_string;

    public:
        String ()
        {
            m_string.resize(1);
            m_string.data()[0] = '\0';
        }

        String (const C* str)
        {
            operator=(str);
        }

        String (const String<C>& rhs)
        {
            operator=(rhs.data());
        }

        String (const std::basic_string<C>& str)
        {
            operator=(str);
        }

        String<C>& operator=(const C* str)
        {
            size_t size = strlen(str);

            resize(size);

            if (!empty())
            {
                Memorycpy(m_string.data(), str, sizeof(C)*size);
            }

            return *this;
        }

        String<C>& operator=(const std::basic_string<C>& str)
        {
            size_t size = str.length();

            resize(size);

            if (!empty())
            {
                Memorycpy(m_string.data(), &(*str.begin()), sizeof(C)*size);
            }

            return *this;
        }

        inline operator const C*() const
        {
            return m_string.data();
        }

        bool operator==(const C* str)
        {
            return strcmp(m_string.data(), str)==0;
        }

        bool operator!=(const C* str)
        {
            return !operator==(str);
        }

        virtual C* data() const
        {
            return m_string.data();
        }

        virtual C* c_str() const
        {
            return m_string.data();
        }

        virtual bool empty() const
        {
            return m_string.size()==1;
        }

        virtual size_t length() const
        {
            return m_string.size()-1;
        }

        virtual void resize(size_t new_size)
        {
            m_string.resize(new_size+1);
            Memoryset(m_string.data(), 0, new_size+1);
        }
    };


    //
    // Allocator, a generic allocator for objects of class T
    //

    template<class T>
    class Allocator
    {
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef value_type& reference;
        typedef const value_type* const_pointer;
        typedef const value_type& const_reference;

        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        template<class O>
        struct rebind
        {
            // convert an allocator<T> to an allocator <O>
            typedef Allocator<O> other;
        };

        pointer address(reference val) const
        {
            // return address of mutable val
            return (&val);
        }

        const_pointer address(const_reference val) const
        {
            // return address of nonmutable val
            return (&val);
        }

        Allocator()
        {
            // construct default allocator (do nothing)
        }

        Allocator(const Allocator<T>&) 
        {
            // construct by copying (do nothing)
        }

        template<class O>
        Allocator(const Allocator<O>&)
        {
            // construct from a related allocator (do nothing)
        }

        template<class O>
        Allocator<T>& operator=(const Allocator<O>&)
        {
            // assign from a related allocator (do nothing)
            return (*this);
        }

        void deallocate(pointer ptr, size_type)
        {
            ::HeapFree(GetProcessHeap(), 0x00000004, (void*)ptr);
        }

        pointer allocate(size_type count)
        {	
            // allocate Array of count elements
            return pointer( ::HeapAlloc(GetProcessHeap(), 0x00000004, sizeof(T)*count) );
        }

        pointer allocate(size_type count, const void*)
        {
            // allocate Array of count elements, ignore hint
            return (allocate(count));
        }

        void construct(pointer ptr, const T& val)
        {
            // construct object at ptr with value val
            ::new(ptr) T(val); 
        }

        void destroy(pointer ptr)
        {
            // destroy object at ptr
            ptr->~T(); 
        }

        size_t max_size() const 
        {
            // estimate maximum Array size
            size_t count = (size_t)(-1) / sizeof (T);
            return (0 < count ? count : 1);
        }
    };

    template<class T, class O, typename containerID>
    inline bool operator==(const Allocator<T>&, const Allocator<O>&)
    {
        // test for allocator equality (always true)
        return (true);
    }

    template<class T, class O, typename containerID>
    inline bool operator!=(const Allocator<T>&, const Allocator<O>&)
    {
        // test for allocator inequality (always false)
        return (false);
    }
}