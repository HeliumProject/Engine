#pragma once

#include "Pipeline/API.h"
#include "Foundation/Reflect/Version.h"
#include "Foundation/Reflect/Serializers.h"

namespace Helium
{
    namespace Content
    {
        //
        // Version information for scene classes
        //

        class PIPELINE_API ContentVersion : public Reflect::Version
        {
        public:
            tstring m_ContentVersion;
            static const tchar* CONTENT_VERSION;

            REFLECT_DECLARE_CLASS(ContentVersion, Reflect::Version)

                static void EnumerateClass( Reflect::Compositor<ContentVersion>& comp );

            ContentVersion();

            ContentVersion(const tchar* source, const tchar* sourceVersion);

            virtual bool IsCurrent();
        };

        typedef Helium::SmartPtr<ContentVersion> ContentVersionPtr;
        typedef std::vector<ContentVersionPtr> V_ContentVersion;
    }
}