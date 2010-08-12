#pragma once

#include "InstanceSet.h"

#include "Foundation/TUID.h"
#include "Core/Asset/Classes/Entity.h"
#include "Core/Asset/Manifests/EntityManifest.h"

namespace Helium
{
    namespace Core
    {
        class Entity;
        class EntityAssetSet;
        class EntityType;
        class PrimitiveCube;
        struct SharedFileChangeArgs;

        struct EntityAssetSetChangeArgs
        {
            Core::EntityAssetSet* m_EntityAssetSet;

            EntityAssetSetChangeArgs( Core::EntityAssetSet* entityClassSet )
                : m_EntityAssetSet( entityClassSet )
            {
            }
        };
        typedef Helium::Signature< void, const EntityAssetSetChangeArgs& > EntityAssetSetChangeSignature;

        class EntityAssetSet : public Core::InstanceSet
        {
            //
            // Members
            //

        protected:
            // file resolver id
            Helium::Path m_AssetPath;
            bool m_ClassMissing;

            // loaded class data, not shared via cache
            Asset::EntityPtr m_Class;

            // manifest information exported from the content
            Asset::EntityManifestPtr m_Manifest;

            // the master file from the content attribute
            tstring m_ArtFile;

            // shape to render with
            Core::Primitive* m_Shape;

            // the name of the class, derived from the path
            tstring m_Name;

            //
            // Runtime Type Info
            //

        public:
            SCENE_DECLARE_TYPE( Core::EntityAssetSet, Core::InstanceSet );
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

        public:
            EntityAssetSet( Core::EntityType* type, const Helium::Path& assetPath );

            virtual ~EntityAssetSet();

            Helium::Path GetEntityAssetPath() const
            {
                return m_AssetPath;
            }

            Asset::Entity* GetEntity() const
            {
                return m_Class;
            }

            const tstring& GetContentFile() const
            {
                return m_ArtFile;
            }

            Core::Primitive* GetShape()
            {
                return m_Shape;
            }

            const Core::Primitive* GetShape() const
            {
                return m_Shape;
            }

            virtual const tstring& GetName() const HELIUM_OVERRIDE
            {
                return m_Name;
            }

            virtual void Create();
            virtual void Delete();

            virtual void AddInstance(Core::Instance* i) HELIUM_OVERRIDE;
            virtual void RemoveInstance(Core::Instance* i) HELIUM_OVERRIDE;

            virtual void LoadAssetClass();

        private:
            EntityAssetSetChangeSignature::Event m_ClassLoaded;
        public:
            void AddClassLoadedListener( const EntityAssetSetChangeSignature::Delegate& listener )
            {
                m_ClassLoaded.Add( listener );
            }
            void RemoveClassLoadedListener( const EntityAssetSetChangeSignature::Delegate& listener )
            {
                m_ClassLoaded.Remove( listener );
            }
        };
    }
}