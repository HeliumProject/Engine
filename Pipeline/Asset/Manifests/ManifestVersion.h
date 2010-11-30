#pragma once

#include "Pipeline/API.h"

#include "Foundation/Reflect/Version.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

namespace Helium
{
    namespace Asset
    {
        //
        // Version information for scene classes
        //

        class PIPELINE_API ManifestVersion : public Reflect::Version
        {
        public:
            tstring m_ManifestVersion;
            static const tchar_t* MANIFEST_VERSION;

            REFLECT_DECLARE_CLASS(ManifestVersion, Reflect::Version);

            static void EnumerateClass( Reflect::Compositor<ManifestVersion>& comp );

            ManifestVersion();

            ManifestVersion(const tchar_t* source, const tchar_t* sourceVersion);

            virtual bool IsCurrent();
        };

        typedef Helium::SmartPtr<ManifestVersion> ManifestVersionPtr;
        typedef std::vector<ManifestVersionPtr> V_ManifestVersion;
    }
}