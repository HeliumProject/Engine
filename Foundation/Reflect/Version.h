#pragma once

#include "Foundation/Reflect/Element.h"

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
            Version(const tchar_t* source, const tchar_t* sourceVersion);

            virtual bool IsCurrent();
            bool ConvertToInts( int* ints );
            void DetectVersion();

            tstring m_Source;
            tstring m_SourceVersion;
        };

        typedef Helium::SmartPtr<Version> VersionPtr;
    }
}