#pragma once

#include "Instance.h"

#include "Core/Content/Nodes/ContentEntityInstance.h"

namespace Helium
{
    namespace Core
    {
        struct LightmapTweakArgs
        {
            class EntityInstance* m_Entity;
            u32      m_LightmapSetIndex;

            LightmapTweakArgs( class Core::EntityInstance* entity, u32 lightmapSetIndex )
                : m_Entity( entity )
                , m_LightmapSetIndex( lightmapSetIndex )
            {}
        };
        typedef Helium::Signature< const LightmapTweakArgs& > LightmapTweakSignature;

        struct CubemapTweakArgs
        {
            class EntityInstance* m_Entity;

            CubemapTweakArgs( class Core::EntityInstance* entity )
                : m_Entity( entity )
            {}
        };
        typedef Helium::Signature< const CubemapTweakArgs& > CubemapTweakSignature;

        struct EntityAssetChangeArgs
        {
            class Core::EntityInstance* m_Entity;
            Helium::Path m_OldPath;
            Helium::Path m_NewPath;

            EntityAssetChangeArgs( class Core::EntityInstance* entity, const Helium::Path& oldPath, const Helium::Path& newPath )
                : m_Entity( entity )
                , m_OldPath( oldPath )
                , m_NewPath( newPath )
            {}
        };
        typedef Helium::Signature< const EntityAssetChangeArgs& > EntityAssetChangeSignature;

        /////////////////////////////////////////////////////////////////////////////
        // Editor's wrapper for an entity instance.
        //
        class CORE_API EntityInstance : public Core::Instance
        {
            //
            // Members
            //

        protected:
            class EntitySet* m_ClassSet;
            mutable Core::ScenePtr m_Scene;

        public:
            SCENE_DECLARE_TYPE( Core::EntityInstance, Core::Instance );
            static void InitializeType();
            static void CleanupType();


            EntityInstance(Core::Scene* parent);
            EntityInstance(Core::Scene* parent, Content::EntityInstance* entity);
            virtual ~EntityInstance();

            void ConstructorInit();

            virtual tstring GenerateName() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;
            virtual SceneNodeTypePtr CreateNodeType( Core::Scene* scene ) const HELIUM_OVERRIDE;

            // retrieve the nested scene from the scene manager
            Core::Scene* GetNestedScene(GeometryMode mode, bool load_on_demand = true) const;


            //
            // Should we show the pointer
            //

            bool IsPointerVisible() const;
            void SetPointerVisible(bool visible);


            //
            // Should we show our bounds
            //

            bool IsBoundsVisible() const;
            void SetBoundsVisible(bool visible);


            //
            // Should we show geometry while we are drawing?
            //

            bool IsGeometryVisible() const;
            void SetGeometryVisible(bool visible);


            //
            // Sets help us organize groups of entities together
            //

            // class set is the object common to all entities with common class within the same type
            class Core::EntitySet* GetClassSet();
            const class Core::EntitySet* GetClassSet() const;
            void SetClassSet( class Core::EntitySet* artClass );

            // gather some manifest data
            virtual void PopulateManifest( Asset::SceneManifest* manifest ) const HELIUM_OVERRIDE;


            //
            // Evaluate and Render
            //

        public:
            virtual void Evaluate(GraphDirection direction) HELIUM_OVERRIDE;

            virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
            static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
            static void DrawBounds( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;

        private:
            static void CreatePanel( CreatePanelArgs& args );


            //
            // UI
            //

        public:
            tstring GetEntityAssetPath() const;
            void SetEntityAssetPath( const tstring& entityClass );

            //
            // Callbacks
            //
        protected:
            void OnComponentAdded( const Component::ComponentCollectionChanged& args );
            void OnComponentRemoved( const Component::ComponentCollectionChanged& args );

            //
            // Events
            //
        protected:
            EntityAssetChangeSignature::Event m_ClassChanging;
        public:
            void AddClassChangingListener( const EntityAssetChangeSignature::Delegate& listener )
            {
                m_ClassChanging.Add( listener );
            }

            void RemoveClassChangingListener( const EntityAssetChangeSignature::Delegate& listener )
            {
                m_ClassChanging.Remove( listener );
            }

        protected:
            EntityAssetChangeSignature::Event m_ClassChanged;
        public:
            void AddClassChangedListener( const EntityAssetChangeSignature::Delegate& listener )
            {
                m_ClassChanged.Add( listener );
            }

            void RemoveClassChangedListener( const EntityAssetChangeSignature::Delegate& listener )
            {
                m_ClassChanged.Remove( listener );
            }
        };

        typedef Helium::SmartPtr<Core::EntityInstance> EntityPtr;
        typedef std::vector<Core::EntityInstance*> V_EntityDumbPtr;
    }
}