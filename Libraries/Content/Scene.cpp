#include "Scene.h"
#include "ContentVisitor.h" 

#include "Effector.h"
#include "Common/Version.h"
#include "Common/Container/Insert.h" 
#include "Common/String/Utilities.h"

#include "rcs/rcs.h"
#include "tuid/TUID.h"
#include "Console/Console.h"

#include "FileSystem/FileSystem.h"
#include "File/Manager.h"

#include "Finder/Finder.h"
#include "Finder/ContentSpecs.h"
#include "Finder/TieSpecs.h"
#include "Finder/ShaderSpecs.h"

#include "AppUtils/AppUtils.h"
#include "Math/EulerAngles.h"
#include "Math/AlignedBox.h"
#include "Math/Line.h"

#include <algorithm>
#include <omp.h>

using Nocturnal::Insert; 
using namespace Reflect;
using namespace File;
using namespace Finder;
using namespace Math;
using namespace Asset;

#pragma TODO("make the min bsphere radius 1mm and get art to not be smaller than that")

#define MIN_MOBY_JOINT_SKIN_WEIGHT  0.1f
#define MIN_MOBY_JOINT_BSPHERE_RAD  0.0005f  // .5 mm

//#define DO_BSPHERE_DUMP // dump building of bspheres - lots of crap
//#define DUMP_LOTS_OF_CRAP

namespace Content
{

  Scene::Scene()
    : m_MorphTargetData( new MorphTargetData() )
  {

  }

  Scene::Scene( tuid fileID )
    : m_MorphTargetData( new MorphTargetData() )
  {
    Load( fileID );
  }

  Scene::Scene( const std::string &filePath )
    : m_MorphTargetData( new MorphTargetData() )
  {
    Load( filePath );
  }

  void Scene::Reset()
  {
    m_FilePath.clear();
    m_Hierarchy.clear();
    m_DependencyNodes.clear();
    m_Transforms.clear();
    m_Joints.clear();
    m_JointIds.clear();
    m_Meshes.clear();
    m_Shaders.clear();
    m_Skins.clear();
    m_ExportNodes.clear();
    m_AddedHierarchyNodes.clear();
    m_AddedDescriptors.clear();
    m_AnimationClips.clear();
    m_TypedMeshes.clear();
    m_MorphTargetData = new MorphTargetData();

    for ( u32 contentType = 0; contentType < Content::ContentTypes::NumContentTypes; ++contentType )
    {
      m_JointBspheres[contentType].clear();
      m_Bspheres[contentType].clear();
    }

    m_JointOrdering = NULL;
  }

  void Scene::Remove( const SceneNodePtr &node )
  {
    // remove the current transform from the scene
    // remove the current transform from
    m_DependencyNodes.erase( node->m_ID );

    // remove the current transform from the hierarchy if it's a Hierarchy node
    HierarchyNodePtr hierarchyNode = ObjectCast< HierarchyNode >( node );
    if( hierarchyNode.ReferencesObject() )
    {
      V_HierarchyNode children;
      GetChildren( children, hierarchyNode );
      for each ( const HierarchyNodePtr& child in children )
      {
        AddChild( child, hierarchyNode->m_ParentID );
      }
      RemoveFromHierarchy( hierarchyNode );      
    }
  }

  void Scene::Add( const SceneNodePtr &node )
  {
    NOC_ASSERT( node.ReferencesObject() );

    Insert<M_DependencyNode>::Result result = m_DependencyNodes.insert( std::pair<UniqueID::TUID, SceneNodePtr>( node->m_ID, node ) );

    //if it already exists in the scene, bail out early
    if( !result.second )
      return;

    if( node->HasType( Reflect::GetType<HierarchyNode>() ) )
    {
      // if it's a hierarchy node, hold onto it so we can update the Scene Hierarchy
      m_AddedHierarchyNodes.push_back( DangerousCast< HierarchyNode >( node ) );
      if( node->HasType( Reflect::GetType<Transform>() ) )
      {
        Transform* transform = DangerousCast< Transform >( node );

        if( transform->HasType( Reflect::GetType<PivotTransform>() ) )
        {
          PivotTransform* pivot = DangerousCast< PivotTransform >( transform );
          m_Transforms.push_back( pivot );

          if( pivot->HasType( Reflect::GetType<Descriptor>() ) )
          {
            Descriptor* exportNode = DangerousCast< Descriptor >( pivot );
            m_ExportNodes.push_back( exportNode );
            // if it's a Descriptor node, hold onto it so we can update the typed meshes
            m_AddedDescriptors.push_back( exportNode );
          }
          else if( node->HasType( Reflect::GetType<Mesh>() ) )
          {
            Mesh* mesh = DangerousCast< Mesh >( node );
            m_Meshes.push_back( mesh );
          }
        }
        else if( transform->HasType( Reflect::GetType<JointTransform>() ) )
        {
          JointTransform* joint = DangerousCast< JointTransform >( transform );
          m_Joints.push_back( joint );
          m_JointIds.insert( joint->m_ID );
        }
      }
    }
    else if( node->HasType( Reflect::GetType<Shader>() ) )
    {
      Shader* shader = DangerousCast< Shader >( node );
      m_Shaders.push_back( shader );
    }      

    else if( node->HasType( Reflect::GetType<Skin>() ) )
    {
      Skin* skin = DangerousCast< Skin >( node );
      m_Skins.push_back( skin );
    }

    else if( node->HasType( Reflect::GetType<AnimationClip>() ) )
    {
      AnimationClip* clip = DangerousCast< AnimationClip >( node );
      m_AnimationClips.push_back( clip );
    }

    m_NodeAddedSignature.Raise( NodeAddedArgs( *node ) );
  }

  void Scene::Load( tuid fileID )
  {
    Reflect::V_Element elements;
    Load( fileID, elements );
  }

  void Scene::Load( const std::string &filePath )
  {
    Reflect::V_Element elements;
    Load( filePath, elements );
  }

  void Scene::Load( const std::string &filePath, Reflect::V_Element& elements, Reflect::StatusHandler* status )
  {
    m_FilePath = filePath;
    Archive::FromFile( filePath, elements, status );
    PostLoad( elements );
  }

  void Scene::LoadXML( const std::string& xml, Reflect::V_Element& elements, Reflect::StatusHandler* status )
  {
    Archive::FromXML( xml, elements, status );
    PostLoad( elements );
  }

  void Scene::PostLoad( Reflect::V_Element& elements )
  {
    // gather data and do stuff with it post-load

    V_Element::iterator itr = elements.begin();
    V_Element::iterator end= elements.end();

    for( ; itr != end; ++itr )
    {
      // if it's not a SceneNode, skip it
      if( !(*itr)->HasType( Reflect::GetType<SceneNode>() ) )
        continue;

      SceneNodePtr dependencyNode = DangerousCast< SceneNode >( *itr );
      Add( dependencyNode );
    }

    // Additional post-processing after all nodes have been created.
    Reflect::V_Element newElements;
    M_DependencyNode::const_iterator nodeItr = m_DependencyNodes.begin();
    M_DependencyNode::const_iterator nodeEnd = m_DependencyNodes.end();
    for ( ; nodeItr != nodeEnd; ++nodeItr )
    {
      nodeItr->second->PostLoad( newElements );
    }

    // Add any new items to the scene which were created during PostLoad
    for ( Reflect::V_Element::const_iterator newItemItr = newElements.begin(), 
      newItemEnd = newElements.end(); newItemItr != newItemEnd; ++newItemItr )
    {
      SceneNodePtr newNode = Reflect::ObjectCast< SceneNode >( *newItemItr );
      if ( newNode.ReferencesObject() )
      {
        Add( newNode );
      }
    }

    Update(); 
  }

  void Scene::Serialize()
  {
    NOC_ASSERT( !m_FilePath.empty() );
    Serialize( m_FilePath );
  }

  void Scene::Serialize( const std::string& filePath )
  {
    RCS::File rcsFile( filePath );
    rcsFile.Open();

    V_Element elements;

    M_DependencyNode::const_iterator itr = m_DependencyNodes.begin();
    M_DependencyNode::const_iterator end = m_DependencyNodes.end();
    for ( ; itr != end; ++itr )
    {
      elements.push_back( itr->second );
    }
  
    Archive::ToFile( elements, filePath );
  }

  void Scene::Update()
  {
    UpdateHierarchy();
    UpdateExportData();

    CalculateJointOrdering();
    m_MorphTargetData->CollateMorphTargets( m_Meshes );
  }

  void Scene::Load( tuid fileID, Reflect::V_Element& elements, Reflect::StatusHandler* status )
  {
    std::string filePath;
    if ( !File::GlobalManager().GetPath( fileID, filePath ) )
    {
      throw Nocturnal::Exception( "Could not locate a content scene with id: "TUID_HEX_FORMAT, fileID );
    }

    Load( filePath, elements, status );
  }

  ShaderPtr Scene::GetShader( const MeshPtr &mesh, u32 triIndex )
  {
    // look up the shader index asssociated with the given tri index,
    //  then look up the shader object in the scene database
    return Get< Shader >( mesh->m_ShaderIDs[ mesh->m_ShaderIndices[triIndex] ] );     
  }

  void Scene::GetChildren( V_HierarchyNode &children, const HierarchyNodePtr &node ) const
  {
    std::pair< MM_HierarchyNode::const_iterator, MM_HierarchyNode::const_iterator > itors = m_Hierarchy.equal_range( node->m_ID );
    MM_HierarchyNode::const_iterator itor = itors.first;
    for( ; itor != itors.second; ++itor )
    {
      children.push_back( itor->second );
    }
  }

  void Scene::GetDescendants( V_HierarchyNode &descendants, const HierarchyNodePtr &node ) const
  {
    std::pair< MM_HierarchyNode::const_iterator, MM_HierarchyNode::const_iterator > itors = m_Hierarchy.equal_range( node->m_ID );
    MM_HierarchyNode::const_iterator itor = itors.first;
    for( ; itor != itors.second; ++itor )
    {
      descendants.push_back( itor->second );
      GetDescendants( descendants, itor->second );
    }
  }

  bool Scene::IsChildOf( const HierarchyNodePtr &potentialChild, const HierarchyNodePtr &potentialParent ) const
  {
    std::pair< MM_HierarchyNode::const_iterator, MM_HierarchyNode::const_iterator > itors = m_Hierarchy.equal_range( potentialParent->m_ID );
    MM_HierarchyNode::const_iterator itor = itors.first;
    for( ; itor != itors.second; ++itor )
    {
      if( itor->first == potentialChild->m_ID )
      {
        return true;
      }
      if ( IsChildOf( potentialChild, itor->second ) )
      {
        return true;
      }
    }
    return false;
  }

  void Scene::RemoveFromParent( const HierarchyNodePtr& node )
  {
    std::pair< MM_HierarchyNode::iterator, MM_HierarchyNode::iterator > itors = m_Hierarchy.equal_range( node->m_ParentID );
    MM_HierarchyNode::iterator itor = itors.first;
    for( ; itor != itors.second; ++itor )
    {
      if( itor->second->m_ID == node->m_ID )
      {
        m_Hierarchy.erase( itor );
        node->m_ParentID = UniqueID::TUID::Null;
        break;
      }      
    }
  }

  void Scene::RemoveFromHierarchy( const HierarchyNodePtr& node )
  {
    RemoveFromParent( node );
    m_Hierarchy.erase( node->m_ID );
  }

  void Scene::AddChild( const HierarchyNodePtr& child, const HierarchyNodePtr& parent )
  {
    AddChild( child, parent->m_ID );
  }

  void Scene::AddChild( const HierarchyNodePtr& child, const UniqueID::TUID& parentID )
  {
    RemoveFromParent( child );
    child->m_ParentID = parentID;
    m_Hierarchy.insert( std::pair<UniqueID::TUID, HierarchyNodePtr>( parentID, child ) );
  }

  void Scene::UpdateHierarchy()
  {
    V_HierarchyNode::const_iterator itor = m_AddedHierarchyNodes.begin();
    V_HierarchyNode::const_iterator end = m_AddedHierarchyNodes.end();

    for( ; itor != end; ++itor )
    {
      m_Hierarchy.insert( std::pair<UniqueID::TUID, HierarchyNodePtr>( (*itor)->m_ParentID, *itor ) );
    }

    // done updating with the current batch of newly added hierarchy nodes...safe to clear it
    m_AddedHierarchyNodes.clear();
  }

  bool Scene::HasDuplicateBangleIndexedExportNodes( V_string& duplicate_bangle_ids_info)
  {
    V_i32 bangle_indices;
    bool res = false;
    for each( const DescriptorPtr& descriptor in m_ExportNodes )
    { 
      if (descriptor->m_ExportType == Content::ContentTypes::Bangle)
      {
        V_i32::iterator ii = std::find(bangle_indices.begin(), bangle_indices.end(), descriptor->m_ContentNum);
        if (ii == bangle_indices.end())
        {
          bangle_indices.push_back(descriptor->m_ContentNum);
        }
        else
        {
          std::ostringstream str; 
          str << descriptor->m_GivenName << " has duplicate bangle id " << descriptor->m_ContentNum<<"\n";
          duplicate_bangle_ids_info.push_back(str.str());
          res = true;
        }
      }
    }
    return res;
  }

  void Scene::UpdateExportData()
  {
    for each( const DescriptorPtr& descriptor in m_ExportNodes )
    {
      // gather the descendants of each Descriptor
      V_HierarchyNode descendants;
      GetDescendants( descendants, descriptor );

      V_HierarchyNode::iterator childItor = descendants.begin();
      V_HierarchyNode::iterator childEnd  = descendants.end();

      for( ; childItor != childEnd; ++childItor )
      {
        // if it's a mesh, put it in the correct container
        MeshPtr mesh = ObjectCast< Mesh >( *childItor );
        if( mesh )
        {
          ExportTypeAndIndexPair typeAndLodIndex( descriptor->m_ExportType, descriptor->m_ContentNum );
          m_TypedMeshes.insert( std::pair< ExportTypeAndIndexPair, MeshPtr >(typeAndLodIndex, mesh) );
          mesh->m_ExportTypes[ descriptor->m_ExportType ] = true;
          mesh->m_ExportTypeIndex.insert( M_ContentTypeToIndex::value_type( descriptor->m_ExportType, descriptor->m_ContentNum ) );
        }
        CollisionPrimitivePtr collprim = ObjectCast<CollisionPrimitive>( *childItor );
        // this is specifically for prims on ties and ufrag...
        // we can't set the descriptor all the time or it will break moby collision
        // this is still probably pretty bad kludge -EC
        if(collprim && 
           ((descriptor->m_ExportType == Content::ContentTypes::HighResCollision) ||
           (descriptor->m_ExportType == Content::ContentTypes::LowResCollision)))
        {
          collprim->m_ExportTypes[ descriptor->m_ExportType] = true;
        }
      }
    }
    // done updating with the current batch of newly added export transform nodes...safe to clear it
    m_AddedDescriptors.clear();
  }

  void Scene::GetMeshesByType( S_Mesh& meshes, Content::ContentType exportType, i32 groupIndex ) const
  {
    MM_TypedMesh::iterator itor = m_TypedMeshes.begin();
    MM_TypedMesh::iterator end = m_TypedMeshes.end();
    for( ; itor != end; ++itor )
    {
      const ExportTypeAndIndexPair& typeAndLod = itor->first;
      const MeshPtr& contentMesh = itor->second;

      if( exportType == ContentTypes::Default || typeAndLod.first == exportType )
      {
        if( groupIndex == -1 || groupIndex == typeAndLod.second )
          meshes.insert( contentMesh );
      }
    }
    /*
    for each( const DescriptorPtr& descriptor in m_ExportNodes )
    {
      if( groupIndex > -1 && descriptor->m_ContentNum != groupIndex )
        continue;

      if( descriptor->m_ExportType != exportType )
        continue;

      V_HierarchyNode children;
      GetDescendants( children, descriptor );

      for each( const HierarchyNodePtr& child in children )
      {
        MeshPtr mesh = ObjectCast< Mesh >( child );
        if( mesh )
          meshes.insert( mesh );
      }
    }
    */
  }

  // FIXME linear search, maybe think about other ways we may want to handle this in the future?
  void Scene::GetMeshesByType( V_Mesh& meshes, Content::ContentType exportType, i32 groupIndex ) const
  {
    MM_TypedMesh::iterator itor = m_TypedMeshes.begin();
    MM_TypedMesh::iterator end = m_TypedMeshes.end();
    for( ; itor != end; ++itor )
    {
      const ExportTypeAndIndexPair& typeAndLod = itor->first;
      const MeshPtr& contentMesh = itor->second;

      if( exportType == ContentTypes::Default || typeAndLod.first == exportType )
      {
        if( groupIndex == -1 || groupIndex == typeAndLod.second )
          meshes.push_back( contentMesh );
      }
    }
    /*
    for each( const DescriptorPtr& descriptor in m_ExportNodes )
    {
      if( groupIndex > -1 && descriptor->m_ContentNum != groupIndex )
        continue;

      if( descriptor->m_ExportType != exportType )
        continue;

      V_HierarchyNode children;
      GetDescendants( children, descriptor );

      for each( const HierarchyNodePtr& child in children )
      {
        MeshPtr mesh = ObjectCast< Mesh >( child );
        if( mesh )
          meshes.push_back( mesh );
      }
    }
    */
  }

  bool Scene::Exists( const UniqueID::TUID& id )
  {
    M_DependencyNode::iterator findItor = m_DependencyNodes.find( id );
    return ( findItor != m_DependencyNodes.end() );
  }

  void Scene::UpdateGlobalTransforms( const TransformPtr& parent )
  {
    V_HierarchyNode children;
    HierarchyNodePtr hierarchyNode = ObjectCast< HierarchyNode >( parent );
    if( !hierarchyNode ) 
      return;

    GetChildren( children, hierarchyNode );

    for each( const HierarchyNodePtr& node in children )
    {
      TransformPtr child = ObjectCast< Transform >( node );
      if( child )
      {
        child->m_GlobalTransform = parent->m_GlobalTransform * child->m_ObjectTransform;
        UpdateGlobalTransforms( child );
      }
    }
  }

  void Scene::UpdateGlobalTransforms()
  {
    for each( const PivotTransformPtr& transform in m_Transforms )
    {
      if( !Exists( transform->m_ParentID )  )
      {
        UpdateGlobalTransforms( transform );
      }
    }
  }


  //
  // This function optimizes the scene by:
  //  Removing extraneous PivotTransform objects from the hierarchy (saving any Descriptor nodes)
  //  Recomputing the local and component transform data for all the children (in case thier parent was removed)
  //  Transforming mesh geometry into world space
  //

  void Scene::Optimize()
  {
    M_DependencyNode dependencyNodes = m_DependencyNodes;

    M_DependencyNode::const_iterator itr = dependencyNodes.begin();
    M_DependencyNode::const_iterator end = dependencyNodes.end();
    for ( ; itr != end; ++itr )
    {
      HierarchyNodePtr node = ObjectCast<HierarchyNode>( itr->second );

      if (!node)
      {
        continue;
      }

      if (node->m_ParentID == UniqueID::TUID::Null || m_DependencyNodes.find( node->m_ParentID ) == m_DependencyNodes.end() )
      {
        // node is a root node, he has not parent whether that be by null id or just missing
        Optimize( node );          
      }
    }
  }

  //
  // Note there is a lot of crap going on in this function
  //  Transform fudging uses the global matrix always to stay true
  //  to the overall object transformation no matter how we have munged
  //  our parent object (including changing the parent entirely)
  //

  void Scene::Optimize(const HierarchyNodePtr& object)
  {
    // cache our current children before we do anything drastic
    V_HierarchyNode children;
    GetChildren(children, object);

    bool removed = false;
    if (object->HasType( Reflect::GetType<PivotTransform>() ))
    {
      PivotTransformPtr transform = DangerousCast< PivotTransform >( object );

      if ( object->HasType( Reflect::GetType<Descriptor>() ) )
      {
        //
        // Keep him around and reset local transform to identity
        //

        transform->ResetTransform();
      }
      else if ( object->HasType( Reflect::GetType<Mesh>() ) )
      {
        //
        // Transform vertices and normals and reset transform to identity
        //

        MeshPtr mesh = DangerousCast< Mesh >( object );

        if( !mesh->m_ExportTypes[ ContentTypes::Foliage ] )
        {
          // don't bother if the transform is already identity
          if( mesh->m_GlobalTransform != Math::Matrix4::Identity )
          {
            const Math::Matrix4& globalTransform = mesh->m_GlobalTransform;

            Math::Matrix4 normalTransform = globalTransform.Inverted().Transposed();

            size_t len = mesh->m_Positions.size();
            NOC_ASSERT( len == mesh->m_Normals.size() );

            for( u32 i = 0; i < len; ++i )
            {
              globalTransform.TransformVertex( mesh->m_Positions[i] );
              normalTransform.TransformVertex( mesh->m_Normals[i] );
              mesh->m_Normals[i].Normalize();             
            }
          }

          transform->ResetTransform();
        }
      }
      else if (object->GetType() != Reflect::GetType<PivotTransform>())
      {
        //
        // This is something we want to keep (and presumably persist its transform)
        //  so recompute its local transform because its parent could have been
        //  optimized away (we still have a hierarchy to maintain relationships, but every
        //  global and local transform will become equal)
        //

        M_DependencyNode::const_iterator found = m_DependencyNodes.find( object->m_ParentID );

        if (found != m_DependencyNodes.end())
        {
          TransformPtr parentTransform = ObjectCast< Transform > ( found->second );

          if (parentTransform.ReferencesObject())
          {
            Math::Matrix4 matrix = transform->m_GlobalTransform;

            // we have the baked matrix data, reset our transform node
            transform->ResetTransform();

            Math::EulerAngles rotate;
            matrix.Decompose( (Math::Scale)transform->m_Scale, transform->m_Shear, rotate, transform->m_Translate );
            transform->m_Rotate = Math::Vector3 (rotate.I(), rotate.J(), rotate.H());
            transform->m_ObjectTransform = matrix;
            transform->m_GlobalTransform = matrix;
          }
        }
      }
      else
      {
        //
        // It must be exactly a pivot transform, remove it from the hierarchy (its a group node)
        //

        NOC_ASSERT( object->GetType() == Reflect::GetType<PivotTransform>() );

        removed = true;
      }
    }

    // now recurse on our children, having changed the hierarchy
    //  (children's parent id will be updated)
    V_HierarchyNode::const_iterator itr = children.begin();
    V_HierarchyNode::const_iterator end = children.end();
    for ( ; itr != end; ++itr )
    {
      Optimize( *itr );
    }

    if ( removed )
    {
      Remove( object );
    }
  }

  void Scene::GetJointsFromClip( const AnimationClipPtr& clip, V_JointTransform& joints )
  {
    UniqueID::V_TUID jointIDs;
    clip->GetJointIDs( jointIDs );

    for each ( const UniqueID::TUID& uid in jointIDs )
    {
      joints.push_back( Get<JointTransform>( uid ) );
    }
  }

  void Scene::SetRenderGeometryToCollision() const
  {
    S_Mesh meshes;
    GetMeshesByType( meshes, Content::ContentTypes::Geometry );

    S_Mesh::iterator itor = meshes.begin();
    S_Mesh::iterator end = meshes.end();
    for( ; itor != end; ++itor )
    {
      if( (*itor)->m_ExportTypes[Content::ContentTypes::Foliage] )
        continue;

      (*itor)->m_ExportTypes[Content::ContentTypes::HighResCollision] = true;
      ExportTypeAndIndexPair typeAndLodIndex( Content::ContentTypes::HighResCollision, 0 );
      m_TypedMeshes.insert( std::pair< ExportTypeAndIndexPair, MeshPtr >(typeAndLodIndex, *itor) );
    }    
  }

  const JointOrderingPtr& Scene::GetJointOrdering() const
  {
    return m_JointOrdering;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void Scene::CalculateJointBoundingVolumes()
  {

#ifdef DUMP_LOTS_OF_CRAP

    // dump joint ids
    printf("----------------------------------------\n");
    printf("%5d joints\n", m_JointIds.size());
    for(UniqueID::S_TUID::const_iterator ijoint = m_JointIds.begin(); ijoint != m_JointIds.end(); ++ijoint)
    {
      std::string joint_str;
      ijoint->ToString(joint_str);

      JointTransform* joint = Get< JointTransform >(*ijoint);

      printf("$$$joint_id '%s' - '%s'\n", joint_str.c_str(), joint ? joint->m_Name.c_str() : "bad uid");
    }



//    // dump inv bind mats
//    printf("%%%----------------------------------------\n");
//    for(UniqueID::S_TUID::const_iterator ijoint = m_JointIds.begin(); ijoint != m_JointIds.end(); ++ijoint)
//    {
//      const igSuperJointArray* bindJoints = m_Processor.GetEngineScene().GetSuperJointArray();
//
//      std::string joint_str;
//      ijoint->ToString(joint_str);
//
//      JointTransform* joint = Get< JointTransform >(joint_id);
//
//      printf("%%%joint_id '%s' - '%s'\n", joint_str.c_str(), joint ? joint->m_Name.c_str() : "bad uid");
//    }



    // dump verts
    printf("$$$----------------------------------------\n");
    for(u32 content_type = 0; content_type < Content::ContentTypes::NumContentTypes; content_type++)
    {
      if ((content_type != Content::ContentTypes::Geometry) && (content_type != Content::ContentTypes::Bangle))
      {
        continue;
      }

      for each (const SkinPtr& skin in m_Skins )
      {
        MeshPtr mesh = Get< Mesh >(skin->m_Mesh);

        if ((!mesh.ReferencesObject()) || (mesh->m_ExportTypes[content_type] == false))
        {
          continue;
        }

        u64 mesh_id = (u64)0;
        if (content_type == Content::ContentTypes::Bangle)
        {
          mesh_id = ((u64)mesh->GetExportTypeIndex((Content::ContentType)content_type) + 1);
        }

        printf("$$$%5d mesh_id\n", mesh_id);

        size_t num_verts = (u32)mesh->m_Positions.size();

        printf("$$$%5d verts\n", num_verts);

        for(size_t ivert = 0; ivert < num_verts; ivert++)
        {
          const Math::Vector3&  pos         = mesh->m_Positions[ivert];
          u32                   iskin_vert  = skin->m_InfluenceIndices[ivert];
          InfluencePtr          influence   = skin->m_Influences[iskin_vert];
          u32                   num_weights = (u32)influence->m_Weights.size();

          printf("$$$%5d %f %f %f\n", ivert, pos.x, pos.y, pos.z);
          printf("$$$%5d infs\n", num_weights);

          for(u32 inf = 0; inf < num_weights; inf++)
          {
            f32                 weight          = influence->m_Weights[inf];
            const UniqueID::TUID& weight_joint_id = skin->m_InfluenceObjectIDs[influence->m_Objects[inf]];

            std::string weight_joint_str;
            weight_joint_id.ToString(weight_joint_str);

            printf("$$$%5d '%s' %f\n", inf, weight_joint_str.c_str(), weight);
          }
        }      
      }
    }

#endif //DUMP_LOTS_OF_CRAP


    t_JointVertsMap     joint_verts_map[Content::ContentTypes::NumContentTypes];

    typedef std::map<i32, Math::V_Vector3>  t_MeshIdBsphereMap;
    t_MeshIdBsphereMap  meshid_verts_map[Content::ContentTypes::NumContentTypes];

    // 
    // grab verts for each content type, partitioned by (joint uid, mesh export type id)
    //                                   partitioned by (mesh export type)
    // 
    for(u32 content_type = 0; content_type < Content::ContentTypes::NumContentTypes; content_type++)
    {
      for each (const SkinPtr& skin in m_Skins )
      {
        MeshPtr mesh = Get< Mesh >(skin->m_Mesh);

        if ((!mesh.ReferencesObject()) || (mesh->m_ExportTypes[content_type] == false))
        {
          continue;
        }

        u64 mesh_id = (u64)0;
        if (content_type == Content::ContentTypes::Bangle)
        {
          mesh_id = ((u64)mesh->GetExportTypeIndex((Content::ContentType)content_type) + 1);
        }

        size_t num_verts = (u32)mesh->m_Positions.size();
        for(size_t ivert = 0; ivert < num_verts; ivert++)
        {
          const Math::Vector3&  pos         = mesh->m_Positions[ivert];
          u32                   iskin_vert  = skin->m_InfluenceIndices[ivert];
          InfluencePtr          influence   = skin->m_Influences[iskin_vert];
          u32                   num_weights = (u32)influence->m_Weights.size();

          for(u32 inf = 0; inf < num_weights; inf++)
          {
            f32 weight = influence->m_Weights[inf];
            if (weight < MIN_MOBY_JOINT_SKIN_WEIGHT)
            {
              continue;
            }

            const UniqueID::TUID&  joint_id  = skin->m_InfluenceObjectIDs[influence->m_Objects[inf]];
            t_UidId              uid_id    = std::make_pair(joint_id, mesh_id);

            joint_verts_map[content_type][uid_id].push_back(pos);
          }

          meshid_verts_map[content_type][(u32)mesh_id].push_back(pos);
        }      
      }
    }





    // 
    // calc bsphere for joint verts and for mesh type
    //
    for(u32 content_type = 0; content_type < Content::ContentTypes::NumContentTypes; content_type++)
    {
      #ifdef DO_BSPHERE_DUMP
      {
        Console::Debug("--------------------\n");
        Console::Debug("contentType %d\n", content_type);
      }
      #endif //DO_BSPHERE_DUMP

      // 
      // joints
      // 
      t_JointVertsMap::iterator start = joint_verts_map[content_type].begin();
      t_JointVertsMap::iterator end   = joint_verts_map[content_type].end();

      for(t_JointVertsMap::iterator i = start; i != end; ++i)
      {
        const t_UidId&      uid_id      = i->first;
        const UniqueID::TUID& joint_id    = uid_id.first;
        u64                 mesh_id     = uid_id.second;
        Math::V_Vector3&    joint_verts = i->second;

        #ifdef DO_BSPHERE_DUMP
        {
          std::string joint_str;
          joint_id.ToString(joint_str);

          JointTransform* joint = Get< JointTransform >(joint_id);

          Console::Debug("-----\n");
          Console::Debug("joint '%s' (%s)\n", joint_str.c_str(), joint ? joint->m_Name.c_str() : "bad uid");
        }
        #endif //DO_BSPHERE_DUMP
        
        Math::BoundingVolumeGenerator           bvg(&joint_verts[0], (int)joint_verts.size(), Math::BoundingVolumeGenerator::BSPHERE_OPTIMIZED);
        Math::BoundingVolumeGenerator::BSphere  bvg_bsphere = bvg.GetPrincipleAxisBoundingSphere();

        if (bvg_bsphere.m_Radius >= MIN_MOBY_JOINT_BSPHERE_RAD)
        {
          m_JointBspheres[content_type][joint_id].push_back(std::make_pair(bvg_bsphere, mesh_id));
        }


        #ifdef DO_BSPHERE_DUMP
        {
          Console::Debug("  bsphere: myCreateSphere(%f, %f, %f, %f);\n", bvg_bsphere.m_Center.x, bvg_bsphere.m_Center.y, bvg_bsphere.m_Center.z, bvg_bsphere.m_Radius);

          Math::Vector3* vv = &joint_verts[0];
          for(u32 i = 0; i < (u32)joint_verts.size(); i++)
          {
            Console::Debug("myCreatePoint(%f, %f, %f);\n", vv->x, vv->y, vv->z);
            vv++;
          }
        }
        #endif //DO_BSPHERE_DUMP
      }



      //
      // mesh
      //
      for(t_MeshIdBsphereMap::iterator it = meshid_verts_map[content_type].begin(); it != meshid_verts_map[content_type].end(); ++it)
      {
        u32               mesh_id     = it->first;
        Math::V_Vector3&  mesh_verts  = it->second;

        Math::BoundingVolumeGenerator           bvg(&mesh_verts[0], (int)mesh_verts.size(), Math::BoundingVolumeGenerator::BSPHERE_OPTIMIZED);
        Math::BoundingVolumeGenerator::BSphere  bvg_bsphere = bvg.GetPrincipleAxisBoundingSphere();

        if (bvg_bsphere.m_Radius >= MIN_MOBY_JOINT_BSPHERE_RAD)
        {
          t_BsphereId bsphere = std::make_pair(bvg_bsphere, mesh_id);
          m_Bspheres[content_type].push_back(bsphere);
        }
      }
    }

#if 0

        #ifdef DO_BSPHERE_DUMP
        {
          std::string joint_str;
          joint_id.ToString(joint_str);

          JointTransform* joint = Get< JointTransform >(joint_id);

          Console::Debug("-----\n");
          Console::Debug("joint '%s' (%s)\n", joint_str.c_str(), joint ? joint->m_Name.c_str() : "bad uid");
        }
        #endif //DO_BSPHERE_DUMP

        Math::BoundingVolumeGenerator bvg(&itor->second[0], (int)itor->second.size(), true);
        //Math::BoundingVolumeGenerator bvg(&itor->second[0], (int)itor->second.size(), false);
        Math::BoundingVolumeGenerator::BSphere bvg_bsphere = bvg.GetPrincipleAxisBoundingSphere();

        m_JointBspheres[contentType][joint_id] = bvg_bsphere;


        #ifdef DO_BSPHERE_DUMP
        {
          Console::Debug("  bsphere: myCreateSphere(%f, %f, %f, %f);\n", bvg_bsphere.m_Center.x, bvg_bsphere.m_Center.y, bvg_bsphere.m_Center.z, bvg_bsphere.m_Radius);

          Math::Vector3* vv = &itor->second[0];
          for(u32 i = 0; i < (u32)itor->second.size(); i++)
          {
            Console::Debug("myCreatePoint(%f, %f, %f);\n", vv->x, vv->y, vv->z);
            vv++;
          }
        }
        #endif //DO_BSPHERE_DUMP
          

        Math::Vector3* v = &itor->second[0];
        for(u32 i = 0; i < (u32)itor->second.size(); i++)
        {
          verts.push_back(*v++);
        }
      }


      Math::BoundingVolumeGenerator           bvg(&verts[0], (u32)verts.size(), true);
      //Math::BoundingVolumeGenerator           bvg(&verts[0], (u32)verts.size(), false);
      Math::BoundingVolumeGenerator::BSphere  bvg_bsphere = bvg.GetPrincipleAxisBoundingSphere();

      #ifdef DO_BSPHERE_DUMP
      {
        Console::Debug("new bsphere: myCreateSphere(%f, %f, %f, %f);\n", bvg_bsphere.m_Center.x, bvg_bsphere.m_Center.y, bvg_bsphere.m_Center.z, bvg_bsphere.m_Radius);
      }
      #endif //DO_BSPHERE_DUMP

      m_JointsBsphere[contentType].m_Center = bvg_bsphere.m_Center;
      m_JointsBsphere[contentType].m_Radius = bvg_bsphere.m_Radius;
#endif

    // 
    // dump verts
    // 
    #ifdef DO_BSPHERE_DUMP
    {
      Console::Debug("\n\n\n--------------------\n");

      for(u32 content_type = 0; content_type < Content::ContentTypes::NumContentTypes; content_type++)
      {
        Console::Debug("***** content_type %d\n", content_type);

        t_JointVertsMap::const_iterator  start = joint_verts_map[content_type].begin();
        t_JointVertsMap::const_iterator  end   = joint_verts_map[content_type].end();

        for(t_JointVertsMap::const_iterator i = start; i != end; ++i)
        {
          const t_UidId&          uid_id  = i->first;
          const Math::V_Vector3&  verts   = i->second;

          const UniqueID::TUID&  joint_id  = uid_id.first;
          u64                  mesh_id   = uid_id.second;

          std::string joint_str;
          joint_id.ToString(joint_str);

          JointTransform* joint = Get< JointTransform >(joint_id);

          Console::Debug("*****   joint '%s' - mesh_id %2d - '%s'\n", joint_str.c_str(), (u32)mesh_id, joint ? joint->m_Name.c_str() : "bad uid");

          for(u32 iv = 0; iv < verts.size(); iv++)
          {
            Console::Debug("*****    myCreatePoint(%f, %f, %f);\n", verts[iv].x, verts[iv].y, verts[iv].z);
          }
        }
      }
    }
    #endif //DO_BSPHERE_DUMP



    #ifdef DO_BSPHERE_DUMP
    {
      // 
      // dump bspheres
      // 
      for(u32 content_type = 0; content_type < Content::ContentTypes::NumContentTypes; content_type++)
      {
        M_UIDBSphere::const_iterator  start = m_JointBspheres[content_type].begin();
        M_UIDBSphere::const_iterator  end   = m_JointBspheres[content_type].end();

        for(M_UIDBSphere::const_iterator i = start; i != end; ++i)
        {
          const UniqueID::TUID&             joint_id  = i->first;
          const std::vector<t_BsphereId>& bspheres  = i->second;

          for(size_t ibsphere = 0; ibsphere < bspheres.size(); ibsphere++)
          {
            const Math::BoundingVolumeGenerator::BSphere& bsphere = bspheres[ibsphere].first;
            u64                                           mesh_id = bspheres[ibsphere].second;

            std::string joint_str;
            joint_id.ToString(joint_str);

            JointTransform* joint = Get< JointTransform >(joint_id);

            Console::Debug("***** joint '%s' - mesh_id %2d - %f %f %f %f - '%s'\n", joint_str.c_str(), (u32)mesh_id, bsphere.m_Center.x, bsphere.m_Center.y, bsphere.m_Center.z, bsphere.m_Radius, joint ? joint->m_Name.c_str() : "bad uid");
          }
        }
      }
    }
    #endif //DO_BSPHERE_DUMP



    #ifdef DO_BSPHERE_DUMP
    {
      // 
      // dump mesh bspheres
      // 
      for(u32 content_type = 0; content_type < Content::ContentTypes::NumContentTypes; content_type++)
      {
        for(u32 i = 0; i < m_Bspheres[content_type].size(); i++)
        {
          const Math::BoundingVolumeGenerator::BSphere& bsphere = m_Bspheres[content_type][i].first;
          u64                                           mesh_id = m_Bspheres[content_type][i].second;

          Console::Debug("***** mesh_id %2d - myCreateSphere(%f, %f, %f, %f);\n", (u32)mesh_id, bsphere.m_Center.x, bsphere.m_Center.y, bsphere.m_Center.z, bsphere.m_Radius);
        }
      }
    }
    #endif //DO_BSPHERE_DUMP




#if 0

    MapUidBsphere_t m_JointBspheres[kNumContentTypes];   // for each content type, map uid, mesh id to bsphere
    BSphere         m_JointBspheres[kNumContentTypes];    // for each content type, bsphere
    //@@@ does this need to change ? - do we need one for each
    //@@@ bangle-id ?
    //@@@ when do we use this ?



    #if 0
    {
      for(UniqueID::S_TUID::const_iterator ijoint = m_JointIds.begin(); ijoint != m_JointIds.end(); ++ijoint)
      {
        UniqueID::TUID joint_id = *ijoint;

        std::string joint_str;
        joint_id.ToString(joint_str);

        JointTransform* joint = Get< JointTransform >(joint_id);

        Console::Debug("@@@-----\n");
        Console::Debug("@@@joint '%s' (%s)\n", joint_str.c_str(), joint ? joint->m_Name.c_str() : "bad uid");


        for each (const SkinPtr& skin in m_Skins )
        {
          MeshPtr mesh = Get< Mesh >( skin->m_Mesh );

          // skip in invalid mesh
          if (mesh.ReferencesObject() == false)
          {
            continue;
          }

          // skip if not our content type
          if ((mesh->m_ExportTypes[Content::ContentTypes::Geometry] == false) && (mesh->m_ExportTypes[Content::ContentTypes::Bangle] == false))
          {
            continue;
          }

          u32 num_verts = (u32)mesh->m_Positions.size();
          for(u32 ivert = 0; ivert < num_verts; ++ivert)
          {
            u32           iinf_vert   = skin->m_InfluenceIndices[ivert];
            InfluencePtr  influence   = skin->m_Influences[iinf_vert];
            u32           num_weights = (u32)influence->m_Weights.size();

            for(u32 inf = 0; inf < num_weights; inf++)
            {
              // skip if not our joint-id
              if (skin->m_InfluenceObjectIDs[influence->m_Objects[inf]] != joint_id)
              {
                continue;
              }

              // skip if minimally influenced
              f32 weight = influence->m_Weights[inf];
              if (weight < MIN_MOBY_JOINT_SKIN_WEIGHT)
              {
                continue;
              }

              const Math::Vector3& v = mesh->m_Positions[ivert];
              Console::Debug("@@@blah (%10.9f) myCreatePoint(%f, %f, %f);\n", influence->m_Weights[inf], v.x, v.y, v.z);
            }
          }
        }
      }
    }
    #endif



    for each (const SkinPtr& skin in m_Skins )
    {
      MeshPtr mesh = Get< Mesh >( skin->m_Mesh );
      if( !mesh.ReferencesObject() )
        continue;

      u32 num_verts = (u32)mesh->m_Positions.size();
      for( u32 ivert = 0; ivert < num_verts; ++ivert )
      {
        u32           iskin_vert  = skin->m_InfluenceIndices[ivert];
        InfluencePtr  influence   = skin->m_Influences[iskin_vert];
        u32           num_weights = (u32)influence->m_Weights.size();

        for(u32 inf = 0; inf < num_weights; inf++)
        {
          f32 weight = influence->m_Weights[inf];
          if (weight < MIN_MOBY_JOINT_SKIN_WEIGHT)
          {
            continue;
          }

          UniqueID::TUID joint_id = skin->m_InfluenceObjectIDs[influence->m_Objects[inf]];

          for( u32 contentType = 0; contentType < Content::ContentTypes::NumContentTypes; ++contentType )
          {
            if( !mesh->m_ExportTypes[contentType] )
              continue;

            joint_verts_map[contentType][joint_id].push_back(mesh->m_Positions[ivert]);
          }          
        }
      }      
    }




    // 
    // calc bsphere for joint verts
    // 
    Math::V_Vector3 verts;
    for( u32 contentType = 0; contentType < Content::ContentTypes::NumContentTypes; ++contentType )
    {
      #ifdef DO_BSPHERE_DUMP
      {
        Console::Debug("--------------------\n");
        Console::Debug("contentType %d\n", contentType);
      }
      #endif //DO_BSPHERE_DUMP

      t_JointVertsMap::iterator itor = joint_verts_map[contentType].begin();
      t_JointVertsMap::iterator end = joint_verts_map[contentType].end();

      if (itor != end)
      {
        size_t num_verts = 0;
        for(t_JointVertsMap::iterator i = itor; i != end; ++i)
        {
          num_verts += i->second.size();
        }

        verts.reserve(num_verts);


      for( ; itor != end; ++itor )
      {
          UniqueID::TUID joint_id = itor->first;

          #ifdef DO_BSPHERE_DUMP
          {
            std::string joint_str;
            joint_id.ToString(joint_str);

            JointTransform* joint = Get< JointTransform >(joint_id);

            Console::Debug("-----\n");
            Console::Debug("joint '%s' (%s)\n", joint_str.c_str(), joint ? joint->m_Name.c_str() : "bad uid");
          }
          #endif //DO_BSPHERE_DUMP

          Math::BoundingVolumeGenerator bvg(&itor->second[0], (int)itor->second.size(), true);
          //Math::BoundingVolumeGenerator bvg(&itor->second[0], (int)itor->second.size(), false);
        Math::BoundingVolumeGenerator::BSphere bvg_bsphere = bvg.GetPrincipleAxisBoundingSphere();

          m_JointBspheres[contentType][joint_id] = bvg_bsphere;


          #ifdef DO_BSPHERE_DUMP
          {
            Console::Debug("  bsphere: myCreateSphere(%f, %f, %f, %f);\n", bvg_bsphere.m_Center.x, bvg_bsphere.m_Center.y, bvg_bsphere.m_Center.z, bvg_bsphere.m_Radius);

            Math::Vector3* vv = &itor->second[0];
            for(u32 i = 0; i < (u32)itor->second.size(); i++)
            {
              Console::Debug("myCreatePoint(%f, %f, %f);\n", vv->x, vv->y, vv->z);
              vv++;
            }
        }
          #endif //DO_BSPHERE_DUMP


          Math::Vector3* v = &itor->second[0];
          for(u32 i = 0; i < (u32)itor->second.size(); i++)
        {
            verts.push_back(*v++);
        }
        }


        Math::BoundingVolumeGenerator           bvg(&verts[0], (u32)verts.size(), true);
        //Math::BoundingVolumeGenerator           bvg(&verts[0], (u32)verts.size(), false);
        Math::BoundingVolumeGenerator::BSphere  bvg_bsphere = bvg.GetPrincipleAxisBoundingSphere();

        #ifdef DO_BSPHERE_DUMP
        {
          Console::Debug("new bsphere: myCreateSphere(%f, %f, %f, %f);\n", bvg_bsphere.m_Center.x, bvg_bsphere.m_Center.y, bvg_bsphere.m_Center.z, bvg_bsphere.m_Radius);
        }
        #endif //DO_BSPHERE_DUMP

        m_JointsBsphere[contentType].m_Center = bvg_bsphere.m_Center;
        m_JointsBsphere[contentType].m_Radius = bvg_bsphere.m_Radius;


        // clear for next time
        verts.clear();
      }
      else
      {
        m_JointsBsphere[contentType].m_Center = Math::Vector3(0.0f, 0.0f, 0.0f);
        m_JointsBsphere[contentType].m_Radius = 0.0f;
      }
    }
#endif
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  bool Scene::GetBSpheresForJoint(std::vector<t_BsphereId>& bspheres, const UniqueID::TUID& jointID, Content::ContentType contentType ) const
  {
    M_UIDBSphere::const_iterator findItor = m_JointBspheres[contentType].find( jointID);
    if( findItor == m_JointBspheres[contentType].end() )
    {
      return false;
    }

    bspheres = findItor->second;
    return true;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  bool Scene::GetBSpheres(std::vector<t_BsphereId>& bspheres, Content::ContentType contentType) const
  {
    if (m_Bspheres[contentType].empty())
    {
      return false;
    }

    bspheres = m_Bspheres[contentType];
    return true;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  bool Scene::GetSkinVerts(u32 contentType, const std::map<UniqueID::TUID, u32>& jointUidToId, const UniqueID::TUID& rootUid, std::vector<t_SkinVerts>& skinVerts) const
  {
    u32         root_id = jointUidToId.find(rootUid)->second;
    M_u32       mesh_id_to_skin_verts;
    t_SkinVerts new_verts;
    size_t      total_num_weights = 0;


    for each (const SkinPtr& skin in m_Skins )
    {
      MeshPtr mesh = Get< Mesh >(skin->m_Mesh);

      if ((!mesh.ReferencesObject()) || (mesh->m_ExportTypes[contentType] == false))
      {
        continue;
      }

      u32 mesh_id = 0;
      if (contentType == Content::ContentTypes::Bangle)
      {
        mesh_id = (mesh->GetExportTypeIndex((Content::ContentType)contentType) + 1);
      }


      M_u32::iterator it            = mesh_id_to_skin_verts.find(mesh_id);
      u32             skin_verts_id = 0;

      if (it != mesh_id_to_skin_verts.end())
      {
        // we already have this mesh - get its entry
        skin_verts_id = it->second;
      }
      else
      {
        // we dont have this mesh - add it
        new_verts.m_mesh_id = mesh_id;
        skinVerts.push_back(new_verts);

        skin_verts_id = (u32)(skinVerts.size() - 1);
        mesh_id_to_skin_verts[mesh_id] = skin_verts_id;
      }

      t_SkinVerts&  skin_verts            = skinVerts[skin_verts_id];
      size_t        new_skin_verts_start  = skin_verts.m_verts.size();

      NOC_ASSERT(skin_verts.m_mesh_id == mesh_id);

      size_t num_verts = mesh->m_Positions.size();

      u32 pre_total_num_weights = (u32)total_num_weights;

      if ( num_verts > 0 )
      {
        for(size_t ivert = 0; ivert < num_verts; ivert++)
        {
          const Math::Vector3&  pos         = mesh->m_Positions[ivert];
          u32                   iskin_vert  = skin->m_InfluenceIndices[ivert];
          InfluencePtr          influence   = skin->m_Influences[iskin_vert];
          size_t                num_weights = influence->m_Weights.size();

          t_SkinVert v;
          v.m_pos = pos;
          v.m_joint_ids.reserve(num_weights);
          v.m_joints.reserve   (num_weights);
          v.m_weights.reserve  (num_weights);

          size_t                num_joint_influences = 0;

          for(size_t inf = 0; inf < num_weights; inf++)
          {
            const UniqueID::TUID& joint_id = skin->m_InfluenceObjectIDs[influence->m_Objects[inf]];

            i32 joint_index = -1;

            TransformPtr transform = Get< Transform >( joint_id );

            std::map< UniqueID::TUID, u32 >::const_iterator indexIt = jointUidToId.find(joint_id);
            
            if ( indexIt == jointUidToId.end() )
            {
              std::string jointStr;
              joint_id.ToString( jointStr );
              continue;
            }

            joint_index = indexIt->second;

            v.m_joint_ids.push_back( joint_id );
            v.m_joints.push_back( joint_index );
            v.m_weights.push_back( influence->m_Weights[inf] );

            num_joint_influences++;
          }

          if ( num_joint_influences > 0 )
          {
            skin_verts.m_verts.push_back(v);
            total_num_weights += num_joint_influences;
          }
        }

        if (total_num_weights == pre_total_num_weights)
        {
          //printf("*** contentType %d - mesh %3d (bangle_id %3d) has 0 weights\n", contentType, skin_verts_id, mesh_id);

          for(size_t ivert = 0; ivert < num_verts; ivert++)
          {
            t_SkinVert v;
            v.m_joint_ids.push_back(rootUid);
            v.m_joints.push_back   (root_id);
            v.m_weights.push_back  (1.0f);
            
            skin_verts.m_verts.push_back( v );
          }
        }
      }
    }

    return (total_num_weights != 0);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void Scene::GetInfluentialJoints( S_JointTransform& joints )
  {
    for each (const SkinPtr& skin in m_Skins )
    {
      for each ( const UniqueID::TUID& influenceObject in skin->m_InfluenceObjectIDs )
      {
        JointTransformPtr joint = Get< JointTransform >( influenceObject );
        if ( joint.ReferencesObject() )
        {
          joints.insert( joint );
        }
      }
    }
  }

  u32 Scene::GetHierarchyNodeDepth( const HierarchyNodePtr& node ) const
  {
    u32 depth = 0;

    UniqueID::TUID parentId = node->m_ParentID;
    while ( parentId != UniqueID::TUID::Null )
    {
      HierarchyNodePtr parentNode = Get< HierarchyNode >( parentId );

      if ( parentNode.ReferencesObject() )
      {
        depth++;

        parentId = parentNode->m_ParentID;
      }
      else
      {
        parentId = UniqueID::TUID::Null;
      }
    }

    return depth;
  }

  void Scene::CalculateJointOrdering()
  {
    m_JointOrdering = new JointOrdering;

    S_JointTransform requiredJoints;

    // add any joint that influences the skin in any way
    GetInfluentialJoints( requiredJoints );

    // Get any effectors/locators, and force their parents to be required
    V_CollisionPrimitive primitives;
    GetAll<CollisionPrimitive>( primitives );
    for each ( const CollisionPrimitivePtr& prim in primitives )
    {
      if ( prim->m_ParentID == UniqueID::TUID::Null )
      {
        continue;
      }

      JointTransformPtr parentJoint = Get< JointTransform >( prim->m_ParentID );

      if ( parentJoint.ReferencesObject() )
      {
        requiredJoints.insert( parentJoint );
      }
    }

    V_Effector effectors;
    GetAll<Effector>( effectors );
    for each ( const EffectorPtr& effector in effectors )
    {
      if ( effector->m_ParentID == UniqueID::TUID::Null )
      {
        continue;
      }

      JointTransformPtr parentJoint = Get< JointTransform >( effector->m_ParentID );

      if ( parentJoint.ReferencesObject() )
      {
        requiredJoints.insert( parentJoint );
      }
    }

    for each ( const JointTransformPtr& joint in m_Joints )
    {
      // fill out the default mapping (this joint id->this joint id)
      m_JointOrdering->m_MasterToLocalMap.insert( std::make_pair( joint->m_ID, joint->m_ID ) );
      m_JointOrdering->m_LocalToMasterMap.insert( std::make_pair( joint->m_ID, joint->m_ID ) );

      requiredJoints.insert( joint );

      // add the parent hierarchy of any joint that is required
      if ( requiredJoints.find( joint ) != requiredJoints.end() )
      {
        UniqueID::TUID parentId = joint->m_ParentID;
        while ( parentId != UniqueID::TUID::Null )
        {
          JointTransformPtr parentJoint = Get< JointTransform >( parentId );

          if ( parentJoint.ReferencesObject() )
          {
            requiredJoints.insert( parentJoint );

            parentId = parentJoint->m_ParentID;
          }
          else
          {
            parentId = UniqueID::TUID::Null;
          }
        }
      }
    }

    V_JointTransform sortedJoints;
    sortedJoints.insert( sortedJoints.begin(), requiredJoints.begin(), requiredJoints.end() );

    std::sort( sortedJoints.begin(), sortedJoints.end(), HierarchySort( *this ) );

    // now add each of the required joints to the joint ordering
    for each ( const JointTransformPtr& joint in sortedJoints )
    {
      m_JointOrdering->AddJoint( joint->m_ID );
    }

    for each ( const JointTransformPtr& joint in m_Joints )
    {
      if ( requiredJoints.find( joint ) == requiredJoints.end() )
      {
        Console::Debug( Console::Levels::Extreme, "Joint not required: %s\n", joint->GetName().c_str() );
      }
    }
  }

  void Scene::GetAlignedBoundingBox( Math::AlignedBox& box ) const
  {
    for each ( const MeshPtr& mesh in m_Meshes )
    {
      AlignedBox meshBox;
      mesh->GetAlignedBoundingBox( meshBox );
      box.Merge( meshBox );
    }
  }

  typedef std::multimap< UniqueID::TUID, MeshPtr > M_ShaderMesh;

  void Scene::MergeMeshes()
  {
    M_ShaderMesh meshesByShader;
   
    for each( const MeshPtr& mesh in m_Meshes )
    { 
      for each( const UniqueID::TUID& shaderID in mesh->m_ShaderIDs )
      {
        M_ShaderMesh::iterator itor = meshesByShader.lower_bound( shaderID );
        M_ShaderMesh::iterator upper = meshesByShader.upper_bound( shaderID );
        
        if( itor == upper )
        {
          std::string idStr;
          shaderID.ToString(idStr);
          MeshPtr newMesh = new Mesh( mesh->m_ID );
          newMesh->m_DefaultName = idStr;
          newMesh->m_ShaderIDs.push_back( shaderID );
          newMesh->m_ExportTypes = mesh->m_ExportTypes;
          newMesh->m_ParentID = mesh->m_ParentID;
          newMesh->m_ShaderTriangleCounts.resize( 1 );

          meshesByShader.insert( M_ShaderMesh::value_type( shaderID, newMesh ) );          
        }

        itor = meshesByShader.lower_bound( shaderID );
        upper = meshesByShader.upper_bound( shaderID );

        bool foundMatch = false;
        for( ; itor != upper; ++itor )
        {
          if( itor->second->m_ExportTypes == mesh->m_ExportTypes )
          {
            foundMatch = true;

            Mesh& destMesh = *itor->second;
            u32 numTris = mesh->GetTriangleCount();
            u32 addedTris = 0;
            u32 triOffset = destMesh.GetTriangleCount();
            for( u32 triIdx = 0; triIdx < numTris; ++triIdx )
            {
              if( mesh->m_ShaderIDs[ mesh->m_ShaderIndices[triIdx] ] == shaderID )
              {
                ++addedTris;
                u32 vertIdxOffset = triIdx * 3;
               
                for( u32 i = 0; i < 3; ++i )
                {
                  u32 vertIndex = mesh->m_TriangleVertexIndices[ vertIdxOffset + i ];

                  destMesh.m_Positions.push_back( mesh->m_Positions[vertIndex] );
                  destMesh.m_Normals.push_back( mesh->m_Normals[vertIndex] );
                  destMesh.m_Colors.push_back( mesh->m_Colors[vertIndex] );
                  destMesh.m_BaseUVs.push_back( mesh->m_BaseUVs[vertIndex] );

                  if( mesh->m_LightMapUVs.size() )
                    destMesh.m_LightMapUVs.push_back( mesh->m_LightMapUVs[vertIndex] );

                  if( mesh->m_BlendMapUVs.size() )
                    destMesh.m_BlendMapUVs.push_back( mesh->m_BlendMapUVs[vertIndex] );

                  destMesh.m_TriangleVertexIndices.push_back( (u32)destMesh.m_Positions.size()-1 );
                  destMesh.m_WireframeVertexIndices.push_back( (u32)destMesh.m_Positions.size()-1 );
                }
                
                destMesh.m_ShaderIndices.push_back( 0 );
                destMesh.m_PolygonIndices.push_back( mesh->m_PolygonIndices[ triIdx ] );
              }              
            }
            destMesh.m_ShaderTriangleCounts[0] += addedTris;
            
            break;
          }
        }
      }

      Console::Print( "Removing Mesh: %s\n", mesh->GetName().c_str() );
      Remove( mesh );
    }
      
    m_Meshes.clear();
    m_TypedMeshes.clear();
    //m_Hierarchy.clear();

    M_ShaderMesh::iterator itor = meshesByShader.begin();
    M_ShaderMesh::iterator end = meshesByShader.end();
    for( ; itor != end; ++itor )
    {
      Console::Print( "Adding MergedMesh: %s\n", itor->second->GetName().c_str() );
      Add( itor->second );
    }
    Update();
  }

  void Scene::GetMentalRayMeshes( V_Mesh& mentalRayMeshes ) const
  {
    S_Mesh meshes;
    GetMeshesByType( meshes, Content::ContentTypes::Geometry, 0 );   

    for each( const Content::MeshPtr& mesh in meshes )
    {
      if( mesh->m_ExportTypes[Content::ContentTypes::LightMapped] && !mesh->ShareLightmap() )
      {
        mentalRayMeshes.push_back( mesh );
      }
      else if( mesh->m_ExportTypes[Content::ContentTypes::VertexLit] )
      {
        mentalRayMeshes.push_back( mesh );
      }
    }
  }


  void Scene::GetMentalRayMeshes( V_Mesh& lightmapped, V_Mesh& vertexLit ) const
  {
    S_Mesh meshes;
    GetMeshesByType( meshes, Content::ContentTypes::Geometry, 0 );   

    for each( const Content::MeshPtr& mesh in meshes )
    {
      if( mesh->m_ExportTypes[Content::ContentTypes::LightMapped] && !mesh->ShareLightmap() )
      {
        lightmapped.push_back( mesh );
      }
      else if( mesh->m_ExportTypes[Content::ContentTypes::VertexLit] )
      {
        vertexLit.push_back( mesh );
      }
    }
  }

  void Scene::GetMentalRayMeshes( V_Mesh& lightmapped, V_Mesh& vertexLit, V_Mesh& foliageMeshes ) const
  {
    S_Mesh meshes;
    GetMeshesByType( meshes, Content::ContentTypes::Geometry, 0 );   

    for each( const Content::MeshPtr& mesh in meshes )
    {
      if( mesh->m_ExportTypes[Content::ContentTypes::LightMapped] && !mesh->ShareLightmap() )
      {
        lightmapped.push_back( mesh );
      }
      else if( mesh->m_ExportTypes[Content::ContentTypes::VertexLit] )
      {
        vertexLit.push_back( mesh );
      }
      else if( mesh->m_ExportTypes[Content::ContentTypes::Foliage] )
      {
        if( mesh->m_ExportTypeIndex[ Content::ContentTypes::Foliage ] == 0 )
        {
          foliageMeshes.push_back( mesh );
        }
      }
    }
  }

   bool Scene::LightmapUVsExist() const
   {
     for each( const Content::MeshPtr& mesh in m_Meshes )
     {
       if( mesh->m_ExportTypes[Content::ContentTypes::LightMapped] )
       {
         if( !mesh->LightmapUVsExist() )
           return false;
       }
     }
     return true;
   }

   bool Scene::MeshesAreClassifiedForLighting() const
   {
     for each( const Content::MeshPtr& mesh in m_Meshes )
     {
       if( !mesh->m_ExportTypes[Content::ContentTypes::LightMapped] && !mesh->m_ExportTypes[Content::ContentTypes::VertexLit])
       {
         return false;
       }
     }
     return true;
   }

   bool Scene::LightmapUVsInRange() const
   {
     for each( const Content::MeshPtr& mesh in m_Meshes )
     {
       if( mesh->m_ExportTypes[Content::ContentTypes::LightMapped] )
       {
         if( !mesh->LightmapUVsInRange() )
           return false;
       }
     }
     return true;     
   }

   //http://www.geometrictools.com/LibFoundation/Intersection/Intersection.html
   static int WhichSide ( Vector2* akV, const Vector2& rkP, const Vector2& rkD)
   {
     // Vertices are projected to the form P+t*D.  Return value is +1 if all
     // t > 0, -1 if all t < 0, 0 otherwise, in which case the line splits the
     // triangle.

     int iPositive = 0, iNegative = 0, iZero = 0;
     for (int i = 0; i < 3; i++)
     {
       f32 fT = rkD.Dot(akV[i] - rkP);
       if (fT > (f32)0.0)
       {
         iPositive++;
       }
       else if (fT < (f32)0.0)
       {
         iNegative++;
       }
       else
       {
         iZero++;
       }

       if (iPositive > 0 && iNegative > 0)
       {
         return 0;
       }
     }
     return (iZero == 0 ? (iPositive > 0 ? 1 : -1) : 0);
   }

   //http://www.geometrictools.com/LibFoundation/Intersection/Intersection.html
   static bool TriTri( Vector2* t1, Vector2* t2 )
   {
     int i0, i1;
     Vector2 kDir;

     int numZero1 = 0;
     int numThree1 = 0;
     // test edges of triangle0 for separation
     for (i0 = 0, i1 = 2; i0 < 3; i1 = i0, i0++)
     {
       // test axis V0[i1] + t*perp(V0[i0]-V0[i1]), perp(x,y) = (y,-x)
       kDir.x = t1[i0].y - t1[i1].y;
       kDir.y = t1[i1].x - t1[i0].x;

       int side = WhichSide(t2,t1[i1],kDir);
       if ( side == 1)
       {
         // triangle1 is entirely on positive side of triangle0 edge
         return false;
       }
     }

     int numZero2 = 0;
     int numThree2 = 0;
     // test edges of triangle1 for separation
     for (i0 = 0, i1 = 2; i0 < 3; i1 = i0, i0++)
     {
       // test axis V1[i1] + t*perp(V1[i0]-V1[i1]), perp(x,y) = (y,-x)
       kDir.x = t2[i0].y - t2[i1].y;
       kDir.y = t2[i1].x - t2[i0].x;
       
       int side = WhichSide(t1,t2[i1],kDir);
       if ( side == 1)
       {
         // triangle0 is entirely on positive side of triangle1 edge
         return false;
       }
     }
     return true;
   }
   

   struct Sphere
   {

     f32 m_Radius;
     Vector2 m_Center;
   };

   static void GatherBSpheres( std::vector< Sphere >& spheres, const std::vector< V_Vector2 >& uvShells  )
   {
     spheres.resize( uvShells.size() );

     V_UVShell::const_iterator itor = uvShells.begin();
     V_UVShell::const_iterator end  = uvShells.end();

     std::vector< Sphere >::iterator sphereItor = spheres.begin();
   
     // for each shell
     for( ; itor != end; ++itor, ++sphereItor )
     {
       // for each triangle  
       Vector2 max( (*itor)[0] );
       Vector2 min( (*itor)[0] );

       int size = (int)itor->size();
       for( int i = 1; i < size; ++i )
       {        
         if( (*itor)[i].x > max.x )
           max.x = (*itor)[i].x;
         if( (*itor)[i].y > max.y )
           max.y = (*itor)[i].y;

         if( (*itor)[i].x < min.x )
           min.x = (*itor)[i].x;
         if( (*itor)[i].y < min.y )
           min.y = (*itor)[i].y;
           
       }
       Vector2 diff = max - min;
       sphereItor->m_Center = (diff * 0.5f) + min;
       sphereItor->m_Radius = diff.Length() * 0.5f;

     }
   }
   static bool SphereIntersect( const Sphere& sphere1, const Sphere& sphere2)
   {
     Vector2 diff = sphere2.m_Center - sphere1.m_Center;

     if( diff.Length() < sphere2.m_Radius + sphere1.m_Radius )
       return true;

     return false;
   }

   static void FindBroadPhaseIntersections( const V_UVShell& uvShells, std::map< u32, std::set< u32 > >& toCheck )
   {
     std::vector< Sphere > spheres;
     GatherBSpheres( spheres, uvShells );

     std::vector< Sphere >::iterator sphereItor = spheres.begin();
     std::vector< Sphere >::iterator sphereEnd  = spheres.end();

     for( ; sphereItor != sphereEnd; ++sphereItor )
     {
       std::vector< Sphere >::iterator sphereInsideItor = spheres.begin();

       for( ; sphereInsideItor != sphereEnd; ++sphereInsideItor )
       {
         if( sphereInsideItor == sphereItor )
         {
           toCheck[ (u32)( sphereItor - spheres.begin() ) ].insert( (u32)( sphereInsideItor - spheres.begin() ) );
           continue;
         }
         if( SphereIntersect( *sphereItor, *sphereInsideItor ) )
         {
           toCheck[ (u32)( sphereItor - spheres.begin() ) ].insert( (u32)( sphereInsideItor - spheres.begin() ) );
         }        
       }
     }
   }

   // brute force tri-tri intersection tests
   bool Scene::LightmapUVsOverlap( std::map< u32, std::set<u32> >& overlap ) const
   {

     V_UVShell uvShells;
     std::vector< V_u32 > triangleIndices;

     GetUVShells( UVSetTypes::Lightmap, uvShells, triangleIndices );

     std::map< u32, std::set< u32 > > toCheck;
     FindBroadPhaseIntersections( uvShells, toCheck );

     V_UVShell::iterator itor = uvShells.begin();
     V_UVShell::iterator end  = uvShells.end();

     std::vector< V_u32 >::iterator indicesItor = triangleIndices.begin();

     // for each shell
     for( ; itor != end; ++itor, ++indicesItor )
     {
       u32 shellIndex = u32(itor - uvShells.begin());

       // for each shell that we haven't already checked
       V_UVShell::iterator itorInside = uvShells.begin() + shellIndex;      
       std::vector< V_u32 >::iterator indicesInsideItor = triangleIndices.begin() + shellIndex; 

       for( ; itorInside != end; ++itorInside, ++indicesInsideItor )
       {
         bool check = true;
         if( itor != itorInside )
         {
           std::set< u32 >& set = toCheck[ u32( itor - uvShells.begin() ) ];
           std::set< u32 >::iterator findItor = set.find( u32( itorInside - uvShells.begin() ) );
           if( findItor == set.end() )
             check = false;          
         }
         if( !check )
           continue;

         // for each triangle        
         int size = (int)indicesItor->size();
#pragma omp parallel
         for( int i = 0; i < size; i+=3 )
         {        
           Vector2 tri1[3] = { (*itor)[(*indicesItor)[i]]   * 100.f, 
             (*itor)[(*indicesItor)[i+1]] * 100.f, 
             (*itor)[(*indicesItor)[i+2]] * 100.f };

           Vector2 center = tri1[0] + tri1[1] + tri1[2];
           center /= 3.0f;

           tri1[0] -= center;
           tri1[1] -= center;
           tri1[2] -= center;

           tri1[0] *= 0.99f;
           tri1[1] *= 0.99f;
           tri1[2] *= 0.99f;

           tri1[0] += center;
           tri1[1] += center;
           tri1[2] += center;

           // for each triangle        
           int insideSize = (int)indicesInsideItor->size();

           for( int j = 0; j < insideSize; j+=3 )
           {
             // skip if we are comparing the same triangle
             if( itor == itorInside )
             {
               if( i == j )
                 continue;
             }

             Vector2 tri2[3] = {(*itorInside)[(*indicesInsideItor)[j]] * 100.f, 
                                (*itorInside)[(*indicesInsideItor)[j+1]] * 100.f, 
                                (*itorInside)[(*indicesInsideItor)[j+2]] * 100.f };

             if( TriTri( tri1, tri2 ) )
             {
#pragma omp critical
               {
                 overlap[ u32( itor - uvShells.begin() ) ].insert( i/3 );
                 overlap[ u32( itorInside - uvShells.begin() ) ].insert( j/3 );
               }
             }
           }
         }  
       }
     }
     return !overlap.empty();
   }

   // brute force tri-tri intersection tests
   bool Scene::LightmapUVsOverlap() const
   {
     V_UVShell uvShells;
     std::vector< V_u32 > triangleIndices;

     GetUVShells( UVSetTypes::Lightmap, uvShells, triangleIndices );

     std::map< u32, std::set< u32 > > toCheck;
     FindBroadPhaseIntersections( uvShells, toCheck );

     V_UVShell::iterator itor = uvShells.begin();
     V_UVShell::iterator end  = uvShells.end();

     std::vector< V_u32 >::iterator indicesItor = triangleIndices.begin();

     bool overlap = false;
     // for each shell
     for( ; itor != end; ++itor, ++indicesItor )
     {
       if( overlap )
         return true;

       u32 shellIndex = u32(itor - uvShells.begin());

       // for each shell that we haven't already checked
       V_UVShell::iterator itorInside = uvShells.begin() + shellIndex;      
       std::vector< V_u32 >::iterator indicesInsideItor = triangleIndices.begin() + shellIndex; 

       for( ; itorInside != end; ++itorInside, ++indicesInsideItor )
       {
         if( overlap )
           return true;

         bool check = true;
         if( itor != itorInside )
         {
           std::set< u32 >& set = toCheck[ u32( itor - uvShells.begin() ) ];
           std::set< u32 >::iterator findItor = set.find( u32( itorInside - uvShells.begin() ) );
           if( findItor == set.end() )
             check = false;          
         }
         if( !check )
           continue;

         // for each triangle        
         int size = (int)indicesItor->size();
#pragma omp parallel
         for( int i = 0; i < size; i+=3 )
         { 
#pragma omp flush(overlap )
           if( overlap )
             continue;

           Vector2 tri1[3] = { (*itor)[(*indicesItor)[i]]   * 100.f, 
                               (*itor)[(*indicesItor)[i+1]] * 100.f, 
                               (*itor)[(*indicesItor)[i+2]] * 100.f };

           // shrink the tri slightly, so colinear edges and coincident verts don't cause intersections.
           Vector2 center = tri1[0] + tri1[1] + tri1[2];
           center /= 3.0f;

           tri1[0] -= center;
           tri1[1] -= center;
           tri1[2] -= center;

           tri1[0] *= 0.99f;
           tri1[1] *= 0.99f;
           tri1[2] *= 0.99f;

           tri1[0] += center;
           tri1[1] += center;
           tri1[2] += center;

           // for each triangle        
           int insideSize = (int)indicesInsideItor->size();

           for( int j = 0; j < insideSize; j+=3 )
           {
             // skip if we are comparing the same triangle
             if( itor == itorInside )
             {
               if( i == j )
                 continue;
             }

             Vector2 tri2[3] = {(*itorInside)[(*indicesInsideItor)[j]] * 100.f, 
                                (*itorInside)[(*indicesInsideItor)[j+1]] * 100.f, 
                                (*itorInside)[(*indicesInsideItor)[j+2]] * 100.f };

             if( TriTri( tri1, tri2 ) )
             {
               overlap = true;   
             }
           }
         }  
       }
     }
     return overlap;
   }

   // copy all specified uv's and indices into passed in containers
   void Scene::GetUVShells( UVSetType set, V_UVShell& uvShells, std::vector< V_u32 >& triangleIndices ) const
   {
     Content::V_Mesh::const_iterator itor = m_Meshes.begin();
     Content::V_Mesh::const_iterator end  = m_Meshes.end();
     for ( ; itor != end; ++itor )
     {
       V_Vector2* uvSet = NULL;
       if( set == UVSetTypes::Base )
       {
         uvSet = &(*itor)->m_BaseUVs;
       }
       else if( set == UVSetTypes::Blend )
       {
         uvSet = &(*itor)->m_BlendMapUVs;
       }
       else if( set == UVSetTypes::Lightmap )
       {
         if( (*itor)->m_ExportTypes[Content::ContentTypes::LightMapped] )
         {
           uvSet = &(*itor)->m_LightMapUVs;
         }
       }

       if( uvSet == NULL )
         continue;

       if( !uvSet->empty() )
       {
         uvShells.push_back( Math::V_Vector2() );
         triangleIndices.push_back( V_u32() );

         uvShells.back() = *uvSet;
         triangleIndices.back() = (*itor)->m_TriangleVertexIndices;
       }
     }
   }

   f32 Scene::AvgLightmapTexelsPerMeter( u32 logTextureSize, Math::Scale* scale ) const
   {
     f32 texelsPerMeter = 0.0f;
     u32 numMeshes = 0;

     int size = (int)m_Meshes.size();

     #pragma omp parallel for reduction(+:texelsPerMeter,numMeshes)
     for ( int i = 0; i < size; ++i )
     {
       Mesh* mesh = NULL;

#pragma omp critical
       {
         mesh = m_Meshes[i];
       }

       if( mesh->m_ExportTypes[Content::ContentTypes::LightMapped] && !mesh->ShareLightmap() )
       {
         texelsPerMeter += mesh->LightmapTexelsPerMeter( logTextureSize, scale );
         numMeshes +=1;
       }
     }
     return texelsPerMeter / numMeshes;
   }

   f32 Scene::LightmapTexelsPerMeter( u32 logTextureSize, Math::Scale* scale ) const
   {    
     f32 surfaceArea = 0.0f;
     f32 uvArea      = 0.0f;

     int size = (int)m_Meshes.size();

#pragma omp parallel for reduction(+:surfaceArea,uvArea)
     for ( int i = 0; i < size; ++i )
     {
       Mesh* mesh = NULL;

#pragma omp critical
       {
         mesh = m_Meshes[i];
       }

       if( mesh->m_ExportTypes[Content::ContentTypes::LightMapped] && !mesh->ShareLightmap() )
       {
         surfaceArea += mesh->SurfaceArea( scale );
         uvArea += mesh->UVSurfaceArea( UVSetTypes::Lightmap );
       }
     }
     u32 textureSize = 1 << logTextureSize;

     return sqrt( uvArea * textureSize * textureSize / surfaceArea );
   }

   f32 Scene::LightmapTexelsPerMeter( u32 logTextureSize, f32 uvArea ) const
   {    
     f32 surfaceArea = 0.0f;
     int size = (int)m_Meshes.size();

#pragma omp parallel for reduction(+:surfaceArea,uvArea)
     for ( int i = 0; i < size; ++i )
     {
       Mesh* mesh = NULL;

#pragma omp critical
       {
         mesh = m_Meshes[i];
       }

       if( mesh->m_ExportTypes[Content::ContentTypes::LightMapped] && !mesh->ShareLightmap() )
       {
         surfaceArea += mesh->SurfaceArea();
       }
     }
     u32 textureSize = 1 << logTextureSize;

     return sqrt( uvArea * textureSize * textureSize / surfaceArea );
   }

   f32 Scene::UVSurfaceArea( UVSetType uvSetType ) const
   {
     f32 surfaceArea = 0.0f;

     int size = (int)m_Meshes.size();
#pragma omp parallel for reduction(+:surfaceArea)
     for ( int i = 0; i < size; ++i )
     {
       Mesh* mesh = NULL;

#pragma omp critical
       {
         mesh = m_Meshes[i];
       }

       if( uvSetType == UVSetTypes::Lightmap )
       {
         if( !mesh->m_ExportTypes[Content::ContentTypes::LightMapped] && !mesh->ShareLightmap() )
           continue;
       }
       surfaceArea += mesh->UVSurfaceArea( uvSetType );
     }
     return surfaceArea;
   }

   f32 Scene::MeshSurfaceArea( Content::ContentType meshType, Math::Scale* scale ) const
   {
     f32 surfaceArea = 0.0f;

     int size = (int)m_Meshes.size();
#pragma omp parallel for reduction(+:surfaceArea)
     for ( int i = 0; i < size; ++i )
     {
       Mesh* mesh = NULL;

#pragma omp critical
       {
         mesh = m_Meshes[i];
       }

       if( mesh->m_ExportTypes[meshType] && !mesh->ShareLightmap() )
         surfaceArea += mesh->SurfaceArea( scale );       
     }
     return surfaceArea;
   }

   f32 Scene::MeshSurfaceAreaComponents( Content::ContentType meshType, Math::Vector3& areaVec ) const
   {
     f32 surfaceArea = 0.0f;

     int size = (int)m_Meshes.size();
     for ( int i = 0; i < size; ++i )
     {
       Mesh* mesh = NULL;

#pragma omp critical
       {
         mesh = m_Meshes[i];
       }

       if( mesh->m_ExportTypes[meshType] && !mesh->ShareLightmap() )
         surfaceArea += mesh->SurfaceAreaComponents( areaVec );       
     }
     return surfaceArea;
   }

   bool Scene::HasSharedLightmapOverlays() const
   {
     Content::V_Mesh::const_iterator itor = m_Meshes.begin();
     Content::V_Mesh::const_iterator end  = m_Meshes.end();
     for ( ; itor != end; ++itor )
     {
       if( (*itor)->ShareLightmap() )
         return true;
     }
     return false;
   }

   bool Scene::IntersectSegment( const Math::Line& segment,  Math::Matrix4* transform )
   {
     V_Mesh::iterator itor = m_Meshes.begin();
     V_Mesh::iterator end  = m_Meshes.end();

     for( ; itor != end; ++itor )
     {
       u32 numTris = (*itor)->GetTriangleCount();
       
       for( u32 i = 0; i < numTris; ++i )
       {
         Math::Vector3 v0, v1, v2;
         (*itor)->GetTriangle( i, v0, v1, v2, transform );

         if( segment.IntersectSegmentTriangle( v0, v1, v2 ) )
         {
           return true;
         }
       }
     }
     return false;
   }

   bool Scene::IntersectRay( const Math::Line& ray,  Math::Matrix4* transform )
   {
     V_Mesh::iterator itor = m_Meshes.begin();
     V_Mesh::iterator end  = m_Meshes.end();

     for( ; itor != end; ++itor )
     {
       u32 numTris = (*itor)->GetTriangleCount();

       for( u32 i = 0; i < numTris; ++i )
       {
         Math::Vector3 v0, v1, v2;
         (*itor)->GetTriangle( i, v0, v1, v2, transform );

         if( ray.IntersectRayTriangle( v0, v1, v2 ) )
         {
           return true;
         }
       }
     }
     return false;
   }
/*
   void Scene::UniqueAssetClasses( S_tuid& entityClasses, bool (*verifyFunc)( Asset::Entity& ) ) const
   {
     Asset::V_Entity entities;
     GetAll< Asset::Asset >( entities );


     Asset::V_Entity::iterator itor = entities.begin();
     Asset::V_Entity::iterator end  = entities.end();

     for( ; itor != end; ++itor )
     {
       if( verifyFunc )
       {
         if( verifyFunc( **itor ) )
         {
           entityClasses.insert( (*itor)->GetEntityClassID() );
         }
       }
       else
       {
       entityClasses.insert( (*itor)->GetEntityClassID() );
       }
     }
   }*/


   void Scene::MeshesWithVertDensity( V_Mesh& meshes, f32 density ) const
   {
     Content::V_Mesh::const_iterator itor = m_Meshes.begin();
     Content::V_Mesh::const_iterator end  = m_Meshes.end();
     for ( ; itor != end; ++itor )
     {
       f32 vertDensity = (*itor)->VertDensity();
       if( vertDensity >= density )
       {
         meshes.push_back( *itor );
       }
       
     }     
   }

}

