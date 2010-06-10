#include "stdafx.h"
#include "ExportScene.h"
#include "MayaContentCmd.h"
#include "MayaUtils/Utils.h"
#include "RCS/RCS.h"

#include "Console/Console.h"

#include "MayaNodes/ExportNode.h"

#include "Finder/Finder.h"
#include "Finder/ContentSpecs.h"

#include "Content/Curve.h"

#include "ExportDescriptor.h"
#include "ExportMesh.h"
#include "ExportAnimationBase.h"

using namespace MayaContent;
using namespace Maya;
using namespace Nocturnal;

static void AddChildrenToSet( S_MObject& objectSet, MObject object )
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
  {
    V_string dup_bangle_errors;
    dup_bangle_errors.reserve(64);
    if (m_ContentScene.HasDuplicateBangleIndexedExportNodes(dup_bangle_errors))
    {
      for (V_string::const_iterator error_it = dup_bangle_errors.begin(); error_it != dup_bangle_errors.end(); ++error_it)
      {
        Console::Error( error_it->c_str());
        MGlobal::displayError(error_it->c_str());
      }
    }
  }
  

  // opimtize will cull extraneous nodes
  m_ContentScene.Optimize();
}

void ExportScene::ExportData()
{
  S_ElementDumbPtr duplicateCheck;

  m_Spool.reserve( m_ContentScene.m_DependencyNodes.size() );

  Content::M_DependencyNode::iterator itr = m_ContentScene.m_DependencyNodes.begin();
  Content::M_DependencyNode::iterator end  = m_ContentScene.m_DependencyNodes.end();
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

  if ( hierarchyNode && hierarchyNode->m_ParentID != UID::TUID::Null )
  {
    Content::M_DependencyNode::const_iterator parentItr = m_ContentScene.m_DependencyNodes.find( hierarchyNode->m_ParentID );
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
