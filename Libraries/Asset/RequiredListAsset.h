#pragma once

#include "API.h"
#include "AssetClass.h"

#include "Finder/AssetSpecs.h"
#include "Foundation/File/Path.h"

#pragma TODO( "Uh, how is this different from a DependenciesAttribute?" )

namespace Asset
{
    class ASSET_API RequiredListAsset : public AssetClass
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