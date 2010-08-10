#include "Precompile.h"
#include "ExportScene.h"
#include "ExportMesh.h"
#include "ExportAnimationBase.h"
#include "MayaContentCmd.h"

#include "Foundation/Log.h"
#include "Core/Content/Nodes/Curve.h"
#include "Maya/Utils.h"

using namespace Helium;
using namespace Helium::MayaContent;
using namespace Helium::Maya;

static void AddChildrenToSet( MObjectSet& objectSet, MObject object )
{
    objectSet.insert( object );
    MFnDagNode nodeFn( object );
    u32 num = nodeFn.childCount();
    for( u32 j = 0; j < num; ++j )
    {
        AddChildrenToSet( objectSet, nodeFn.child( j ) );
    }
}

void ExportScene::GatherMayaData()
{
    for ( u32 index = 0; index < m_ExportObjects.size(); ++index )
    {
        m_ExportObjects[index]->GatherMayaData( m_ExportObjects );
    }
}

void ExportScene::ProcessMayaData()
{
    V_ExportBase::reverse_iterator itor = m_ExportObjects.rbegin();
    V_ExportBase::reverse_iterator begin = m_ExportObjects.rend();

    for( ; itor != begin; ++itor )
    {
        (*itor)->ProcessMayaData();

        m_ContentScene.Add( (*itor)->GetContentObject() );
    }

    // done adding to the scene, update it.
    m_ContentScene.Update();

    // opimtize will cull extraneous nodes
    m_ContentScene.Optimize();
}

void ExportScene::ExportData()
{
    S_ElementDumbPtr duplicateCheck;

    m_Spool.reserve( m_ContentScene.m_DependencyNodes.size() );

    std::map< Helium::TUID, Content::SceneNodePtr >::iterator itr = m_ContentScene.m_DependencyNodes.begin();
    std::map< Helium::TUID, Content::SceneNodePtr >::iterator end = m_ContentScene.m_DependencyNodes.end();
    for( ; itr != end; ++itr )
    {
        Content::SceneNode* sceneNode = itr->second;
        ExportNode( sceneNode, duplicateCheck );
    }
}

void ExportScene::Add( const ExportBasePtr &node )
{
    m_ExportObjects.push_back( node );
}

void ExportScene::ExportNode( Content::SceneNode* node, S_ElementDumbPtr& duplicateCheck )
{
    Content::HierarchyNode* hierarchyNode = Reflect::ObjectCast< Content::HierarchyNode >( node );

    if ( hierarchyNode && hierarchyNode->m_ParentID != TUID::Null )
    {
        std::map< Helium::TUID, Content::SceneNodePtr >::const_iterator parentItr = m_ContentScene.m_DependencyNodes.find( hierarchyNode->m_ParentID );
        if ( parentItr != m_ContentScene.m_DependencyNodes.end() )
        {
            ExportNode( parentItr->second, duplicateCheck );
        }
    }

    if ( duplicateCheck.insert( node ).second )
    {
        m_Spool.push_back( node );
    }
}
