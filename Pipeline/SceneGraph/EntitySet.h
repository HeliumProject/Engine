#pragma once

#include "InstanceSet.h"

#include "Foundation/TUID.h"
#include "Pipeline/Asset/Classes/Entity.h"
#include "Pipeline/Asset/Manifests/EntityManifest.h"

namespace Helium
{
    namespace SceneGraph
    {
        class EntityInstance;
        class EntitySet;
        class EntityInstanceType;
        class PrimitiveCube;
        struct SharedFileChangeArgs;

        struct EntitySetChangeArgs
        {
            SceneGraph::EntitySet* m_EntitySet;

            EntitySetChangeArgs( SceneGraph::EntitySet* entityClassSet )
                : m_EntitySet( entityClassSet )
            {
            }
        };
        typedef Helium::Signature< const EntitySetChangeArgs& > EntitySetChangeSignature;

        class EntitySet : public SceneGraph::InstanceSet
        {
        protected:
            Helium::Path m_AssetPath;
            bool m_ClassMissing;

            // loaded class data, not shared via cache
            Asset::EntityPtr m_Class;

            // manifest information exported from the content
            Asset::EntityManifestPtr m_Manifest;

            // shape to render with
            SceneGraph::Primitive* m_Shape;

            // the name of the class, derived from the path
            tstring m_Name;

        public:
            REFLECT_DECLARE_ABSTRACT( SceneGraph::EntitySet, SceneGraph::InstanceSet );
            static void InitializeType();
            static void CleanupType();

        public:
            EntitySet( SceneGraph::EntityInstanceType* type, const Helium::Path& assetPath );

            virtual ~EntitySet();

            Helium::Path GetEntityAssetPath() const
            {
                return m_AssetPath;
            }

            Asset::Entity* GetEntity() const
            {
                return m_Class;
            }

            SceneGraph::Primitive* GetShape()
            {
                return m_Shape;
            }

            const SceneGraph::Primitive* GetShape() const
            {
                return m_Shape;
            }

            virtual const tstring& GetName() const HELIUM_OVERRIDE
            {
                return m_Name;
            }

            virtual void Create();
            virtual void Delete();

            virtual void AddInstance(SceneGraph::Instance* i) HELIUM_OVERRIDE;
            virtual void RemoveInstance(SceneGraph::Instance* i) HELIUM_OVERRIDE;

            virtual void LoadAssetClass();

        private:
            EntitySetChangeSignature::Event m_ClassLoaded;
        public:
            void AddClassLoadedListener( const EntitySetChangeSignature::Delegate& listener )
            {
                m_ClassLoaded.Add( listener );
            }
            void RemoveClassLoadedListener( const EntitySetChangeSignature::Delegate& listener )
            {
                m_ClassLoaded.Remove( listener );
            }
        };
    }
}