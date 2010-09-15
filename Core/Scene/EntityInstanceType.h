#pragma once

#include "InstanceType.h"

namespace Helium
{
    namespace Core
    {
        class EntityType : public Core::InstanceType
        {
            //
            // Members
            //

        private:
            // materials
            D3DMATERIAL9 m_Material;


            //
            // Runtime Type Info
            //

        public:
            REFLECT_DECLARE_ABSTRACT( Core::EntityType, Core::InstanceType );
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

        public:
            EntityType( Core::Scene* scene, i32 instanceType );

            virtual ~EntityType();

            virtual void Reset() HELIUM_OVERRIDE;
            virtual void Create() HELIUM_OVERRIDE;
            virtual void Delete() HELIUM_OVERRIDE;

        public:
            virtual void PopulateManifest( Asset::SceneManifest* manifest ) const HELIUM_OVERRIDE;
        };
    }
}