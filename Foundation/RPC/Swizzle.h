#pragma once

#include "Foundation/Memory/Endian.h"

namespace Helium
{
    namespace RPC
    {
        typedef void (*SwizzleFunc)(void* data);

        template <class T>
        inline void Swizzle(T* data)
        {
#ifdef WIN32
            HELIUM_BREAK();
#endif
        }

        template<>
        inline void Swizzle<uint8_t>(uint8_t* data)
        {
#ifdef WIN32
            Helium::Swizzle(*data, true);
#endif
        }
        template<> inline void Swizzle(int8_t* data)
        {
#ifdef WIN32
            Helium::Swizzle(*data, true);
#endif
        }

        template<> inline void Swizzle(uint16_t* data)
        {
#ifdef WIN32
            Helium::Swizzle(*data, true);
#endif
        }
        template<> inline void Swizzle(int16_t* data)
        {
#ifdef WIN32
            Helium::Swizzle(*data, true);
#endif
        }

        template<> inline void Swizzle(uint32_t* data)
        {
#ifdef WIN32
            Helium::Swizzle(*data, true);
#endif
        }
        template<> inline void Swizzle(int32_t* data)
        {
#ifdef WIN32
            Helium::Swizzle(*data, true);
#endif
        }

        template<> inline void Swizzle(uint64_t* data)
        {
#ifdef WIN32
            Helium::Swizzle(*data, true);
#endif
        }
        template<> inline void Swizzle(int64_t* data)
        {
#ifdef WIN32
            Helium::Swizzle(*data, true);
#endif
        }

        template<> inline void Swizzle(float32_t* data)
        {
#ifdef WIN32
            Helium::Swizzle(*data, true);
#endif
        }
        template<> inline void Swizzle(float64_t* data)
        {
#ifdef WIN32
            Helium::Swizzle(*data, true);
#endif
        }

        template <class T>
        inline void Swizzle(T& data)
        {
#ifdef WIN32
            Swizzle(&data);
#endif
        }

        template <class T>
        SwizzleFunc GetSwizzleFunc()
        {
            void (*func)(T*) = &Swizzle<T>;
            return (SwizzleFunc)(void*)func;
        }
    }
}