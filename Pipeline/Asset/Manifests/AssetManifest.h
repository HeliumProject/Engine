#pragma once

#include "Pipeline/API.h"

#include "Foundation/TUID.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

namespace Helium
{
    namespace Asset
    {
        class PIPELINE_API AssetManifest : public Reflect::Object
        {
        public:
            Vector3 m_BoundingBoxMin;
            Vector3 m_BoundingBoxMax;

            REFLECT_DECLARE_ABSTRACT(AssetManifest, Reflect::Object);

            static void PopulateComposite( Reflect::Composite& comp );
        };

        typedef Helium::SmartPtr<AssetManifest> AssetManifestPtr;
        typedef std::vector<AssetManifestPtr> V_AssetManifest;
    }
}