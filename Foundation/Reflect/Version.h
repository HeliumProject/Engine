#pragma once

#include "Serializers.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API Version : public Element
        {
        public:
            REFLECT_DECLARE_CLASS( Version, Element );
            static void EnumerateClass( Reflect::Compositor<Version>& comp );

            Version ();
            Version(const tchar* source, const tchar* sourceVersion);

            virtual bool IsCurrent();
            bool ConvertToInts( int* ints );
            void DetectVersion();

            tstring m_Source;
            tstring m_SourceVersion;
        };

        typedef Helium::SmartPtr<Version> VersionPtr;
    }
}