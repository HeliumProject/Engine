#pragma once

#include "Instance.h"
#include "Pipeline/Asset/Classes/Entity.h"

namespace Helium
{
    namespace SceneGraph
    {
        class EntitySet;
        class EntityInstance;

        struct EntityAssetChangeArgs
        {
            EntityInstance* m_Entity;
            Helium::Path    m_OldPath;
            Helium::Path    m_NewPath;

            EntityAssetChangeArgs( class EntityInstance* entity, const Helium::Path& oldPath, const Helium::Path& newPath )
                : m_Entity( entity )
                , m_OldPath( oldPath )
                , m_NewPath( newPath )
            {}
        };
        typedef Helium::Signature< const EntityAssetChangeArgs& > EntityAssetChangeSignature;

        class PIPELINE_API EntityInstance : public Instance
        {
        public:
            REFLECT_DECLARE_OBJECT( EntityInstance, Instance );
            static void PopulateComposite( Reflect::Composite& comp );
            static void InitializeType();
            static void CleanupType();

            EntityInstance();
            ~EntityInstance();

            virtual void Initialize() HELIUM_OVERRIDE;

            virtual bool ValidatePersistent( const Component::ComponentPtr& attr ) const HELIUM_OVERRIDE;
            virtual const Component::ComponentPtr& GetComponent( const Reflect::Class* type ) const HELIUM_OVERRIDE;
            virtual bool SetComponent( const Component::ComponentPtr& component, bool validate = true, tstring* error = NULL ) HELIUM_OVERRIDE;

            virtual tstring GenerateName() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;
            virtual SceneNodeTypePtr CreateNodeType( Scene* scene ) const HELIUM_OVERRIDE;
            void CheckSets();

            // retrieve the nested scene from the scene manager
            Scene* GetNestedScene();

            tstring GetEntityPath() const;
            void SetEntityPath( const tstring& path );
            Asset::EntityPtr GetEntity() const;

            bool IsPointerVisible() const;
            void SetPointerVisible(bool visible);

            bool IsBoundsVisible() const;
            void SetBoundsVisible(bool visible);

            bool IsGeometryVisible() const;
            void SetGeometryVisible(bool visible);

            // class set is the object common to all entities with common class within the same type
            class EntitySet* GetClassSet();
            const class EntitySet* GetClassSet() const;
            void SetClassSet( class EntitySet* artClass );

            // gather some manifest data
            virtual void PopulateManifest( Asset::SceneManifest* manifest ) const HELIUM_OVERRIDE;

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
            // Callbacks
            //
        protected:
            void OnComponentAdded( const Component::ComponentCollectionChanged& args );
            void OnComponentRemoved( const Component::ComponentCollectionChanged& args );

            //
            // Events
            //
        public:
            EntityAssetChangeSignature::Event e_ClassChanging;
            EntityAssetChangeSignature::Event e_ClassChanged;

        protected:
            Helium::Path        m_Path;
            bool                m_ShowPointer;
            bool                m_ShowBounds;
            bool                m_ShowGeometry;
            EntitySet*          m_ClassSet;
            mutable ScenePtr    m_Scene;
        };

        typedef Helium::StrongPtr<EntityInstance> EntityInstancePtr;
        typedef std::vector<EntityInstance*> V_EntityInstanceDumbPtr;
    }
}