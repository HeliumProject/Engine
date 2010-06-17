#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetClass.h"

#include "Finder/AssetSpecs.h"
#include "Foundation/File/Path.h"

#pragma TODO( "Uh, how is this different from a DependenciesAttribute?" )

namespace Asset
{
    class PIPELINE_API RequiredListAsset : public AssetClass
    {
    public:

        Nocturnal::S_Path m_Paths;

        RequiredListAsset() {}
        virtual ~RequiredListAsset() {}

        REFLECT_DECLARE_CLASS( RequiredListAsset, AssetClass );

        static void EnumerateClass( Reflect::Compositor<RequiredListAsset>& comp );
    };

    typedef Nocturnal::SmartPtr< RequiredListAsset > RequiredListAssetPtr;
}