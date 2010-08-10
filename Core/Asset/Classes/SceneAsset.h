#pragma once

#include "Core/API.h"
#include "Core/Asset/AssetClass.h"


#include "Foundation/TUID.h"
#include "Foundation/Reflect/Registry.h"

namespace Helium
{
    namespace Asset
    {
        class CORE_API SceneAsset : public AssetClass
        {
        public:
            float       m_NearClipDist;
            float       m_FarClipDist;
            float       m_SpatialGridSize;
            bool        m_OcclTestDownwardColl;
            bool        m_OcclTestDownwardVis;
            float       m_OcclVisDistAdjust;
            float       m_DecalGeomMem;

            std::set< Helium::Path > m_Zones;

            SceneAsset()
                : m_NearClipDist( 0.06f )
                , m_FarClipDist( 1800.0f )
                , m_SpatialGridSize( 2.0f )
                , m_OcclTestDownwardColl( true )
                , m_OcclTestDownwardVis( true )
                , m_OcclVisDistAdjust( 0.0f )
                , m_DecalGeomMem( 1024.0f )
            {
            }

            virtual bool ProcessComponent(Reflect::ElementPtr element, const tstring& fieldName) HELIUM_OVERRIDE;

            virtual bool ValidateCompatible( const Component::ComponentPtr& attr, tstring& error ) const HELIUM_OVERRIDE;

            REFLECT_DECLARE_CLASS( SceneAsset, AssetClass );

            static void EnumerateClass( Reflect::Compositor<SceneAsset>& comp );
        };

        typedef Helium::SmartPtr< SceneAsset > SceneAssetPtr;
        typedef std::vector< SceneAsset > V_SceneAsset;
    }
}