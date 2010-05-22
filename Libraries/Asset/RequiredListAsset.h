#pragma once

#include "API.h"
#include "AssetClass.h"

#include "Finder/AssetSpecs.h"
#include "File/File.h"

namespace Asset
{
    class ASSET_API RequiredListAsset : public AssetClass
    {
    public:

        File::S_Reference m_FileReferences;

        RequiredListAsset() {}
        virtual ~RequiredListAsset() {}

        REFLECT_DECLARE_CLASS( RequiredListAsset, AssetClass );

        static void EnumerateClass( Reflect::Compositor<RequiredListAsset>& comp );
    };

    typedef Nocturnal::SmartPtr< RequiredListAsset > RequiredListAssetPtr;
}