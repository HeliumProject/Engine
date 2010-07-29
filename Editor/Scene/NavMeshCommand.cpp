#include "Precompile.h"
#include "NavMeshCommand.h"
#include "NavMesh.h"

#include "Scene.h"
#include "SceneManager.h"
#include "NavMeshCreateTool.h"

using namespace Editor;

///////////////////////////////////////////////////////////////////////////////
// Constructor 
// 
AddNavMeshCommand::AddNavMeshCommand( Editor::NavMeshPtr mesh, const Math::V_Vector3& positions, u32 mode )
: m_Mesh( mesh )
, m_OldPositions( positions )
, m_Mode( mode )
, m_Merged( false )
{
  HELIUM_ASSERT( m_Mesh );
 
  m_SelectedVerts = m_Mesh->m_selected_verts;
  m_SelectedTris = m_Mesh->m_selected_tris;
  m_SelectedEdge = m_Mesh->m_selected_edge;
  m_Mesh->GetMeshData( m_Positions, m_TriangleVertexIndices, m_WireframeVertexIndices );

  switch( m_Mode )
  {
  case kAddVert:
    {
      HELIUM_ASSERT( positions.size() == 1 );
      // We actually do the add in here
      if ( m_Mesh->m_mouse_over_vert != 0xFFFFFFFF )
      {
        // we are moused over a vert so use that one instead of making a new one
        Math::Vector3 vert = m_Mesh->GetVertPosition( m_Mesh->m_mouse_over_vert );
        m_Mesh->m_selected_verts.insert( m_Mesh->m_selected_verts.begin(), m_Mesh->m_mouse_over_vert );
        m_Mesh->m_selected_verts.pop_back();
        m_TriAdded = m_Mesh->m_selected_verts;
        m_Mesh->AddTri();
        m_Merged = true;
      }
      else
      {
        m_Mesh->AddVert( positions.front() );
      }
    }
    break;
  case kDeleteVerts:
    {
      HELIUM_ASSERT( positions.size() > 0 );
      Math::V_Vector3::iterator it;
      for ( it = m_OldPositions.begin(); it != m_OldPositions.end(); it++ )
      {
        m_Mesh->DeleteVert( *it );
      }
      m_Mesh->ClearEditingState();
    }
    break;
  case kDeleteTris:
    {
      if ( !m_SelectedTris.empty() )
      {
        m_Mesh->m_to_be_deleted_tris = m_SelectedTris;
        m_Mesh->DeleteTrisInQueue();
        m_Mesh->ClearEditingState();
      }
    }
    break;
  case kDeleteEdges:
    {
      if ( m_SelectedEdge != 0xFFFFFFFF )
      {
        m_Mesh->DeleteEdgesInQueue();
        m_Mesh->ClearEditingState();
      }
    }
    break;
  case kModify:
    {
      Editor::NavMeshCreateTool* navMeshCreate;
      navMeshCreate = static_cast<NavMeshCreateTool*>( m_Mesh->GetScene()->GetTool().Ptr() );
      m_ManipulatorPos = navMeshCreate->GetManipulatorPos();
      m_RotationStartValue = navMeshCreate->GetRotationStart();

      // Moving happens during mouse move so here we just store the original position to restore to
      m_SelectedVerts = m_Mesh->m_selected_verts;
      for ( u32 i = 0; i < m_SelectedVerts.size(); i++ )
      {
        Math::Vector3 vert_position = m_Mesh->GetVertPosition( m_SelectedVerts.at(i) );
        m_OldPositions.push_back( vert_position );
      }
      // at this point the positions in m_OldPositions should correspond the the vert indicies stored in m_SelectedVerts
    }
    break;
  case kSnapMergeToNearestVert:
    {
      m_Mesh->MergeVertToClosest(m_SelectedVerts[0]);
      m_Mesh->ClearEditingState();
    }
    break;
  case kAddTri:
    {
      m_Mesh->AddTri();
      m_Mesh->GetScene()->Execute( true );
    }
    break;
  case kPaste:
    {
      m_Mesh->ClearEditingState();
      std::vector< u32 > selected_verts;

      for ( u32 i = 0; i < m_OldPositions.size(); i++ )
      {
        m_Mesh->AddVert( m_OldPositions.at(i) );
        selected_verts.push_back( m_Mesh->m_selected_verts.front() );
      }
      m_Mesh->m_selected_verts = selected_verts;
      m_Mesh->GetScene()->Execute( true );
    }
    break;
  }
}

AddNavMeshCommand::AddNavMeshCommand( Editor::NavMeshPtr mesh, const Math::Matrix4& cube_transform,  const Math::Matrix4& cube_inv_transform, u32 mode )
: m_Mesh( mesh )
, m_Mode( mode )
, m_Merged( false )
{
  HELIUM_ASSERT( m_Mesh );

  m_SelectedVerts = m_Mesh->m_selected_verts;
  m_SelectedTris = m_Mesh->m_selected_tris;
  m_SelectedEdge = m_Mesh->m_selected_edge;
  m_Mesh->GetMeshData( m_Positions, m_TriangleVertexIndices, m_WireframeVertexIndices );
  switch( m_Mode )
  {
  case kPunchCubeHole:
    {
      m_CubeTransform = cube_transform;
      m_CubeInvTransform = cube_inv_transform;
      m_Mesh->PunchCubeHole(m_CubeTransform, m_CubeInvTransform, 0.01f);
      m_Mesh->ClearEditingState();
    }
    break;
  }
}

void AddNavMeshCommand::Undo()
{
  switch( m_Mode )
  {
  case kAddVert:
    {
      if (! m_Merged )
      {
        m_Mesh->ClearEditingState();
      }
      m_Mesh->UpdateMeshData( m_Positions, m_TriangleVertexIndices, m_WireframeVertexIndices );
      m_Mesh->m_selected_verts = m_SelectedVerts;
      m_Mesh->m_mouse_over_tri = 0xFFFFFFFF;
      m_Mesh->m_selected_tris = m_SelectedTris;
      m_Mesh->m_selected_edge = m_SelectedEdge;

    }
    break;
  case kDeleteVerts:
    {
      m_Mesh->UpdateMeshData( m_Positions, m_TriangleVertexIndices, m_WireframeVertexIndices );
      m_Mesh->m_selected_verts = m_SelectedVerts;
      m_Mesh->m_mouse_over_tri = 0xFFFFFFFF;
      m_Mesh->m_selected_tris = m_SelectedTris;
      m_Mesh->m_selected_edge = m_SelectedEdge;
    }
    break;
  case kDeleteTris:
    {
      if ( !m_SelectedTris.empty() )
      {
        m_Mesh->UpdateMeshData( m_Positions, m_TriangleVertexIndices, m_WireframeVertexIndices );
        m_Mesh->m_selected_verts = m_SelectedVerts;
        m_Mesh->m_mouse_over_tri = 0xFFFFFFFF;
        m_Mesh->m_selected_tris = m_SelectedTris;
        m_Mesh->m_selected_edge = m_SelectedEdge;
      }
    }
    break;
  case kDeleteEdges:
    {
      m_Mesh->UpdateMeshData( m_Positions, m_TriangleVertexIndices, m_WireframeVertexIndices );
      m_Mesh->m_selected_verts = m_SelectedVerts;
      m_Mesh->m_mouse_over_tri = 0xFFFFFFFF;
      m_Mesh->m_selected_tris = m_SelectedTris;
      m_Mesh->m_selected_edge = m_SelectedEdge;
    }
    break;
  case kModify:
    {
      Math::Matrix4 tempPos;
      Editor::NavMeshCreateTool* navMeshCreate;
      navMeshCreate = static_cast<NavMeshCreateTool*>( m_Mesh->GetScene()->GetTool().Ptr() );
      tempPos = navMeshCreate->GetManipulatorPos();
      navMeshCreate->SetManipulatorPos( m_ManipulatorPos );
      m_ManipulatorPos = tempPos;

      Math::Vector3 tempVal = navMeshCreate->GetRotationStart();
      navMeshCreate->SetRotationStart( m_RotationStartValue );
      m_RotationStartValue = tempVal;

      // keep the current positions for redo
      Math::V_Vector3 positions;

      m_Mesh->ClearEditingState();
      m_Mesh->m_selected_verts = m_SelectedVerts;
      m_Mesh->m_mouse_over_tri = 0xFFFFFFFF;
      m_Mesh->m_selected_tris = m_SelectedTris;
      m_Mesh->m_selected_edge = m_SelectedEdge;
      for ( u32 i = 0; i < m_SelectedVerts.size(); i++ )
      {
        Math::Vector3 current_position = m_Mesh->GetVertPosition( m_SelectedVerts.at(i) );
        positions.push_back( current_position );

        Math::Vector3 vert_position = m_OldPositions.at(i);
        m_Mesh->UpdateVert( m_SelectedVerts.at(i), vert_position );
      }
      m_OldPositions = positions;
      m_Mesh->GetScene()->Execute( true );
    }
    break;
  case kSnapMergeToNearestVert:
    {
      m_Mesh->UpdateMeshData( m_Positions, m_TriangleVertexIndices, m_WireframeVertexIndices );
      m_Mesh->m_selected_verts = m_SelectedVerts;
      m_Mesh->m_mouse_over_tri = 0xFFFFFFFF;
      m_Mesh->m_selected_tris = m_SelectedTris;
      m_Mesh->m_selected_edge = m_SelectedEdge;
    }
    break;
  case kAddTri:
    {
      m_Mesh->UpdateMeshData( m_Positions, m_TriangleVertexIndices, m_WireframeVertexIndices );
      m_Mesh->m_selected_verts = m_SelectedVerts;
      m_Mesh->m_mouse_over_tri = 0xFFFFFFFF;
      m_Mesh->m_selected_tris = m_SelectedTris;
      m_Mesh->m_selected_edge = m_SelectedEdge;
    }
    break;
  case kPunchCubeHole:
    {
      m_Mesh->UpdateMeshData( m_Positions, m_TriangleVertexIndices, m_WireframeVertexIndices );
      m_Mesh->m_selected_verts = m_SelectedVerts;
      m_Mesh->m_mouse_over_tri = 0xFFFFFFFF;
      m_Mesh->m_selected_tris = m_SelectedTris;
      m_Mesh->m_selected_edge = m_SelectedEdge;
    }
    break;
  case kPaste:
    {
      m_Mesh->UpdateMeshData( m_Positions, m_TriangleVertexIndices, m_WireframeVertexIndices );
      m_Mesh->m_selected_verts = m_SelectedVerts;
      m_Mesh->m_mouse_over_tri = 0xFFFFFFFF;
      m_Mesh->m_selected_tris = m_SelectedTris;
      m_Mesh->m_selected_edge = m_SelectedEdge;
    }
    break;
  }

  if ( !m_Mesh->GetScene()->GetManager()->GetCurrentScene()->GetTool() || m_Mesh->GetScene()->GetManager()->GetCurrentScene()->GetTool()->GetType() != Reflect::GetType<Editor::NavMeshCreateTool>() )
  {
    m_Mesh->ClearEditingState();
  }
}

void AddNavMeshCommand::Redo()
{
  switch( m_Mode )
  {
  case kAddVert:
    {
      if ( m_Merged )
      {
        m_Mesh->ClearEditingState();
        m_Mesh->m_selected_verts = m_TriAdded;
        m_Mesh->AddTri();
      }
      else
      {
        m_Mesh->ClearEditingState();
        m_Mesh->m_selected_verts = m_SelectedVerts;
        m_Mesh->m_mouse_over_tri = 0xFFFFFFFF;
        m_Mesh->m_selected_tris = m_SelectedTris;
        m_Mesh->m_selected_edge = m_SelectedEdge;
        m_Mesh->AddVert( m_OldPositions.front() );
      }
    }
    break;
  case kDeleteVerts:
    {
      Math::V_Vector3::iterator it;
      for ( it = m_OldPositions.begin(); it != m_OldPositions.end(); it++ )
      {
        m_Mesh->DeleteVert( *it );
      }
      m_Mesh->ClearEditingState();
    }
    break;
  case kDeleteTris:
    {
      if ( !m_SelectedTris.empty() )
      {
        m_Mesh->m_to_be_deleted_tris = m_SelectedTris;
        m_Mesh->DeleteTrisInQueue();
      }
    }
    break;
  case kDeleteEdges:
    {
      if ( m_SelectedEdge != 0xFFFFFFFF )
      {
        m_Mesh->DeleteEdgesInQueue();
        m_Mesh->ClearEditingState();
      }
    }
    break;
  case kModify:
    {
      Math::Matrix4 tempPos;
      Editor::NavMeshCreateTool* navMeshCreate;
      navMeshCreate = static_cast<NavMeshCreateTool*>( m_Mesh->GetScene()->GetTool().Ptr() );
      tempPos = navMeshCreate->GetManipulatorPos();
      navMeshCreate->SetManipulatorPos( m_ManipulatorPos );
      m_ManipulatorPos = tempPos;

      Math::V_Vector3 positions;

      m_Mesh->ClearEditingState();
      m_Mesh->m_selected_verts = m_SelectedVerts;
      m_Mesh->m_mouse_over_tri = 0xFFFFFFFF;
      m_Mesh->m_selected_tris = m_SelectedTris;
      m_Mesh->m_selected_edge = m_SelectedEdge;
      for ( u32 i = 0; i < m_SelectedVerts.size(); i++ )
      {
        Math::Vector3 current_position = m_Mesh->GetVertPosition( m_SelectedVerts.at(i) );
        positions.push_back( current_position );

        Math::Vector3 vert_position = m_OldPositions.at(i);
        m_Mesh->UpdateVert( m_SelectedVerts.at(i), vert_position );
      }
      m_OldPositions = positions;
      m_Mesh->GetScene()->Execute( true );
    }
    break;
  case kSnapMergeToNearestVert:
    {
      m_Mesh->MergeVertToClosest(m_SelectedVerts[0]);
      m_Mesh->ClearEditingState();
    }
    break;
  case kAddTri:
    {
      m_Mesh->ClearEditingState();
      m_Mesh->m_selected_verts = m_SelectedVerts;
      m_Mesh->AddTri();
      m_Mesh->GetScene()->Execute( true );
    }
    break;
  case kPunchCubeHole:
    {
      m_Mesh->PunchCubeHole(m_CubeTransform, m_CubeInvTransform, 0.01f);
      m_Mesh->ClearEditingState();
    }
    break;
  case kPaste:
    {
      m_Mesh->ClearEditingState();
      std::vector< u32 > selected_verts;

      for ( u32 i = 0; i < m_OldPositions.size(); i++ )
      {
        m_Mesh->AddVert( m_OldPositions.at(i) );
        selected_verts.push_back( m_Mesh->m_selected_verts.front() );
      }
      m_Mesh->m_selected_verts = selected_verts;
      m_Mesh->GetScene()->Execute( true );
    }
    break;
  }

  if ( !m_Mesh->GetScene()->GetManager()->GetCurrentScene()->GetTool() || m_Mesh->GetScene()->GetManager()->GetCurrentScene()->GetTool()->GetType() != Reflect::GetType<Editor::NavMeshCreateTool>() )
  {
    m_Mesh->ClearEditingState();
  }
}
