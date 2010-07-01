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
#include "Pipeline/Content/Animation/AnimationClip.h"
#include "Pipeline/Content/Animation/JointOrdering.h"
#include "Pipeline/Content/Animation/MorphTargetData.h"

#include "Foundation/TUID.h"
#include "Foundation/Math/Vector4.h"
#include "Foundation/Math/CalculateBounds.h"
#include "Foundation/Automation/Event.h"

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
    class ContentVisitor; 
    class SceneNode;

    typedef stdext::hash_map< Nocturnal::TUID, SceneNodePtr > M_DependencyNode;
    typedef stdext::hash_multimap< Nocturnal::TUID, HierarchyNodePtr, Nocturnal::TUIDHasher > MM_HierarchyNode;

    struct NodeAddedArgs
    {
        SceneNode* m_AddedNode;
        NodeAddedArgs( SceneNode& addedNode)
            : m_AddedNode ( &addedNode )
        {

        }
    };

    typedef Nocturnal::Signature<void, NodeAddedArgs&> NodeAddedSignature;

    struct TUIDBSphere
    {
        Nocturnal::TUID                         m_JointID;
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
    class PIPELINE_API Scene : public Nocturnal::RefCountBase<Scene>
    {
    private:
        //internal utility members
        V_HierarchyNode m_AddedHierarchyNodes;

        std::map< Nocturnal::TUID, std::vector< Math::BoundingVolumeGenerator::BSphere > >       m_JointBspheres;    // joint uid <-> bsphere map
        std::vector< Math::BoundingVolumeGenerator::BSphere >                                    m_Bspheres;         // bspheres surrounding each bangle for each content type

    public:

        // allow anyone to add a delegate for this event
        //
        NodeAddedSignature::Event m_NodeAddedSignature;

        struct SkinVertex
        {
            Math::Vector3               m_Position;
            Nocturnal::V_TUID           m_JointIDs;  // joint uids
            std::vector< u32 >          m_Joints;     // joint uids as runtime indices
            std::vector< f32 >          m_Weights;
        };

        // enable reference counting
        tstring          m_FilePath;

        // stores hierarchy info for the scene
        MM_HierarchyNode     m_Hierarchy;

        // all the nodes in the scene
        M_DependencyNode     m_DependencyNodes;

        Nocturnal::S_TUID     m_JointIds;
        // shortcuts to each node by type

        // should probably have made this a map keyed by reflect type.
        V_PivotTransform    m_Transforms;
        V_JointTransform    m_Joints;
        V_Mesh              m_Meshes;
        V_Shader            m_Shaders;
        V_Skin              m_Skins;
        V_AnimationClip     m_AnimationClips;

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
        void GetAll( std::vector< Nocturnal::SmartPtr<T> >& objects, i32 attributeType = -1, bool clear = false ) const;

        //
        // Animation API
        //

        u32  GetNumAnimationClips() const;
        void GetJointsFromClip( const AnimationClipPtr& clip, V_JointTransform& joints );
        u32  GetNumValidJointAnimations( u32 clipIndex = 0 ) const;
        u32  GetNumValidJointAnimations( const Nocturnal::S_TUID& jointList, u32 clipIndex = 0 ) const;
        void GetJointMismatchReport( std::vector< tstring > &mismatchMessages, u32 clipIndex = 0 ) const;

        void CalculateJointBoundingVolumes();
        bool GetBSpheresForJoint          (std::vector< Math::BoundingVolumeGenerator::BSphere >& bspheres, const Nocturnal::TUID& jointID ) const;
        bool GetBSpheres                  (std::vector< Math::BoundingVolumeGenerator::BSphere >& bspheres ) const;
        bool GetSkinVerts                 (const std::map<Nocturnal::TUID, u32>& jointUidToId, const Nocturnal::TUID& rootUid, std::vector< std::vector<SkinVertex> >& skinVerts) const;

        void GetInfluentialJoints( S_JointTransform& joints );
        u32  GetHierarchyNodeDepth( const HierarchyNodePtr& node ) const;

        void CalculateJointOrdering();
        const JointOrderingPtr& GetJointOrdering() const;

        //
        // Lookup an object by GUID and be type safe
        //

        template< class T >
        Nocturnal::SmartPtr< T > Get( const Nocturnal::TUID &uid ) const
        {
            M_DependencyNode::const_iterator itor = m_DependencyNodes.find( uid );
            if( itor != m_DependencyNodes.end() )
            {
                return Reflect::ObjectCast< T >( itor->second );
            }
            return NULL;
        }

        //
        // Get the parent of a node, and be type save
        //

        template <class T>
        Nocturnal::SmartPtr<T> GetParent(const HierarchyNodePtr& node) const
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
        void GetDescendants( V_HierarchyNode& descendants, const HierarchyNodePtr& node ) const;

        bool IsChildOf( const HierarchyNodePtr& potentialChild, const HierarchyNodePtr &potentialParent ) const;

        void AddChild( const HierarchyNodePtr& child, const Nocturnal::TUID& parentID );
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
        bool Exists( const Nocturnal::TUID& id );

        void UpdateGlobalTransforms( const TransformPtr& transform );
        void UpdateHierarchy();

    public:
        void Optimize();

    private:
        void Optimize(const HierarchyNodePtr& object);
        void CollateMorphTargets();
    };

    typedef Nocturnal::SmartPtr< Scene > ScenePtr;

    template< class T >
    void Scene::GetAll( std::vector< Nocturnal::SmartPtr<T> >& objects, i32 attributeType, bool clear) const
    {
        // clear
        if (clear)
        {
            objects.clear();
        }

        // for now do a complete iteration, if we end up nesting dudes
        //  in other classes, we can use reflect's C++ member intospection to find them
        M_DependencyNode::const_iterator itr = m_DependencyNodes.begin();
        M_DependencyNode::const_iterator end = m_DependencyNodes.end();
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
};
