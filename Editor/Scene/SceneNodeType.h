#pragma once

#include "Editor/API.h"

#include "Object.h"

#include "SceneNode.h"

namespace Helium
{
    namespace Editor
    {
        class SceneNode;

        // nodes appear in the objects window in the UI by type
        struct NodeTypeChangeArgs
        {
            Editor::SceneNode* m_Node;

            NodeTypeChangeArgs( Editor::SceneNode* node )
                : m_Node (node)
            {

            }
        };
        typedef Helium::Signature< void, const NodeTypeChangeArgs& > NodeTypeChangeSignature;

        //
        // Node type that stores all the instances of any type.  All the
        // instances refer back to this node type for basic, shared information
        // among all the instances.
        // 

        class LUNA_SCENE_API SceneNodeType : public Object
        {
            //
            // Members
            //

        protected:
            // the name of this type
            tstring m_Name;

            // The scene that owns us
            Editor::Scene* m_Scene;

            // Index of image to use in the UI (from the 16x16 image list)
            i32 m_ImageIndex;

            // the instances that have this type
            HM_SceneNodeSmartPtr m_Instances;

            // compile time type id of the instances
            i32 m_InstanceType;


            //
            // Runtime Type Info
            //

        public:
            EDITOR_DECLARE_TYPE( Editor::SceneNodeType, Object );
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

        public:
            SceneNodeType(Editor::Scene* scene, i32 instanceType);
            virtual ~SceneNodeType();

            Editor::Scene* GetScene();

            const tstring& GetName() const;
            void SetName( const tstring& name );

            i32 GetImageIndex() const;
            void SetImageIndex( i32 index );

            virtual void Reset();

            virtual void AddInstance(SceneNodePtr n);
            virtual void RemoveInstance(SceneNodePtr n);
            const HM_SceneNodeSmartPtr& GetInstances() const;

            i32 GetInstanceType() const;

            virtual void PopulateManifest( Asset::SceneManifest* manifest ) const;

        private:
            NodeTypeChangeSignature::Event m_NodeAdded;
        public:
            void AddNodeAddedListener( const NodeTypeChangeSignature::Delegate& listener )
            {
                m_NodeAdded.Add( listener );
            }
            void RemoveNodeAddedListener( const NodeTypeChangeSignature::Delegate& listener )
            {
                m_NodeAdded.Remove( listener );
            }

        private:
            NodeTypeChangeSignature::Event m_NodeRemoved;
        public:
            void AddNodeRemovedListener( const NodeTypeChangeSignature::Delegate& listener )
            {
                m_NodeRemoved.Add( listener );
            }
            void RemoveNodeRemovedListener( const NodeTypeChangeSignature::Delegate& listener )
            {
                m_NodeRemoved.Remove( listener );
            }
        };

        typedef Helium::SmartPtr< Editor::SceneNodeType > SceneNodeTypePtr;

        typedef std::vector< Editor::SceneNodeType* > V_SceneNodeTypeDumbPtr;

        typedef std::set< Editor::SceneNodeType* > S_SceneNodeTypeDumbPtr;

        typedef stdext::hash_map< i32, S_SceneNodeTypeDumbPtr > HMS_TypeToSceneNodeTypeDumbPtr;

        typedef stdext::hash_map< tstring, SceneNodeTypePtr > HM_StrToSceneNodeTypeSmartPtr;
    }
}