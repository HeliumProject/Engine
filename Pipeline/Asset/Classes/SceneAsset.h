#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetClass.h"


#include "Foundation/TUID.h"
#include "Foundation/Reflect/Registry.h"

namespace Helium
{
    namespace Asset
    {
        class PIPELINE_API SceneAsset : public AssetClass
        {
        public:
            REFLECT_DECLARE_OBJECT( SceneAsset, AssetClass );
            static void AcceptCompositeVisitor( Reflect::Composite& comp );

            virtual bool ValidateCompatible( const Component::ComponentPtr& attr, tstring& error ) const HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr< SceneAsset > SceneAssetPtr;
        typedef std::vector< SceneAsset > V_SceneAsset;
    }
}