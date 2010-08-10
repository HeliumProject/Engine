#pragma once

#include "InstanceSet.h"

#include "Foundation/TUID.h"
#include "Core/Asset/Classes/Entity.h"
#include "Core/Asset/Manifests/EntityManifest.h"

namespace Helium
{
    namespace Editor
    {
        class Entity;
        class EntityAssetSet;
        class EntityType;
        class PrimitiveCube;
        struct SharedFileChangeArgs;

        struct EntityAssetSetChangeArgs
        {
            Editor::EntityAssetSet* m_EntityAssetSet;

            EntityAssetSetChangeArgs( Editor::EntityAssetSet* entityClassSet )
                : m_EntityAssetSet( entityClassSet )
            {
            }
        };
        typedef Helium::Signature< void, const EntityAssetSetChangeArgs& > EntityAssetSetChangeSignature;

        class EntityAssetSet : public Editor::InstanceSet
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
            Editor::Primitive* m_Shape;

            // the name of the class, derived from the path
            tstring m_Name;

            //
            // Runtime Type Info
            //

        public:
            EDITOR_DECLARE_TYPE( Editor::EntityAssetSet, Editor::InstanceSet );
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

        public:
            EntityAssetSet( Editor::EntityType* type, const Helium::Path& assetPath );

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

            Editor::Primitive* GetShape()
            {
                return m_Shape;
            }

            const Editor::Primitive* GetShape() const
            {
                return m_Shape;
            }

            virtual const tstring& GetName() const HELIUM_OVERRIDE
            {
                return m_Name;
            }

            virtual void Create();
            virtual void Delete();

            virtual void AddInstance(Editor::Instance* i) HELIUM_OVERRIDE;
            virtual void RemoveInstance(Editor::Instance* i) HELIUM_OVERRIDE;

            virtual void LoadAssetClass();

        private:
            void FileChanged( const SharedFileChangeArgs& args );

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