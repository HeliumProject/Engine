#pragma once

#include "Pipeline/API.h"

#include <hash_map>

#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/Manifests/EntityManifest.h"

namespace Helium
{
    namespace Asset
    {
        class PIPELINE_API Entity : public AssetClass
        {
        private:

        public:
            REFLECT_DECLARE_OBJECT(Entity, AssetClass);

            static void AcceptCompositeVisitor( Reflect::Composite& comp );

        public:
            Entity()
            {
            }

            virtual void GatherSearchableProperties( Helium::SearchableProperties* properties ) const HELIUM_OVERRIDE;
        }; 

        typedef Helium::StrongPtr< Entity > EntityPtr;
        typedef std::vector< EntityPtr > V_Entity;

    }
}