#pragma once

#include "Pipeline/API.h"

#include "Foundation/TUID.h"
#include "Foundation/TUID.h"

#include "Pipeline/Content/Nodes/Geometry/Mesh.h"
#include "Pipeline/Content/Nodes/Geometry/Shader.h"
#include "Pipeline/Content/Nodes/Geometry/Skin.h"

#include "Pipeline/Content/Nodes/Transform/PivotTransform.h"
#include "Pipeline/Content/Nodes/Transform/JointTransform.h"
#include "Pipeline/Content/Nodes/Transform/Descriptor.h"
#include "Pipeline/Content/Animation/AnimationClip.h"
#include "Pipeline/Content/Animation/JointOrdering.h"
#include "Pipeline/Content/Animation/MorphTargetData.h"
#include "Pipeline/Content/Nodes/Lights/Light.h"
#include "Pipeline/Content/Misc.h"
#include "Pipeline/Content/Nodes/Geometry/CollisionPrimitive.h"

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
  typedef std::pair< u32, u32 > ExportTypeAndIndexPair;
  typedef std::multimap< ExportTypeAndIndexPair, MeshPtr > MM_TypedMesh;

  struct NodeAddedArgs
  {
    SceneNode* m_AddedNode;
    NodeAddedArgs( SceneNode& addedNode)
      : m_AddedNode ( &addedNode )
    {

    }
  };

  typedef Nocturnal::Signature<void, NodeAddedArgs&> NodeAddedSignature;

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
    V_Descriptor m_AddedDescriptors;
    V_HierarchyNode m_AddedHierarchyNodes;

    M_UIDBSphere              m_JointBspheres[Content::ContentTypes::NumContentTypes];    // joint uid <-> bsphere map
    std::vector<t_BsphereId>  m_Bspheres[Content::ContentTypes::NumContentTypes];         // bspheres surrounding each bangle for each content type

  public:

    // allow anyone to add a delegate for this event
    //
    NodeAddedSignature::Event m_NodeAddedSignature;

    struct t_SkinVert
    {
      Math::Vector3  m_pos;
      Nocturnal::V_TUID m_joint_ids;  // joint uids
      V_u32          m_joints;     // joint uids as runtime indices
      V_f32          m_weights;
    };

    struct t_SkinVerts
    {
      u32                     m_mesh_id;
      std::vector<t_SkinVert> m_verts;
    };


    // enable reference counting
    std::string          m_FilePath;

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
    V_Descriptor        m_ExportNodes;
    V_AnimationClip     m_AnimationClips;

    mutable MM_TypedMesh  m_TypedMeshes;

    MorphTargetDataPtr  m_MorphTargetData;

    Scene();
    Scene( tuid fileID );
    Scene( const std::string &filePath );

    //
    //  Resets the Scene
    //
    void Reset();

    //
    // Load a content file into the Scene
    //
    void Load( const std::string& filePath );
    void Load( const std::string &filePath, Reflect::V_Element& elements, Reflect::StatusHandler* status = NULL );
    void LoadXML( const std::string& xml, Reflect::V_Element& elements, Reflect::StatusHandler* status = NULL );

  private:
    void PostLoad( Reflect::V_Element& elements );

    //
    // Serialize the Scene into a file
    //
  public:
    void Serialize();
    void Serialize( const std::string& filePath );

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
    void GetJointMismatchReport( V_string &mismatchMessages, u32 clipIndex = 0 ) const;

    void CalculateJointBoundingVolumes();
    bool GetBSpheresForJoint          (std::vector<t_BsphereId>& bspheres, const Nocturnal::TUID& jointID, Content::ContentType contentType ) const;
    bool GetBSpheres                  (std::vector<t_BsphereId>& bspheres, Content::ContentType contentType) const;
    bool GetSkinVerts                 (u32 contentType, const std::map<Nocturnal::TUID, u32>& jointUidToId, const Nocturnal::TUID& rootUid, std::vector<t_SkinVerts>& skinVerts) const;

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

    void GetMeshesByType( V_Mesh& meshes, Content::ContentType exportType, i32 groupIndex = -1 ) const;

    void GetMeshesByType( S_Mesh& meshes, Content::ContentType exportType, i32 groupIndex = -1 ) const;

    void SetRenderGeometryToCollision() const;

    void GetAlignedBoundingBox( Math::AlignedBox& box ) const;
    //void GetBSphere( Math::Vector3& position, f32& radius ) const;

    void MergeMeshes();

    void GetMentalRayMeshes( V_Mesh& meshes ) const;
    void GetMentalRayMeshes( V_Mesh& lightmapped, V_Mesh& vertexLit ) const;
    void GetMentalRayMeshes( V_Mesh& lightmapped, V_Mesh& vertexLit, V_Mesh& foliageMeshes ) const;

    // checks for overlap and finds any overlap
    // map is keyed on mesh index. value is a set of triangle indices that overlap
    bool LightmapUVsOverlap( std::map< u32, std::set<u32> >& overlap ) const;

    // checks for overlap. stops as soon as one is found.
    bool LightmapUVsOverlap() const;

    bool LightmapUVsExist() const;

    bool LightmapUVsInRange() const;

    bool MeshesAreClassifiedForLighting() const;    

    void GetUVShells( UVSetType set, V_UVShell& uvShells, std::vector< V_u32 >& triangleIndices ) const;

    // takes power-of-two size of the texture
    f32 AvgLightmapTexelsPerMeter( u32 logTextureSize, Math::Scale* scale = NULL ) const;

    // takes power-of-two size of the texture
    f32 LightmapTexelsPerMeter( u32 logTextureSize, Math::Scale* scale = NULL ) const;

    // given a power-of-two textures size and user-defined uvArea, finds the corresponding tpm
    f32 LightmapTexelsPerMeter( u32 logTextureSize, f32 uvArea ) const;

    f32 UVSurfaceArea( UVSetType uvSetType ) const;

    f32 MeshSurfaceArea( Content::ContentType meshType, Math::Scale* scale = NULL ) const;

    f32 MeshSurfaceAreaComponents( Content::ContentType meshType, Math::Vector3& areaVec ) const;

   // void UniqueAssetClasses( S_tuid& assetClasses, bool (*verifyFunc)( Asset::Entity& ) = NULL ) const;

    void MeshesWithVertDensity( V_Mesh& meshes, f32 density = 10.0f ) const;

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
    void UpdateExportData();
    void UpdateHierarchy();

  public:
    bool HasDuplicateBangleIndexedExportNodes( V_string& duplicate_bangle_ids_info);
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
