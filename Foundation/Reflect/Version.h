#pragma once

#include "Serializers.h"

namespace Reflect
{
    class FOUNDATION_API Version : public ConcreteInheritor<Version, Element>
    {
    public:
        tstring m_Source;
        tstring m_SourceVersion;

        static void EnumerateClass( Reflect::Compositor<Version>& comp );

        Version ();
        Version(const tchar* source, const tchar* sourceVersion);

        virtual bool IsCurrent();

        bool ConvertToInts( int* ints );

        void DetectVersion();
    };

    typedef Nocturnal::SmartPtr<Version> VersionPtr;
}