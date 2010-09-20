#pragma once

#include "Core/API.h"

#include "Foundation/Reflect/Object.h"

#include "Core/SceneGraph/SceneNode.h"

namespace Helium
{
    namespace SceneGraph
    {
        class SceneNode;

        // nodes appear in the objects window in the UI by type
        struct NodeTypeChangeArgs
        {
            SceneGraph::SceneNode* m_Node;

            NodeTypeChangeArgs( SceneGraph::SceneNode* node )
                : m_Node (node)
            {

            }
        };
        typedef Helium::Signature< const NodeTypeChangeArgs& > NodeTypeChangeSignature;

        //
        // Node type that stores all the instances of any type.  All the
        // instances refer back to this node type for basic, shared information
        // among all the instances.
        // 

        class CORE_API SceneNodeType : public Reflect::Object
        {
        public:
            REFLECT_DECLARE_ABSTRACT( SceneNodeType, Reflect::Object );
            static void InitializeType();
            static void CleanupType();

        public:
            SceneNodeType(SceneGraph::Scene* scene, i32 instanceType);
            virtual ~SceneNodeType();

            SceneGraph::Scene* GetScene();

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

        protected:
            // the name of this type
            tstring m_Name;

            // The scene that owns us
            SceneGraph::Scene* m_Scene;

            // Index of image to use in the UI (from the 16x16 image list)
            i32 m_ImageIndex;

            // the instances that have this type
            HM_SceneNodeSmartPtr m_Instances;

            // compile time type id of the instances
            i32 m_InstanceType;
        };

        typedef Helium::SmartPtr< SceneGraph::SceneNodeType > SceneNodeTypePtr;

        typedef std::vector< SceneGraph::SceneNodeType* > V_SceneNodeTypeDumbPtr;
        typedef std::set< SceneGraph::SceneNodeType* > S_SceneNodeTypeDumbPtr;

        typedef stdext::hash_map< i32, S_SceneNodeTypeDumbPtr > HMS_TypeToSceneNodeTypeDumbPtr;
        typedef stdext::hash_map< tstring, SceneNodeTypePtr > HM_StrToSceneNodeTypeSmartPtr;
    }
}