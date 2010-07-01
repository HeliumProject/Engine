#include "Scene.h"

#include "Foundation/Version.h"
#include "Foundation/Container/Insert.h" 
#include "Foundation/String/Utilities.h"
#include "Foundation/Reflect/ArchiveXML.h"

#include "Foundation/TUID.h"
#include "Foundation/Log.h"

#include "Foundation/Math/EulerAngles.h"
#include "Foundation/Math/AlignedBox.h"
#include "Foundation/Math/Line.h"

#include "Pipeline/Content/ContentVisitor.h" 

#include <algorithm>
#include <omp.h>

using Nocturnal::Insert; 
using namespace Reflect;
using namespace Math;
using namespace Content;
using namespace Asset;

#define MIN_JOINT_SKIN_WEIGHT  0.1f
#define MIN_JOINT_BSPHERE_RAD  0.001f  // 1 mm

Scene::Scene()
: m_MorphTargetData( new MorphTargetData() )
{

}

Scene::Scene( const tstring &filePath )
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

    m_AddedHierarchyNodes.clear();
    m_AnimationClips.clear();
    m_MorphTargetData = new MorphTargetData();

    m_JointBspheres.clear();
    m_Bspheres.clear();

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

    Insert<M_DependencyNode>::Result result = m_DependencyNodes.insert( std::pair<Nocturnal::TUID, SceneNodePtr>( node->m_ID, node ) );

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

                if( node->HasType( Reflect::GetType<Mesh>() ) )
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

void Scene::Load( const tstring &filePath )
{
    Reflect::V_Element elements;
    Load( filePath, elements );
}

void Scene::Load( const tstring &filePath, Reflect::V_Element& elements, Reflect::StatusHandler* status )
{
    m_FilePath = filePath;
    Archive::FromFile( filePath, elements, status );
    PostLoad( elements );
}

void Scene::LoadXML( const tstring& xml, Reflect::V_Element& elements, Reflect::StatusHandler* status )
{
    ArchiveXML::FromString( xml, elements, status );
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

void Scene::Serialize( const tstring& filePath )
{
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

    CalculateJointOrdering();
    m_MorphTargetData->CollateMorphTargets( m_Meshes );
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
            node->m_ParentID = Nocturnal::TUID::Null;
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

void Scene::AddChild( const HierarchyNodePtr& child, const Nocturnal::TUID& parentID )
{
    RemoveFromParent( child );
    child->m_ParentID = parentID;
    m_Hierarchy.insert( std::pair<Nocturnal::TUID, HierarchyNodePtr>( parentID, child ) );
}

void Scene::UpdateHierarchy()
{
    V_HierarchyNode::const_iterator itor = m_AddedHierarchyNodes.begin();
    V_HierarchyNode::const_iterator end = m_AddedHierarchyNodes.end();

    for( ; itor != end; ++itor )
    {
        m_Hierarchy.insert( std::pair<Nocturnal::TUID, HierarchyNodePtr>( (*itor)->m_ParentID, *itor ) );
    }

    // done updating with the current batch of newly added hierarchy nodes...safe to clear it
    m_AddedHierarchyNodes.clear();
}

bool Scene::Exists( const Nocturnal::TUID& id )
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

        if (node->m_ParentID == Nocturnal::TUID::Null || m_DependencyNodes.find( node->m_ParentID ) == m_DependencyNodes.end() )
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

        if ( object->HasType( Reflect::GetType<Mesh>() ) )
        {
            //
            // Transform vertices and normals and reset transform to identity
            //

            MeshPtr mesh = DangerousCast< Mesh >( object );

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
    Nocturnal::V_TUID jointIDs;
    clip->GetJointIDs( jointIDs );

    for each ( const Nocturnal::TUID& uid in jointIDs )
    {
        joints.push_back( Get<JointTransform>( uid ) );
    }
}

const JointOrderingPtr& Scene::GetJointOrdering() const
{
    return m_JointOrdering;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::CalculateJointBoundingVolumes()
{
    std::map<Nocturnal::TUID, Math::V_Vector3>      joint_verts_map;
    Math::V_Vector3                                 verts_map;

    // 
    // grab verts for each content type, partitioned by (joint uid, mesh export type id)
    //                                   partitioned by (mesh export type)
    // 
    for each (const SkinPtr& skin in m_Skins )
    {
        MeshPtr mesh = Get< Mesh >(skin->m_Mesh);

        if ( !mesh.ReferencesObject() )
        {
            continue;
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
                if (weight < MIN_JOINT_SKIN_WEIGHT)
                {
                    continue;
                }

                const Nocturnal::TUID&  joint_id  = skin->m_InfluenceObjectIDs[influence->m_Objects[inf]];

                joint_verts_map[joint_id].push_back(pos);
            }

            verts_map.push_back(pos);
        }      
    }


    // 
    // joints
    // 
    std::map<Nocturnal::TUID, Math::V_Vector3>::iterator start = joint_verts_map.begin();
    std::map<Nocturnal::TUID, Math::V_Vector3>::iterator end   = joint_verts_map.end();

    for(std::map<Nocturnal::TUID, Math::V_Vector3>::iterator i = start; i != end; ++i)
    {
        const Nocturnal::TUID&  joint_id = i->first;
        Math::V_Vector3&        joint_verts = i->second;

        Math::BoundingVolumeGenerator           bvg(&joint_verts[0], (int)joint_verts.size(), Math::BoundingVolumeGenerator::BSPHERE_OPTIMIZED);
        Math::BoundingVolumeGenerator::BSphere  bvg_bsphere = bvg.GetPrincipleAxisBoundingSphere();

        if (bvg_bsphere.m_Radius >= MIN_JOINT_BSPHERE_RAD)
        {
            m_JointBspheres[joint_id].push_back(bvg_bsphere);
        }
    }

    //
    // mesh
    //

    Math::BoundingVolumeGenerator           bvg(&verts_map[0], (int)verts_map.size(), Math::BoundingVolumeGenerator::BSPHERE_OPTIMIZED);
    Math::BoundingVolumeGenerator::BSphere  bvg_bsphere = bvg.GetPrincipleAxisBoundingSphere();

    if (bvg_bsphere.m_Radius >= MIN_JOINT_BSPHERE_RAD)
    {
        m_Bspheres.push_back(bvg_bsphere);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Scene::GetBSpheresForJoint( std::vector< Math::BoundingVolumeGenerator::BSphere >& bspheres, const Nocturnal::TUID& jointID ) const
{
    std::map< Nocturnal::TUID, std::vector< Math::BoundingVolumeGenerator::BSphere > >::const_iterator findItor = m_JointBspheres.find( jointID );
    if( findItor == m_JointBspheres.end() )
    {
        return false;
    }

    bspheres = findItor->second;
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Scene::GetBSpheres( std::vector< Math::BoundingVolumeGenerator::BSphere > & bspheres ) const
{
    if (m_Bspheres.empty())
    {
        return false;
    }

    bspheres = m_Bspheres;
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Scene::GetSkinVerts(const std::map<Nocturnal::TUID, u32>& jointUidToId, const Nocturnal::TUID& rootUid, std::vector< std::vector<SkinVertex> >& skinVerts) const
{
    u32                     root_id = jointUidToId.find(rootUid)->second;
    std::vector<SkinVertex> new_verts;
    size_t                  total_num_weights = 0;

    for each (const SkinPtr& skin in m_Skins )
    {
        MeshPtr mesh = Get< Mesh >(skin->m_Mesh);

        if ( !mesh.ReferencesObject() )
        {
            continue;
        }

        u32 skin_verts_id = 0;

        // we dont have this mesh - add it
        skinVerts.push_back(new_verts);

        skin_verts_id = (u32)(skinVerts.size() - 1);
        
        std::vector<SkinVertex>&  skin_verts = skinVerts[skin_verts_id];
        size_t new_skin_verts_start  = skin_verts.size();

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

                SkinVertex v;
                v.m_Position = pos;
                v.m_JointIDs.reserve(num_weights);
                v.m_Joints.reserve   (num_weights);
                v.m_Weights.reserve  (num_weights);

                size_t                num_joint_influences = 0;

                for(size_t inf = 0; inf < num_weights; inf++)
                {
                    const Nocturnal::TUID& joint_id = skin->m_InfluenceObjectIDs[influence->m_Objects[inf]];

                    i32 joint_index = -1;

                    TransformPtr transform = Get< Transform >( joint_id );

                    std::map< Nocturnal::TUID, u32 >::const_iterator indexIt = jointUidToId.find(joint_id);

                    if ( indexIt == jointUidToId.end() )
                    {
                        tstring jointStr;
                        joint_id.ToString( jointStr );
                        continue;
                    }

                    joint_index = indexIt->second;

                    v.m_JointIDs.push_back( joint_id );
                    v.m_Joints.push_back( joint_index );
                    v.m_Weights.push_back( influence->m_Weights[inf] );

                    num_joint_influences++;
                }

                if ( num_joint_influences > 0 )
                {
                    skin_verts.push_back(v);
                    total_num_weights += num_joint_influences;
                }
            }

            if (total_num_weights == pre_total_num_weights)
            {
                //Log::Print( TXT( "*** contentType %d - mesh %3d (bangle_id %3d) has 0 weights\n" ), contentType, skin_verts_id, mesh_id);

                for(size_t ivert = 0; ivert < num_verts; ivert++)
                {
                    SkinVertex v;
                    v.m_JointIDs.push_back(rootUid);
                    v.m_Joints.push_back   (root_id);
                    v.m_Weights.push_back  (1.0f);

                    skin_verts.push_back( v );
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
        for each ( const Nocturnal::TUID& influenceObject in skin->m_InfluenceObjectIDs )
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

    Nocturnal::TUID parentId = node->m_ParentID;
    while ( parentId != Nocturnal::TUID::Null )
    {
        HierarchyNodePtr parentNode = Get< HierarchyNode >( parentId );

        if ( parentNode.ReferencesObject() )
        {
            depth++;

            parentId = parentNode->m_ParentID;
        }
        else
        {
            parentId = Nocturnal::TUID::Null;
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

    for each ( const JointTransformPtr& joint in m_Joints )
    {
        // fill out the default mapping (this joint id->this joint id)
        m_JointOrdering->m_MasterToLocalMap.insert( std::make_pair( joint->m_ID, joint->m_ID ) );
        m_JointOrdering->m_LocalToMasterMap.insert( std::make_pair( joint->m_ID, joint->m_ID ) );

        requiredJoints.insert( joint );

        // add the parent hierarchy of any joint that is required
        if ( requiredJoints.find( joint ) != requiredJoints.end() )
        {
            Nocturnal::TUID parentId = joint->m_ParentID;
            while ( parentId != Nocturnal::TUID::Null )
            {
                JointTransformPtr parentJoint = Get< JointTransform >( parentId );

                if ( parentJoint.ReferencesObject() )
                {
                    requiredJoints.insert( parentJoint );

                    parentId = parentJoint->m_ParentID;
                }
                else
                {
                    parentId = Nocturnal::TUID::Null;
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
            Log::Debug( Log::Levels::Extreme, TXT( "Joint not required: %s\n" ), joint->GetName().c_str() );
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

typedef std::multimap< Nocturnal::TUID, MeshPtr > M_ShaderMesh;

void Scene::MergeMeshes()
{
    M_ShaderMesh meshesByShader;

    for each( const MeshPtr& mesh in m_Meshes )
    { 
        for each( const Nocturnal::TUID& shaderID in mesh->m_ShaderIDs )
        {
            M_ShaderMesh::iterator itor = meshesByShader.lower_bound( shaderID );
            M_ShaderMesh::iterator upper = meshesByShader.upper_bound( shaderID );

            if( itor == upper )
            {
                tstring idStr;
                shaderID.ToString(idStr);
                MeshPtr newMesh = new Mesh( mesh->m_ID );
                newMesh->m_DefaultName = idStr;
                newMesh->m_ShaderIDs.push_back( shaderID );
                newMesh->m_ParentID = mesh->m_ParentID;
                newMesh->m_ShaderTriangleCounts.resize( 1 );

                meshesByShader.insert( M_ShaderMesh::value_type( shaderID, newMesh ) );          
            }

            itor = meshesByShader.lower_bound( shaderID );
            upper = meshesByShader.upper_bound( shaderID );

            if ( itor != meshesByShader.end() )
            {
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
            }
        }

        Log::Print( TXT( "Removing Mesh: %s\n" ), mesh->GetName().c_str() );
        Remove( mesh );
    }

    m_Meshes.clear();

    M_ShaderMesh::iterator itor = meshesByShader.begin();
    M_ShaderMesh::iterator end = meshesByShader.end();
    for( ; itor != end; ++itor )
    {
        Log::Print( TXT( "Adding MergedMesh: %s\n" ), itor->second->GetName().c_str() );
        Add( itor->second );
    }
    Update();
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