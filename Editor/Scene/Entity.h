#pragma once

#include "Instance.h"

#include "Pipeline/Asset/Classes/EntityInstance.h"

namespace Helium
{
    namespace Editor
    {
        struct LightmapTweakArgs
        {
            class Entity* m_Entity;
            u32      m_LightmapSetIndex;

            LightmapTweakArgs( class Editor::Entity* entity, u32 lightmapSetIndex )
                : m_Entity( entity )
                , m_LightmapSetIndex( lightmapSetIndex )
            {}
        };
        typedef Helium::Signature< void, const LightmapTweakArgs& > LightmapTweakSignature;

        struct CubemapTweakArgs
        {
            class Entity* m_Entity;

            CubemapTweakArgs( class Editor::Entity* entity )
                : m_Entity( entity )
            {}
        };
        typedef Helium::Signature< void, const CubemapTweakArgs& > CubemapTweakSignature;

        struct EntityAssetChangeArgs
        {
            class Editor::Entity* m_Entity;
            Helium::Path m_OldPath;
            Helium::Path m_NewPath;

            EntityAssetChangeArgs( class Editor::Entity* entity, const Helium::Path& oldPath, const Helium::Path& newPath )
                : m_Entity( entity )
                , m_OldPath( oldPath )
                , m_NewPath( newPath )
            {}
        };
        typedef Helium::Signature< void, const EntityAssetChangeArgs& > EntityAssetChangeSignature;

        /////////////////////////////////////////////////////////////////////////////
        // Editor's wrapper for an entity instance.
        //
        class LUNA_SCENE_API Entity : public Editor::Instance
        {
            //
            // Members
            //

        protected:
            class EntityAssetSet* m_ClassSet;
            mutable Editor::Scene* m_NestedSceneArt;
            mutable Editor::Scene* m_NestedSceneCollision;
            mutable Editor::Scene* m_NestedScenePathfinding;


            //
            // Runtime Type Info
            //

        public:
            EDITOR_DECLARE_TYPE( Editor::Entity, Editor::Instance );
            static void InitializeType();
            static void CleanupType();


            //
            // Member functions
            //

            Entity(Editor::Scene* s);
            Entity(Editor::Scene* s, Asset::EntityInstance* entity);
            virtual ~Entity();

            void ConstructorInit();

            virtual tstring GenerateName() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;
            virtual SceneNodeTypePtr CreateNodeType( Editor::Scene* scene ) const HELIUM_OVERRIDE;

            // retrieve the nested scene from the scene manager
            Editor::Scene* GetNestedScene(GeometryMode mode, bool load_on_demand = true) const;


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
            class Editor::EntityAssetSet* GetClassSet();
            const class Editor::EntityAssetSet* GetClassSet() const;
            void SetClassSet( class Editor::EntityAssetSet* artClass );

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

        typedef Helium::SmartPtr<Editor::Entity> EntityPtr;
        typedef std::vector<Editor::Entity*> V_EntityDumbPtr;
    }
}