#pragma once

#include "Serializers.h"

namespace Reflect
{
    class FOUNDATION_API Version : public ConcreteInheritor<Version, Element>
    {
    public:
        std::string m_Source;
        std::string m_SourceVersion;

        static void EnumerateClass( Reflect::Compositor<Version>& comp );

        Version ();
        Version(const char* source, const char* sourceVersion);

        virtual bool IsCurrent();

        bool ConvertToInts( int* ints );

        void DetectVersion();
    };

    typedef Nocturnal::SmartPtr<Version> VersionPtr;
}