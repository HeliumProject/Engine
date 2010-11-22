#pragma once

#include "API.h"
#include "Types.h"
#include "Assert.h"

namespace Helium
{
    namespace Platform
    {
        namespace Types
        {
            enum Type
            {
                Windows,
                PlayStation3,
                Count,
            };

            static const tchar_t* Strings[] = 
            {
                TXT("Windows"),
                TXT("PlayStation3"),
            };

            HELIUM_COMPILE_ASSERT( Platform::Types::Count == sizeof(Strings) / sizeof(const tchar_t*) );
        }
        typedef Types::Type Type;

        inline const tchar_t* GetTypeName(Type t)
        {
            if (t >= 0 && t<Types::Count)
            {
                return Types::Strings[t];
            }
            else
            {
                return Types::Strings[0];
            }
        }

        PLATFORM_API Type GetType();
    }

    PLATFORM_API void EnableCPPErrorHandling( bool enable );

    PLATFORM_API void Print(const tchar_t* fmt, ...);
    PLATFORM_API void Sleep(int millis);
}
