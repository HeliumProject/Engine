#pragma once

#include "Pipeline/API.h"

#include "Foundation/TUID.h"
#include "Foundation/TUID.h"

#include "Pipeline/Content/Nodes/Mesh.h"
#include "Pipeline/Content/Nodes/Shader.h"
#include "Pipeline/Content/Nodes/Skin.h"
#include "Pipeline/Content/Nodes/PivotTransform.h"
#include "Pipeline/Content/Nodes/JointTransform.h"
#include "Pipeline/Content/Nodes/Light.h"
#include "Pipeline/Content/Animation/Animation.h"
#include "Pipeline/Content/Animation/JointOrdering.h"
#include "Pipeline/Content/Animation/MorphTargetData.h"

#include "Foundation/TUID.h"
#include "Foundation/Math/Vector4.h"
#include "Foundation/Math/CalculateBounds.h"
#include "Foundation/Automation/Event.h"

namespace Helium
{
    namespace Math
    {
        class Line;
    };

    namespace Asset
    {
        class Entity;
    };

    namespace Content
    {  
        class SceneNode;

        struct NodeAddedArgs
        {
            SceneNode* m_AddedNode;
            NodeAddedArgs( SceneNode& addedNode)
                : m_AddedNode ( &addedNode )
            {

            }
        };

        typedef Helium::Signature<void, NodeAddedArgs&> NodeAddedSignature;

        struct TUIDBSphere
        {
            Helium::TUID                         m_JointID;
            Math::BoundingVolumeGenerator::BSphere  m_BSphere;
        };
        typedef std::vector<TUIDBSphere> V_TUIDBSphere;

        class TUIDBSphereList
        {
        public:
            void Add(const TUIDBSphere& t)
            {
                size_t i;
                for(i = 0; i < m_list.size(); i++)
                {
                    // joint-ids dont match - go to next one
                    if (m_list[i].m_JointID != t.m_JointID)
                    {
                        continue;
                    }

                    const Math::BoundingVolumeGenerator::BSphere& ba = m_list[i].m_BSphere;
                    const Math::BoundingVolumeGenerator::BSphere& bb = t.m_BSphere;

                    Math::Vector4 sa(ba.m_Center.x, ba.m_Center.y, ba.m_Center.z, ba.m_Radius);
                    Math::Vector4 sb(bb.m_Center.x, bb.m_Center.y, bb.m_Center.z, bb.m_Radius);
                    f32 dsq = (sa - sb).LengthSquared();

                    // ids matched and these bspheres are very close - we have this one - skip
                    if (dsq < SQR(0.01f))
                    {
                        break;
                    }
                }

                if (i == m_list.size())
                {
                    m_list.push_back(t);
                }
            }

            V_TUIDBSphere m_list;
        };

        /////////////////////////////////////////////////////////////////////////////
        // Scene is a simple database of objects that can read and write from a flat file
        //  It contains a master list of dependency nodes (the most basic type of object in a scene)
        //  as well as per-type shortcuts for optimal iteration by APIs that only need access to specific data
        //  (eg. joints, or meshes, or other data).
        //
        class PIPELINE_API Scene : public Helium::RefCountBase<Scene>
        {
        private:
            V_HierarchyNode m_AddedHierarchyNodes;

            std::map< Helium::TUID, std::vector< Math::BoundingVolumeGenerator::BSphere > >       m_JointBspheres;    // joint uid <-> bsphere map
            std::vector< Math::BoundingVolumeGenerator::BSphere >                                    m_Bspheres;         // bspheres surrounding each bangle for each content type

        public:

            // allow anyone to add a delegate for this event
            //
            NodeAddedSignature::Event m_NodeAddedSignature;

            struct SkinVertex
            {
                Math::Vector3               m_Position;
                Helium::V_TUID           m_JointIDs;  // joint uids
                std::vector< u32 >          m_Joints;     // joint uids as runtime indices
                std::vector< f32 >          m_Weights;
            };

            // enable reference counting
            tstring          m_FilePath;

            // stores hierarchy info for the scene
            std::map< Helium::TUID, Helium::OrderedSet< Helium::TUID > > m_Hierarchy;

            // all the nodes in the scene
            std::map< Helium::TUID, SceneNodePtr > m_DependencyNodes;

            Helium::S_TUID    m_JointIds;

            // shortcuts to each node by type
            // should probably have made this a map keyed by reflect type.
            V_PivotTransform    m_Transforms;
            V_JointTransform    m_Joints;
            V_Mesh              m_Meshes;
            V_Shader            m_Shaders;
            V_Skin              m_Skins;
            V_Animation         m_Animations;

            MorphTargetDataPtr  m_MorphTargetData;

            Scene();
            Scene( tuid fileID );
            Scene( const tstring &filePath );

            //
            //  Resets the Scene
            //
            void Reset();

            //
            // Load a content file into the Scene
            //
            void Load( const tstring& filePath );
            void Load( const tstring &filePath, Reflect::V_Element& elements, Reflect::StatusHandler* status = NULL );
            void LoadXML( const tstring& xml, Reflect::V_Element& elements, Reflect::StatusHandler* status = NULL );

        private:
            void PostLoad( Reflect::V_Element& elements );

            //
            // Serialize the Scene into a file
            //
        public:
            void Serialize();
            void Serialize( const tstring& filePath );

            //
            // Resolve shader object for a given tri in a given mesh
            //
            ShaderPtr GetShader( const MeshPtr &mesh, u32 triIndex );

            //
            // Get all the objects of type T in the scene
            //
            template< class T >
            void GetAll( std::vector< Helium::SmartPtr<T> >& objects, i32 attributeType = -1, bool clear = false ) const;

            //
            // Animation API
            //

            u32  GetNumAnimations() const;
            void GetJointsFromAnimation( const AnimationPtr& clip, V_JointTransform& joints );
            u32  GetNumValidJointAnimations( u32 clipIndex = 0 ) const;
            u32  GetNumValidJointAnimations( const Helium::S_TUID& jointList, u32 clipIndex = 0 ) const;
            void GetJointMismatchReport( std::vector< tstring > &mismatchMessages, u32 clipIndex = 0 ) const;

            void CalculateJointBoundingVolumes();
            bool GetBSpheresForJoint          (std::vector< Math::BoundingVolumeGenerator::BSphere >& bspheres, const Helium::TUID& jointID ) const;
            bool GetBSpheres                  (std::vector< Math::BoundingVolumeGenerator::BSphere >& bspheres ) const;
            bool GetSkinVerts                 (const std::map<Helium::TUID, u32>& jointUidToId, const Helium::TUID& rootUid, std::vector< std::vector<SkinVertex> >& skinVerts) const;

            void GetInfluentialJoints( S_JointTransform& joints );
            u32  GetHierarchyNodeDepth( const HierarchyNodePtr& node ) const;

            void CalculateJointOrdering();
            const JointOrderingPtr& GetJointOrdering() const;

            //
            // Lookup an object by GUID and be type safe
            //

            template< class T >
            Helium::SmartPtr< T > Get( const Helium::TUID &uid ) const
            {
                std::map< Helium::TUID, SceneNodePtr >::const_iterator itr = m_DependencyNodes.find( uid );
                if( itr != m_DependencyNodes.end() )
                {
                    return Reflect::ObjectCast< T >( itr->second );
                }
                return NULL;
            }

            //
            // Get the parent of a node, and be type save
            //

            template <class T>
            Helium::SmartPtr<T> GetParent(const HierarchyNodePtr& node) const
            {
                return Get<T>(node->m_ParentID); 
            }


            //
            // Use to manually add dependency nodes to the Scene
            //
            void Add( const SceneNodePtr &node );
            void Remove( const SceneNodePtr &node );


            //
            // Need to call after you are done manually Adding dependency nodes to the Scene
            //
            void Update();

            //
            // Scene Hierarchy API
            //
            void GetChildren( V_HierarchyNode& children, const HierarchyNodePtr& node ) const;
            bool IsChildOf( const HierarchyNodePtr& potentialChild, const HierarchyNodePtr &potentialParent ) const;

            void AddChild( const HierarchyNodePtr& child, const Helium::TUID& parentID );
            void AddChild( const HierarchyNodePtr& child, const HierarchyNodePtr& parent );

            void RemoveFromHierarchy( const HierarchyNodePtr& child );
            void RemoveFromParent( const HierarchyNodePtr& node );

            void UpdateGlobalTransforms( );

            void GetAlignedBoundingBox( Math::AlignedBox& box ) const;

            void MergeMeshes();

            bool IntersectSegment( const Math::Line& segment,  Math::Matrix4* transform = NULL );
            bool IntersectRay( const Math::Line& ray,  Math::Matrix4* transform = NULL );

        private:
            // use GetJointOrdering
            JointOrderingPtr    m_JointOrdering;

            //
            // Internal utilities 
            //
            bool Exists( const Helium::TUID& id );

            void UpdateGlobalTransforms( const TransformPtr& transform );
            void UpdateHierarchy();

        public:
            void Optimize();

        private:
            void Optimize(const HierarchyNodePtr& object);
            void CollateMorphTargets();
        };

        typedef Helium::SmartPtr< Scene > ScenePtr;

        template< class T >
        void Scene::GetAll( std::vector< Helium::SmartPtr<T> >& objects, i32 attributeType, bool clear) const
        {
            // clear
            if (clear)
            {
                objects.clear();
            }

            // for now do a complete iteration, if we end up nesting dudes
            //  in other classes, we can use reflect's C++ member intospection to find them
            std::map< Helium::TUID, SceneNodePtr >::const_iterator itr = m_DependencyNodes.begin();
            std::map< Helium::TUID, SceneNodePtr >::const_iterator end = m_DependencyNodes.end();
            for ( ; itr != end; ++itr )
            {
                T* node = Reflect::ObjectCast<T>( itr->second );

                if (node == NULL)
                {
                    continue;
                }
                if( attributeType != -1 )
                {
                    if( node->GetComponent( attributeType ).ReferencesObject() )
                        objects.push_back(node);
                }
                else
                {
                    objects.push_back(node);
                }
            }
        }

        struct HierarchySort
        {
            HierarchySort( const Scene& scene )
                : m_Scene( scene )
            {}

            const Scene& m_Scene;

            bool operator()( const JointTransformPtr& lhs, const JointTransformPtr& rhs )
            {
                u32 lhsDepth = m_Scene.GetHierarchyNodeDepth( lhs );
                u32 rhsDepth = m_Scene.GetHierarchyNodeDepth( rhs );

                if ( lhsDepth != rhsDepth )
                    return lhsDepth < rhsDepth;

                if ( lhs->GetName() != rhs->GetName() )
                    return lhs->GetName().compare( rhs->GetName() ) < 0;

                // all else fails, use the guid
                return lhs->m_ID < rhs->m_ID;
            }
        };
    }
}