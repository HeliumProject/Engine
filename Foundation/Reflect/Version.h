#pragma once

#include "Foundation/Reflect/Object.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API Version : public Object
        {
        public:
            REFLECT_DECLARE_OBJECT( Version, Object );
            static void AcceptCompositeVisitor( Reflect::Composite& comp );

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