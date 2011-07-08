#pragma once

#include <hash_map>

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetClass.h"

namespace Helium
{
    namespace Asset
    {
        class PIPELINE_API Entity : public AssetClass
        {
        private:

        public:
            REFLECT_DECLARE_OBJECT(Entity, AssetClass);

            static void PopulateComposite( Reflect::Composite& comp );

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