#pragma once

#include "Pipeline/API.h"

#include "Foundation/File/Path.h"

#include "AssetManifest.h"

namespace Helium
{
    namespace Asset
    {
        class PIPELINE_API EntityManifest : public AssetManifest
        {
        public:
            std::set< Helium::Path > m_Shaders;
            std::set< Helium::Path > m_BlendTextures;
            std::set< Helium::Path > m_LooseTextures;

            uint32_t m_TriangleCount;

            uint32_t m_MentalRayShaderCount;

            bool m_LightMapped;

            EntityManifest()
                : m_TriangleCount(0)
                , m_MentalRayShaderCount(0)
                , m_LightMapped(false)
            {

            }

            REFLECT_DECLARE_CLASS(EntityManifest, AssetManifest);

            static void EnumerateClass( Reflect::Compositor<EntityManifest>& comp );
        };

        typedef Helium::StrongPtr<EntityManifest> EntityManifestPtr;
        typedef std::vector<EntityManifestPtr> V_EntityManifest;
    }
}