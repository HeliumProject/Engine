#include "Precompile.h"
#include "NavMeshCreateTool.h"
#include "NavMeshCommand.h"
#include "CreateTool.h"

#include "Color.h"
#include "Pick.h"

#include "PrimitiveAxes.h"
#include "PrimitiveCone.h"
#include "PrimitiveCircle.h"
#include "PrimitiveFrame.h"
#include "PrimitiveCube.h"

#include "Scene.h"
#include "SceneManager.h"
#include "SceneEditor.h"
#include "HierarchyNodeType.h"
#include "ScaleManipulator.h"
#include "TranslateManipulator.h"
#include "TransformManipulator.h"
#include "Volume.h"
#include "VolumeType.h"

#include "Pipeline/Content/Nodes/Geometry/Mesh.h"
#include "Pipeline/Content/Nodes/Instance/Volume.h"

#include "Foundation/Math/AngleAxis.h"
#include "Foundation/Math/EulerAngles.h"

using namespace Math;
using namespace Luna;

LUNA_DEFINE_TYPE(Luna::NavMeshCreateTool);

bool NavMeshCreateTool::s_SurfaceSnap = false;
bool NavMeshCreateTool::s_ObjectSnap = false;
f32 NavMeshCreateTool::s_Size = 0.3f;

void NavMeshCreateTool::InitializeType()
{
  Reflect::RegisterClass< Luna::NavMeshCreateTool >( TXT( "Luna::NavMeshCreateTool" ) );
}

void NavMeshCreateTool::CleanupType()
{
  Reflect::UnregisterClass< Luna::NavMeshCreateTool >();
}

NavMeshCreateTool::NavMeshCreateTool( Luna::Scene* scene, Enumerator* enumerator )
: Luna::SceneTool( scene, enumerator )
, m_Instance( NULL )
, m_Created( false )
, m_EditMode( EDIT_MODE_DISABLED )
, m_PunchOutEditMode( EDIT_MODE_DISABLED )
, m_SelectedAxes( Math::MultipleAxes::None )
, m_ManipulatorPos( Math::Matrix4::Identity )
, m_PreviousDrag( Math::Vector3::Zero )
, m_ResMode (RES_MODE_HIGH_RES)
, m_PickedManipulator( true )
, m_Type (RotationTypes::None)
, m_AxisSnap( false )
, m_SnapDegrees( 15.0f )
, m_RotationStartValue( Math::Vector3::Zero )
{
  m_PunchOutTranslator = new Luna::TranslateManipulator( ManipulatorModes::Translate, scene, enumerator );
  m_PunchOutRotator = new Luna::RotateManipulator( ManipulatorModes::Rotate, scene, enumerator );
  m_PunchOutScaler = new Luna::ScaleManipulator( ManipulatorModes::Scale, scene, enumerator );

  ZeroMemory(&m_AxisMaterial, sizeof(m_AxisMaterial));
  m_AxisMaterial.Ambient = Luna::Color::BLACK;
  m_AxisMaterial.Diffuse = Luna::Color::BLACK;
  m_AxisMaterial.Specular = Luna::Color::BLACK;

  ZeroMemory(&m_SelectedAxisMaterial, sizeof(m_SelectedAxisMaterial));
  m_SelectedAxisMaterial.Ambient = Luna::Color::YELLOW;
  m_SelectedAxisMaterial.Diffuse = Luna::Color::BLACK;
  m_SelectedAxisMaterial.Specular = Luna::Color::BLACK;

  SetHoverSelectMode( MOUSE_HOVER_SELECT_VERT );

  m_Axes = new Luna::PrimitiveAxes (m_Scene->GetView()->GetResources());
  m_Axes->Update();

  m_Ring = new Luna::PrimitiveCircle (m_Scene->GetView()->GetResources());
  m_Ring->Update();

  m_XCone = new Luna::PrimitiveCone (m_Scene->GetView()->GetResources());
  m_XCone->SetSolid( true );
  m_XCone->Update();

  m_YCone = new Luna::PrimitiveCone (m_Scene->GetView()->GetResources());
  m_YCone->SetSolid( true );
  m_YCone->Update();

  m_ZCone = new Luna::PrimitiveCone (m_Scene->GetView()->GetResources());
  m_ZCone->SetSolid( true );
  m_ZCone->Update();

  ResetManipulatorSize();
  SceneEditor* editor = (SceneEditor*)(m_Scene->GetManager()->GetEditor());
  editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshWorkWithLOWRes, m_ResMode == RES_MODE_LOW_RES);

  m_SelectionFrame = new Luna::PrimitiveFrame ( m_Scene->GetView()->GetResources() );
  m_SelectionFrame->Update();
  SetSurfaceSnap(true);

  OS_HierarchyNodeDumbPtr::Iterator childItr = m_Scene->GetRoot()->GetChildren().Begin();
  OS_HierarchyNodeDumbPtr::Iterator childEnd = m_Scene->GetRoot()->GetChildren().End();

#if (PUNCH_VOLUME)
  m_PunchOutVolume = NULL;
  for ( ; childItr != childEnd; ++childItr )
  {
    m_PunchOutVolume = Reflect::ObjectCast<Luna::Volume >( *childItr );
    if (m_PunchOutVolume)
    {
      break;
    }
  }
  if (m_PunchOutVolume == NULL)
  {
    Content::VolumePtr v = new Content::Volume();
    v->m_Shape = Content::VolumeShapes::Cube ;
    m_PunchOutVolume = new Luna::Volume ( m_Scene, v );
    m_PunchOutVolume->SetSelected( true );
    m_PunchOutVolume->SetTransient( true );
    m_PunchOutVolume->SetHidden( true );
    //m_PunchOutVolume->SetObjectTransform((position);)
    m_Scene->AddObject( m_PunchOutVolume );
    m_PunchOutVolume->Evaluate( GraphDirections::Downstream );
  }

#endif
  childItr = m_Scene->GetRoot()->GetChildren().Begin();
  childEnd = m_Scene->GetRoot()->GetChildren().End();
  for ( ; childItr != childEnd; ++childItr )
  {
    Luna::NavMesh* mesh = Reflect::ObjectCast< Luna::NavMesh >( *childItr );
    if ( mesh )
    {
      Content::Mesh* c_navMesh = mesh->GetPackage<Content::Mesh>();
      if (c_navMesh)
      {
        if (c_navMesh->m_MeshOriginType == Content::Mesh::NavHiRes && m_ResMode != RES_MODE_HIGH_RES)
        {
          continue;
        }
        if (c_navMesh->m_MeshOriginType == Content::Mesh::NavLowRes && m_ResMode != RES_MODE_LOW_RES)
        {
          continue;
        }
      }//if (c_navMesh
      // We only have one navmesh per scene so use the one that is already here
      m_Instance = mesh;
      m_Instance->SetSelected( true );
      //m_Instance->SetTransient( true );
      m_Instance->Evaluate( GraphDirections::Downstream );
      m_Instance->SetSelectionMode( m_MouseHoverSelectionMode );
      m_Selection.Append( m_Instance );
      m_Scene->GetSelection().SetItems( m_Selection );
      return;
    }
  }

  CreateInstance( Math::Vector3::Zero );
  m_Selection.Append( m_Instance );
  m_Scene->GetSelection().SetItems( m_Selection );
}

NavMeshCreateTool::~NavMeshCreateTool()
{
  // make sure to do this BEFORE you delete pointers, m_Scene->Excute will call our Evaluate function
  m_PunchOutVolume->SetHidden( true );
  m_Scene->Execute( true );

  m_Instance->SetSelectionMode( 0xFFFFFFFF );
  m_Instance->ClearEditingState();
  m_Instance->m_DrawLocator = false;

  //m_Scene->RemoveObject( m_PunchOutVolume );
  //m_PunchOutVolume = NULL;

  SetHoverSelectMode( MOUSE_HOVER_SELECT_DISABLED );

  SceneEditor* editor = (SceneEditor*)(m_Scene->GetManager()->GetEditor());
  editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshManipulate, false );
  editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshRotate, false );
  editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshPunchOut, false );
  editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshPunchOutTranslate, false );
  editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshPunchOutRotate, false );
  editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshPunchOutScale, false );

  m_Selection.Clear();
  m_Scene->Push( m_Scene->GetSelection().SetItems( m_Selection ) );

  if (m_Instance.ReferencesObject())
  {
    m_Instance = NULL;
  }

  delete m_Axes;
  delete m_Ring;

  delete m_XCone;
  delete m_YCone;
  delete m_ZCone;

  delete m_SelectionFrame;

  delete m_PunchOutRotator;
  delete m_PunchOutTranslator;
  delete m_PunchOutScaler;
}

void NavMeshCreateTool::CreateInstance( const Math::Vector3& position )
{
  m_Instance = new Luna::NavMesh( m_Scene, new Content::Mesh() );
  if ( m_Instance )
  {
    Content::Mesh* c_navMesh = m_Instance->GetPackage<Content::Mesh>();
    if (c_navMesh)
    {
      if (m_ResMode == RES_MODE_HIGH_RES)
      {
        c_navMesh->m_MeshOriginType = Content::Mesh::NavHiRes; 
      }
      else
      {
        c_navMesh->m_MeshOriginType = Content::Mesh::NavLowRes; 
      }
    }
  }

  m_Instance->SetSelected( true );

  //m_Instance->SetTransient( true );

  Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

  // add the existence of this object to the batch
  batch->Push( new SceneNodeExistenceCommand( Undo::ExistenceActions::Add, m_Scene, m_Instance ) );

  // commit the changes
  m_Scene->Push( batch );
  //m_Scene->AddObject( m_Instance );
  m_Instance->Evaluate( GraphDirections::Downstream );
}

void NavMeshCreateTool::PickPosition(int x, int y, Math::Vector3 &position)
{
  FrustumLinePickVisitor pick (m_Scene->GetView()->GetCamera(), x, y);

  // pick in the world
  m_Scene->GetManager()->GetRootScene()->Pick(&pick);

  bool set = false;

  if (s_SurfaceSnap || s_ObjectSnap)
  {
    V_PickHitSmartPtr sorted;
    PickHit::Sort(m_Scene->GetView()->GetCamera(), pick.GetHits(), sorted, PickSortTypes::Surface);

    V_PickHitSmartPtr::const_iterator itr = sorted.begin();
    V_PickHitSmartPtr::const_iterator end = sorted.end();
    for ( ; itr != end; ++itr )
    {
      // don't snap if we are surface snapping with no normal
      if ( s_SurfaceSnap && !(*itr)->HasNormal() )
      {
        continue;
      }

      if ( (*itr)->GetObject() != m_Instance )
      {
        Luna::HierarchyNode* node = Reflect::ObjectCast<Luna::HierarchyNode>( (*itr)->GetObject() );

        if ( s_ObjectSnap )
        {
          Vector4 v = node->GetTransform()->GetGlobalTransform().t;
          position.x = v.x;
          position.y = v.y;
          position.z = v.z;
        }
        else
        {
          position = (*itr)->GetIntersection();
        }

        set = true;
        break;
      }
    }
  }

  if (!set)
  {
    // place the object on the camera plane
    m_Scene->GetView()->GetCamera()->ViewportToPlaneVertex(x, y, Luna::CreateTool::s_PlaneSnap, position);
  }
}

bool NavMeshCreateTool::PickManipulator( PickVisitor* pick )
{
  // get the transform for our object
  Math::Matrix4 m = m_ManipulatorPos;

  // setup the pick object
  LinePickVisitor* linePick = dynamic_cast<LinePickVisitor*>(pick);
  linePick->SetCurrentObject (this, m);
  linePick->ClearHits();

  Math::AxesFlags parallelAxis = m_View->GetCamera()->ParallelAxis(m, Math::CriticalDotProduct);

  if (linePick->PickPoint(Math::Vector3::Zero, m_Ring->m_Radius))
  {
    m_SelectedAxes = Math::MultipleAxes::All;
  }
  else
  {
    m_SelectedAxes = m_Axes->PickAxis (m,linePick->GetWorldSpaceLine(), m_XCone->m_Radius);

    //
    // Prohibit picking a parallel axis
    //

    if (m_SelectedAxes != Math::MultipleAxes::None)
    {
      if (parallelAxis != Math::MultipleAxes::None)
      {
        switch (m_SelectedAxes)
        {
        case Math::MultipleAxes::X:
          {
            if (parallelAxis == Math::MultipleAxes::X)
              m_SelectedAxes = Math::MultipleAxes::None;
            break;
          }

        case Math::MultipleAxes::Y:
          {
            if (parallelAxis == Math::MultipleAxes::Y)
              m_SelectedAxes = Math::MultipleAxes::None;
            break;
          }

        case Math::MultipleAxes::Z:
          {
            if (parallelAxis == Math::MultipleAxes::Z)
              m_SelectedAxes = Math::MultipleAxes::None;
            break;
          }
        }
      }
    }

    if (m_SelectedAxes == Math::MultipleAxes::None)
    {
      linePick->SetCurrentObject( this, Math::Matrix4::RotateY(-Math::HalfPi) * Math::Matrix4 (m_XPosition) * m );
      if (parallelAxis != Math::MultipleAxes::X && m_XCone->Pick(pick))
      {
        m_SelectedAxes = Math::MultipleAxes::X;
      }
      else
      {
        linePick->SetCurrentObject( this, Math::Matrix4::RotateX(Math::HalfPi) * Math::Matrix4 (m_YPosition) * m );
        if (parallelAxis != Math::MultipleAxes::Y && m_YCone->Pick(pick))
        {
          m_SelectedAxes = Math::MultipleAxes::Y;
        }
        else
        {
          linePick->SetCurrentObject( this, Math::Matrix4 (m_ZPosition) * m );
          if (parallelAxis != Math::MultipleAxes::Z && m_ZCone->Pick(pick))
          {
            m_SelectedAxes = Math::MultipleAxes::Z;
          }
        }
      }
    }

    m_Axes->SetRGB();

    if ( m_SelectedAxes != Math::MultipleAxes::None )
    {
      m_Axes->SetColor(m_SelectedAxes, Luna::Color::ColorValueToColor(Luna::Color::YELLOW));
    }

    m_Axes->Update();
  }

  if (m_SelectedAxes != Math::MultipleAxes::All && m_SelectedAxes != Math::MultipleAxes::None && wxIsCtrlDown())
  {
    m_SelectedAxes = (Math::AxesFlags)(~m_SelectedAxes & Math::MultipleAxes::All);
  }

  if (m_SelectedAxes != Math::MultipleAxes::None)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void NavMeshCreateTool::AddToScene()
{
  if ( !m_Instance.ReferencesObject() )
  {
    return;
  }

  if ( !m_Scene->IsEditable()  )
  {
    return;
  }

  m_Scene->Execute( false );

  m_Instance->Evaluate( GraphDirections::Downstream );
}

bool NavMeshCreateTool::AllowSelection()
{
  return false;
}

void NavMeshCreateTool::MouseUp( wxMouseEvent& e )
{
  switch ( m_EditMode )
  {
  case EDIT_MODE_MOVE:
    {
      if ( m_PickedManipulator )
      {
        break;
      }
      // we didn't hit the manipulator so go back to adding points
      SetEditMode( EDIT_MODE_ADD );
    }
    break;
  case EDIT_MODE_ADD:
    {
      if ( e.LeftUp() )
      {
        // push the add to the undo queue
        Undo::BatchCommandPtr batch = new Undo::BatchCommand();
        Math::V_Vector3 positions;
        positions.push_back( m_StartValue );
        batch->Push( new AddNavMeshCommand( m_Instance, positions, AddNavMeshCommand::kAddVert ));
        m_Scene->Push( batch );

        //m_Instance->AddVert( m_StartValue );

        m_Instance->Dirty();
      }
    }
    break;
  case EDIT_MODE_MARQUEE_SELECT:
    {
      SetEditMode( m_PrevEditMode );
      m_MarqueMouseCoords[1].x = e.GetX();
      m_MarqueMouseCoords[1].y = e.GetY();
      //printf("Marquee ends (%f,%f) --> (%f,%f)\n", m_MarqueMouseCoords[0].x, m_MarqueMouseCoords[0].y, m_MarqueMouseCoords[1].x, m_MarqueMouseCoords[1].y);
      Luna::Camera* cam = m_Scene->GetView()->GetCamera();
      Math::Frustum frustum;
      if (cam->ViewportToFrustum(m_MarqueMouseCoords[0].x, m_MarqueMouseCoords[0].y, m_MarqueMouseCoords[1].x, m_MarqueMouseCoords[1].y, frustum))
      {
        m_Instance->m_marquee_selected_verts.clear();
        m_Instance->m_marquee_selected_edges.clear();
        m_Instance->m_marquee_selected_tris.clear();
        switch(m_MouseHoverSelectionMode)
        {
        case MOUSE_HOVER_SELECT_VERT:
          { 
            NavMesh::MarqueeSelectionMode selection_mode = NavMesh::kClearPreviousSelection;
            if (e.m_shiftDown)
            {
              selection_mode = NavMesh::kAddToPreviousSelection;
            }
            if (e.m_controlDown)
            {
              selection_mode = NavMesh::KRemovedFromPeeviousSElection;
            }
            m_Instance->GetMarqueeSelectedVerts(frustum, selection_mode);
          }
          break;
        case MOUSE_HOVER_SELECT_EDGE:
          {
            NavMesh::MarqueeSelectionMode selection_mode = NavMesh::kClearPreviousSelection;
            if (e.m_shiftDown)
            {
              selection_mode = NavMesh::kAddToPreviousSelection;
            }
            if (e.m_controlDown)
            {
              selection_mode = NavMesh::KRemovedFromPeeviousSElection;
            }
            m_Instance->GetMarqueeSelectedEdges(frustum, selection_mode);
          }
          break; 
        case MOUSE_HOVER_SELECT_TRI:
          {
            NavMesh::MarqueeSelectionMode selection_mode = NavMesh::kClearPreviousSelection;
            if (e.m_shiftDown)
            {
              selection_mode = NavMesh::kAddToPreviousSelection;
            }
            if (e.m_controlDown)
            {
              selection_mode = NavMesh::KRemovedFromPeeviousSElection;
            }
            m_Instance->GetMarqueeSelectedTris(frustum, selection_mode);
          }
          break;
        }
      }

      // set the manipulator position to be the average of all selected points
      u32 numVerts = 0;
      Math::Vector3 vertsAdded = Math::Vector3::Zero;
      std::vector< u32 > selected_verts = m_Instance->GetSelectedVerts();
      for( u32 i = 0; i < selected_verts.size(); i++ )
      {
        numVerts++;
        vertsAdded += m_Instance->GetVertPosition( selected_verts.at(i) );
      }

      m_ManipulatorPos.t = vertsAdded/numVerts;
    }
    break;
  case EDIT_MODE_ROTATE:
    {
      if ( m_StartValue != Math::Vector3::Zero )
      {
        m_RotationStartValue = m_StartValue;
      }

      if ( m_PickedManipulator )
      {
        break;
      }
      // we didn't hit the manipulator so go back to adding points
      SetEditMode( EDIT_MODE_ADD );
    }
    break;
  case EDIT_MODE_CUBE_PUNCH_OUT:
    {
      switch( m_PunchOutEditMode )
      {
      case EDIT_MODE_MOVE:
        {
          m_PunchOutTranslator->MouseUp( e );
        }
        break;
      case EDIT_MODE_ROTATE:
        {
          m_PunchOutRotator->MouseUp( e );
        }
        break;
      case EDIT_MODE_SCALE:
        {
          m_PunchOutScaler->MouseUp( e );
        }
        break;
      }
    }
    break;
  }
}

bool NavMeshCreateTool::MouseDown( wxMouseEvent& e )
{
  LinePickVisitor pick (m_View->GetCamera(), e.GetX(), e.GetY());
  Math::Vector3 position;
  PickPosition( e.GetX(), e.GetY(), position );
  Math::Vector4 view_space(position.x, position.y, position.z, 1.0f);

  if ( m_Instance.ReferencesObject() && m_Scene->IsEditable() )
  {
    m_StartValue = position;
    switch(m_EditMode)
    {
    case EDIT_MODE_MOVE:
      {
        if ( e.m_rightDown )
        {
          switch(m_MouseHoverSelectionMode)
          {
          case MOUSE_HOVER_SELECT_VERT:
            {
              if ( m_Instance->m_mouse_over_vert == 0xFFFFFFFF  )
              {
                m_Instance->ClearEditingState();
              }
              else 
              {
                if (!e.m_shiftDown)
                {
                  m_Instance->ClearSelectionState();
                }
                if ( m_Instance->ToggleMouseOverVertSelection() )
                {
                  m_StartValue = m_Instance->GetMouseOverVertPosition();
                  m_ManipulatorPos.t = m_StartValue;
                }
                else
                {
                  // we deselected the vert so move the manipulator to the last selected
                  if ( !m_Instance->m_selected_verts.empty() )
                  {
                    m_ManipulatorPos.t = m_Instance->GetVertPosition( m_Instance->m_selected_verts.front() );
                  }
                  else
                  {
                    m_ManipulatorPos.t = Math::Vector3::Zero;
                  }
                }
              }
            }
            break;
          case MOUSE_HOVER_SELECT_EDGE:
            {
              if ( m_Instance->m_mouse_over_edge == 0xFFFFFFFF )
              {
                m_Instance->ClearEditingState();
              }
              else
              {
                if (!e.m_shiftDown)
                {
                  m_Instance->ClearSelectionState();
                }
                if ( m_Instance->ToggleMouseOverEdgeSelection() )
                {
                  std::vector< u32 > selected_verts = m_Instance->GetSelectedVerts();
                  Math::Vector3 p1 = m_Instance->GetVertPosition( selected_verts.at(0) );
                  Math::Vector3 p2 = m_Instance->GetVertPosition( selected_verts.at(1) );

                  m_ManipulatorPos.t = (p2 + p1)/2;
                }
                else
                {
                  // we deselected the edge so move the manipulator to the last selected
                  m_ManipulatorPos.t = Math::Vector3::Zero;
                }
              }
            }
            break;
          case MOUSE_HOVER_SELECT_TRI:
            {
              if ( m_Instance->m_mouse_over_tri == 0xFFFFFFFF )
              {
                m_Instance->ClearEditingState();
              }
              else
              {
                m_Instance->ToggleMouseOverTriSelection();

                if ( m_Instance->m_selected_tris.empty() )
                {
                  m_ManipulatorPos.t = Math::Vector3::Zero;
                }
                else
                {
                  std::vector< u32 > selected_verts = m_Instance->GetSelectedVerts();
                  Math::Vector3 p1 = m_Instance->GetVertPosition( selected_verts.at(0) );
                  Math::Vector3 p2 = m_Instance->GetVertPosition( selected_verts.at(1) );
                  Math::Vector3 p3 = m_Instance->GetVertPosition( selected_verts.at(2) );

                  m_ManipulatorPos.t = (p3 + p2 + p1)/3;
                }
              }
            }
            break;
          }
          return true;
        }
        if ( e.m_leftDown )
        {
          m_StartX = e.GetX();
          m_StartY = e.GetY();

          if (PickManipulator(&pick))
          {   
            m_PickedManipulator = true;
            if ( m_SelectedAxes == Math::MultipleAxes::All )
            {
              m_StartValue = Math::Vector3( m_ManipulatorPos.t.x, m_ManipulatorPos.t.y, m_ManipulatorPos.t.z );
            }
            else
            {
              // hack to make sure our start point isn't picked on the ground when we are manipulating
              m_Scene->GetView()->GetCamera()->ViewportToPlaneVertex(e.GetX(), e.GetY(), IntersectionPlanes::View, m_StartValue);
            }

            // push the original position to the undor queue
            Undo::BatchCommandPtr batch = new Undo::BatchCommand();
            Math::V_Vector3 positions;
            batch->Push( new AddNavMeshCommand( m_Instance, positions, AddNavMeshCommand::kModify ));
            m_Scene->Push( batch );

            // don't let the mouse click go to __super
            return true;
          }
          // didn't click on an axis of the manipulator so go back to laying down verts
          m_PickedManipulator = false;
          //SetEditMode( EDIT_MODE_ADD );
          return true;
        }
      } // don't break just go straight into adding
    case EDIT_MODE_ADD:
      {
        if (e.m_leftDown)
        {
          // push the add to the undo queue
          /*
          Undo::BatchCommandPtr batch = new Undo::BatchCommand();
          Math::V_Vector3 positions;
          positions.push_back( m_StartValue );
          batch->Push( new AddNavMeshCommand( m_Instance, positions, AddNavMeshCommand::kAdd ));
          m_Scene->Push( batch );
          */

          //m_Instance->AddVert( m_StartValue );

          //m_Instance->Dirty();
        }
        else if (e.m_rightDown)
        {
          switch(m_MouseHoverSelectionMode)
          {
          case MOUSE_HOVER_SELECT_VERT:
            {
              if ( !e.ShiftDown() && m_Instance->m_mouse_over_vert == 0xFFFFFFFF  )
              {
                m_Instance->ClearEditingState();
              }
              else 
              {
                if (!e.m_shiftDown)
                {
                  m_Instance->ClearSelectionState();
                }
                m_Instance->ToggleMouseOverVertSelection();
              }
            }
            break;
          case MOUSE_HOVER_SELECT_EDGE:
            {
              if ( m_Instance->m_mouse_over_edge == 0xFFFFFFFF )
              {
                m_Instance->ClearEditingState();
              }
              else
              {
                if (!e.m_shiftDown)
                {
                  m_Instance->ClearSelectionState();
                }
                m_Instance->ToggleMouseOverEdgeSelection();
              }
            }
            break;
          case MOUSE_HOVER_SELECT_TRI:
            {
              if ( m_Instance->m_mouse_over_tri == 0xFFFFFFFF  )
              {
                m_Instance->ClearEditingState();
              }
              else
              {
                if (!e.m_shiftDown)
                {
                  m_Instance->ClearSelectionState();
                }
                m_Instance->ToggleMouseOverTriSelection();
                m_Instance->AddMouseOverTriToDeleteQueue();
              }
            }
            break;
          }
        }
        else if (e.m_middleDown)
        {
          m_Instance->ChangeTriEdgeVertSelection();
        } //if (e.m_leftDown)
      }//case
      break;
    case EDIT_MODE_CUBE_PUNCH_OUT:
      {
#if (PUNCH_VOLUME)
   
        switch( m_PunchOutEditMode )
        {
        case EDIT_MODE_DISABLED:
          {
            if ( e.m_leftDown )
            {
              //do the punching
              Math::Matrix4 cube_transform_mat = m_PunchOutVolume->GetGlobalTransform();
              Math::Matrix4 cube_inv_transform_mat = m_PunchOutVolume->GetInverseGlobalTransform();
              // push the original position to the undor queue
              Undo::BatchCommandPtr batch = new Undo::BatchCommand();
              batch->Push( new AddNavMeshCommand( m_Instance, cube_transform_mat, cube_inv_transform_mat, AddNavMeshCommand::kPunchCubeHole ));
              m_Scene->Push( batch );
            }
            else if ( e.m_middleDown )
            {

            }
            else if ( e.m_rightDown )
            {
              m_PunchOutVolume->SetTranslate(position);
              m_PunchOutVolume->Evaluate(GraphDirections::Downstream);
            }
          }
          break;
        case EDIT_MODE_MOVE:
          {
            if ( !m_PunchOutTranslator->MouseDown( e ) )
            {
              SetPunchOutMode( EDIT_MODE_DISABLED );
            }
          }
          break;
        case EDIT_MODE_ROTATE:
          {
            if ( !m_PunchOutRotator->MouseDown( e ) )
            {
              SetPunchOutMode( EDIT_MODE_DISABLED );
            }
          }
          break;
        case EDIT_MODE_SCALE:
          {
            if ( !m_PunchOutScaler->MouseDown( e ) )
            {
              SetPunchOutMode( EDIT_MODE_DISABLED );
            }
          }
          break;
        }
#endif
      }
      break;
    case EDIT_MODE_ROTATE:
      {
        if ( e.m_rightDown )
        {
          switch(m_MouseHoverSelectionMode)
          {
          case MOUSE_HOVER_SELECT_VERT:
            {
              if ( m_Instance->m_mouse_over_vert == 0xFFFFFFFF )
              {
                m_Instance->ClearEditingState();
              }
              else if ( m_Instance->ToggleMouseOverVertSelection() )
              {
                m_ManipulatorPos.t = m_Instance->GetMouseOverVertPosition();
                m_StartValue = Math::Vector3::Zero;
              }
              else
              {
                // we deselected the vert so move the manipulator to the last selected
                if ( !m_Instance->m_selected_verts.empty() )
                {
                  m_ManipulatorPos.t = m_Instance->GetVertPosition( m_Instance->m_selected_verts.front() );
                }
                else
                {
                  m_ManipulatorPos.t = Math::Vector3::Zero;
                }
              }
            }
            break;
          case MOUSE_HOVER_SELECT_EDGE:
            {
              if ( m_Instance->m_mouse_over_edge == 0xFFFFFFFF )
              {
                m_Instance->ClearEditingState();
              }
              else if ( m_Instance->ToggleMouseOverEdgeSelection() )
              {
                std::vector< u32 > selected_verts = m_Instance->GetSelectedVerts();
                Math::Vector3 p1 = m_Instance->GetVertPosition( selected_verts.at(0) );
                Math::Vector3 p2 = m_Instance->GetVertPosition( selected_verts.at(1) );

                m_ManipulatorPos.t = (p2 + p1)/2;
              }
              else
              {
                // we deselected the edge so move the manipulator to the last selected
                m_ManipulatorPos.t = Math::Vector3::Zero;
              }
            }
            break;
          case MOUSE_HOVER_SELECT_TRI:
            {
              if ( m_Instance->m_mouse_over_tri == 0xFFFFFFFF )
              {
                m_Instance->ClearEditingState();
              }
              else
              {
                m_Instance->ToggleMouseOverTriSelection();

                if ( m_Instance->m_selected_tris.empty() )
                {
                  m_ManipulatorPos.t = Math::Vector3::Zero;
                }
                else
                {
                  std::vector< u32 > selected_verts = m_Instance->GetSelectedVerts();
                  Math::Vector3 p1 = m_Instance->GetVertPosition( selected_verts.at(0) );
                  Math::Vector3 p2 = m_Instance->GetVertPosition( selected_verts.at(1) );
                  Math::Vector3 p3 = m_Instance->GetVertPosition( selected_verts.at(2) );

                  m_ManipulatorPos.t = (p3 + p2 + p1)/3;
                }
              }
            }
            break;
          }
          return true;
        }
        if ( e.m_leftDown )
        {
          m_StartX = e.GetX();
          m_StartY = e.GetY();

          if (PickRotator(&pick))
          {
            m_CachedPositions.clear();
            for ( u32 i=0; i < m_Instance->m_selected_verts.size(); i++ )
            {
              m_CachedPositions.push_back( m_Instance->GetVertPosition( m_Instance->m_selected_verts.at(i) ) );
            }

            m_PickedManipulator = true;
            m_StartValue = Math::Vector3::Zero;

            // push the original position to the undor queue
            Undo::BatchCommandPtr batch = new Undo::BatchCommand();
            Math::V_Vector3 positions;
            batch->Push( new AddNavMeshCommand( m_Instance, positions, AddNavMeshCommand::kModify ));
            m_Scene->Push( batch );

            // don't let the mouse click go to __super
            return true;
          }
          // didn't click on an axis of the manipulator so go back to laying down verts
          m_PickedManipulator = false;
          //SetEditMode( EDIT_MODE_ADD );
          return true;
        }
      }
      break;
    }
  }

  m_Scene->Execute( true );
  return __super::MouseDown( e );
}

void NavMeshCreateTool::MouseMove( wxMouseEvent& e )
{
  if ( m_Instance.ReferencesObject() )
  {
    Math::Vector3 position;
    PickPosition( e.GetX(), e.GetY(), position );
    Math::Vector4 view_space(position.x, position.y, position.z, 1.0f);
    m_Instance->UpdateLocator(position);

    if ( m_Instance->GetNumberVertices() > 0 )
    {
      Luna::Camera* cam = m_Scene->GetView()->GetCamera();
      Math::Vector3 ss_start;
      cam->ViewportToWorldVertex (e.GetX(), e.GetY(), ss_start);
      Math::Vector3 ss_dir;
      cam->ViewportToWorldNormal(e.GetX(), e.GetY(), ss_dir); //= position - ss_start;
      ss_dir = ss_dir.Normalize();
      f32 ss_len = Luna::Camera::FarClipDistance;
      Math::Matrix4 view_mat = cam->GetView();
      view_mat.Transform(view_space);
      view_space.w = 0.0f;
      f32 dist_to_cam = view_space.Length();
      f32 t = (dist_to_cam/50.0f);
      t = Math::Clamp(t, 0.0f, 1.0f);

      Math::Vector3 view_proj_vec3 = Math::Vector3::Zero;
      cam->ScreenToViewport(e.GetX(), e.GetY(), view_proj_vec3);
      Math::Vector2 view_proj_vec2;
      view_proj_vec2.x = view_proj_vec3.x;
      view_proj_vec2.y = view_proj_vec3.y;
      Math::Vector3 view_proj_off_set_vec3 = Math::Vector3::Zero;
      cam->ScreenToViewport(e.GetX()+3, e.GetY()+3, view_proj_off_set_vec3);//3 pixel trheshold
      view_proj_off_set_vec3 -= view_proj_vec3;
      f32 thresh_sqr = view_proj_off_set_vec3.LengthSquared();
      Math::Matrix4 view_proj_mat = cam->GetView()*cam->GetProjection();

      switch ( m_EditMode)
      {
      case EDIT_MODE_ADD:
        {
          if ( e.Dragging() && e.LeftIsDown() )
          {
            m_PrevEditMode = m_EditMode;
            SetEditMode( EDIT_MODE_MARQUEE_SELECT );
            //printf("Marquee starts\n");
            m_MarqueMouseCoords[0].x = e.GetX();
            m_MarqueMouseCoords[0].y = e.GetY();
            m_SelectionFrame->m_Start = Math::Point (e.GetX(), e.GetY());
            m_SelectionFrame->m_End = Math::Point (e.GetX(), e.GetY());
            m_SelectionFrame->Update();
            break;
          }
          else
          {
            // select the edge closest to the locator
            m_Instance->SelectNearestEdge( position );
            switch(m_MouseHoverSelectionMode)
            {
            case MOUSE_HOVER_SELECT_VERT:
              {
                m_Instance->m_mouse_over_edge = 0xFFFFFFFF;
                m_Instance->m_mouse_over_tri = 0xFFFFFFFF;
                m_Instance->UpdateMouseOverVert( view_proj_mat, thresh_sqr, view_proj_vec2);
              }
              break;
            case MOUSE_HOVER_SELECT_EDGE:
              {
                m_Instance->m_mouse_over_tri = 0xFFFFFFFF;
                m_Instance->m_mouse_over_vert = 0xFFFFFFFF;
                f32 scan_dist = 0.01f + 0.5f*t;
                m_Instance->UpdateMouseOverEdge(ss_start, scan_dist, ss_dir, ss_len);
              }
              break;
            case MOUSE_HOVER_SELECT_TRI:
              {
                m_Instance->m_mouse_over_edge = 0xFFFFFFFF;
                m_Instance->m_mouse_over_vert = 0xFFFFFFFF;
                f32 scan_dist = 0.1f + 10.0f*t;
                m_Instance->UpdateMouseOverTri(ss_start, scan_dist, ss_dir, ss_len);
              }
              break;
            }
          }
        }
      case EDIT_MODE_ROTATE:
      case EDIT_MODE_MOVE:
        {
          if ( e.Dragging() && e.LeftIsDown() )
          {
            if ( m_PickedManipulator )
            {
              if ( m_EditMode == EDIT_MODE_MOVE )
              {
                // we are draggin the manipulator
                MoveManipulator( e );
              }
              if ( m_EditMode == EDIT_MODE_ROTATE )
              {
                MoveRotator( e );
              }
            }
            else
            {
              m_PrevEditMode = m_EditMode;
              SetEditMode( EDIT_MODE_MARQUEE_SELECT );
              m_MarqueMouseCoords[0].x = e.GetX();
              m_MarqueMouseCoords[0].y = e.GetY();
              m_SelectionFrame->m_Start = Math::Point (e.GetX(), e.GetY());
              m_SelectionFrame->m_End = Math::Point (e.GetX(), e.GetY());
              m_SelectionFrame->Update();
              break;
            }
          }
          else
          {
            switch(m_MouseHoverSelectionMode) 
            {
            case MOUSE_HOVER_SELECT_VERT: 
              {
                m_Instance->m_mouse_over_tri = 0xFFFFFFFF;
                m_Instance->m_mouse_over_edge = 0xFFFFFFFF;
                m_Instance->UpdateMouseOverVert( view_proj_mat, thresh_sqr, view_proj_vec2);
              }
              break;
            case MOUSE_HOVER_SELECT_EDGE:
              {
                m_Instance->m_mouse_over_tri = 0xFFFFFFFF;
                m_Instance->m_mouse_over_vert = 0xFFFFFFFF;
                f32 scan_dist = 0.01f + 0.5f*t;
                m_Instance->UpdateMouseOverEdge(ss_start, scan_dist, ss_dir, ss_len);
              }
              break;
            case MOUSE_HOVER_SELECT_TRI:
              {
                m_Instance->m_mouse_over_edge = 0xFFFFFFFF;
                m_Instance->m_mouse_over_vert = 0xFFFFFFFF;
                f32 scan_dist = 0.1f + 10.0f*t;
                m_Instance->UpdateMouseOverTri(ss_start, scan_dist, ss_dir, ss_len);
              }
              break;
            }
          }
        }
        break;
      case EDIT_MODE_MARQUEE_SELECT:
        {
          m_SelectionFrame->m_End = Math::Point (e.GetX(), e.GetY());
          m_SelectionFrame->Update();
        }
        break;
      case EDIT_MODE_CUBE_PUNCH_OUT:
        {
          switch( m_PunchOutEditMode )
          {
          case EDIT_MODE_MOVE:
            {
              m_PunchOutTranslator->MouseMove( e );
            }
            break;
          case EDIT_MODE_ROTATE:
            {
              m_PunchOutRotator->MouseMove( e );
            }
            break;
          case EDIT_MODE_SCALE:
            {
              m_PunchOutScaler->MouseMove( e );
            }
            break;
          }
        }
        break;
      }

      m_Instance->Dirty();

      m_Scene->Execute( true );
    }
  }

  __super::MouseMove( e );
} 

void NavMeshCreateTool::KeyPress(wxKeyEvent &e)
{
  const int keyCode = e.GetKeyCode();

  switch( keyCode )
  { 
  case WXK_RETURN:
    {
      AddToScene();
      break;
    }
  case WXK_TAB:
    {
      if ( m_Instance.ReferencesObject() )
      {
        SetHoverSelectMode( (m_MouseHoverSelectionMode+1)%(MOUSE_HOVER_SELECT_DISABLED) );
      }
      break;
    }//case WXK_TAB
  }//switch( keyCode )
  switch (tolower(e.KeyCode()))
  {
  case 't':
    {
      //add tri here
      if ( m_Instance.ReferencesObject() )
      {
        // push the add to the undo queue
        Undo::BatchCommandPtr batch = new Undo::BatchCommand();
        Math::V_Vector3 positions;
        positions.push_back( m_StartValue );
        batch->Push( new AddNavMeshCommand( m_Instance, positions, AddNavMeshCommand::kAddTri ));
        m_Scene->Push( batch );
      }
    }
    break;
  case 'o':
    {
      //will force stuff to select obtuse angles tris
      m_Instance->SelectObtuseAngledTris();
      m_Scene->Execute( true );
      return;//dont pass the event back up to luna
    }
    break;
  case 'w':
    {
      m_Selection.Append( m_Instance );
      switch( m_EditMode )
      {
      case EDIT_MODE_MOVE:
        {
          SetEditMode( EDIT_MODE_ADD );
        }
        break;
      case EDIT_MODE_CUBE_PUNCH_OUT:
        {
          if ( m_PunchOutEditMode == EDIT_MODE_MOVE )
          {
            SetPunchOutMode( EDIT_MODE_DISABLED );
          }
          else
          {
            SetPunchOutMode( EDIT_MODE_MOVE );
          }
        }
        break;
      default:
        {
          SetEditMode( EDIT_MODE_MOVE );
        }
        break;
      }
      m_Scene->Execute( true );
      return;   // don't pass this keypress onto scene editor since 'w' does something different there
    }
    break;
  case WXK_DELETE:
  case 'd':
    {
      switch( m_MouseHoverSelectionMode )
      {
      case MOUSE_HOVER_SELECT_VERT:
        {       
          Undo::BatchCommandPtr batch = new Undo::BatchCommand();
          Math::V_Vector3 positions;
          for ( u32 i = 0; i < m_Instance->m_selected_verts.size(); i++ )
          {
            positions.push_back( m_Instance->GetVertPosition( m_Instance->m_selected_verts.at(i) ) );
          }

          batch->Push( new AddNavMeshCommand( m_Instance, positions, AddNavMeshCommand::kDeleteVerts ));
          m_Scene->Push( batch );
        }
        break;
      case MOUSE_HOVER_SELECT_TRI:
        {
          // push the delete to the undo queue
          Undo::BatchCommandPtr batch = new Undo::BatchCommand();
          Math::V_Vector3 positions;

          batch->Push( new AddNavMeshCommand( m_Instance, positions, AddNavMeshCommand::kDeleteTris ));
          m_Scene->Push( batch );

          //delete the tris
          //m_Instance->DeleteTrisInQueue();
        }
        break;
      case MOUSE_HOVER_SELECT_EDGE:
        {
          Undo::BatchCommandPtr batch = new Undo::BatchCommand();
          Math::V_Vector3 positions;

          batch->Push( new AddNavMeshCommand( m_Instance, positions, AddNavMeshCommand::kDeleteEdges ));
          m_Scene->Push( batch );
        }
        break;
      }
      m_Scene->Execute( true );
      return;   // don't pass this keypress onto scene editor 
    }
    break;
  case 'f':
    {
      Math::AlignedBox box;
      if ( m_EditMode == EDIT_MODE_CUBE_PUNCH_OUT )
      {
        box.Merge( m_PunchOutVolume->GetGlobalHierarchyBounds() );
      }
      else
      {
        for ( u32 i = 0; i < m_Instance->m_selected_verts.size(); i++ )
        {
          Math::Vector3 p = m_Instance->GetVertPosition( m_Instance->m_selected_verts.at(i) );
          box.Merge( p );
        }
      }
      m_View->UpdateCameraHistory();    // we want the previous state before the move
      m_View->GetCamera()->Frame(box);
      m_Scene->Execute( true );
      return;   // we are doing the framing, don't pass this to __super
    }
    break;
  case 'v':
    {
      if (m_Instance->m_selected_verts.size())
      {
        //snap the top vert in m_selected_verts to the closest nearby vert in mesh
        Undo::BatchCommandPtr batch = new Undo::BatchCommand();
        Math::V_Vector3 positions;
        batch->Push( new AddNavMeshCommand( m_Instance, positions, AddNavMeshCommand::kSnapMergeToNearestVert ));
        m_Scene->Push( batch );
      }
      return;
    }
  case 'p':
    {
      if ( m_EditMode == EDIT_MODE_CUBE_PUNCH_OUT )
      {
        SetEditMode( EDIT_MODE_ADD );
      }
      else
      {
        SetEditMode(EDIT_MODE_CUBE_PUNCH_OUT);

        if (m_Instance.ReferencesObject())
        {
#if (PUNCH_VOLUME)
          m_PunchOutVolume->SetTranslate(m_Instance->m_LocatorPos);
          m_PunchOutVolume->Evaluate(GraphDirections::Downstream);
#endif
        }
      }
      m_Scene->Execute( true );
    }
    break;
  case 'e':
    {
      switch ( m_EditMode )
      {
      case EDIT_MODE_ROTATE:
        {
          SetEditMode( EDIT_MODE_ADD );
        }
        break;
      case EDIT_MODE_CUBE_PUNCH_OUT:
        {
          if ( m_PunchOutEditMode == EDIT_MODE_ROTATE )
          {
            SetPunchOutMode( EDIT_MODE_DISABLED );
          }
          else
          {
            SetPunchOutMode( EDIT_MODE_ROTATE );
          }
        }
        break;
      default:
        {
          SetEditMode( EDIT_MODE_ROTATE );
        }
        break;
      }
      m_Scene->Execute( true );
      // don't pass the keypress on
      return;
    }
    break;
  case 'r':
    {
      if ( m_EditMode == EDIT_MODE_CUBE_PUNCH_OUT )
      {
        if ( m_PunchOutEditMode == EDIT_MODE_SCALE )
        {
          SetPunchOutMode( EDIT_MODE_DISABLED );
        }
        else
        {
          SetPunchOutMode( EDIT_MODE_SCALE );
        }
      }
      m_Scene->Execute( true );
      // don't pass the keypress on
      return;
    }
    break;
  }
  __super::KeyPress( e );
}

void NavMeshCreateTool::Evaluate()
{
  if ( m_EditMode  == EDIT_MODE_MOVE )
  {
    Math::Matrix4 m = m_ManipulatorPos;

    // compute the scaling factor
    float factor = m_View->GetCamera()->ScalingTo(Math::Vector3 (m.t.x, m.t.y, m.t.z));

    ResetManipulatorSize();

    float f = factor * s_Size;

    m_Axes->m_Length *= f;
    m_Axes->Update();

    m_Ring->m_Radius *= f;
    m_Ring->Update();

    m_XCone->m_Length *= f;
    m_XCone->m_Radius *= f;
    m_XCone->Update();
    m_XPosition *= f;

    m_YCone->m_Length *= f;
    m_YCone->m_Radius *= f;
    m_YCone->Update();
    m_YPosition *= f;

    m_ZCone->m_Length *= f;
    m_ZCone->m_Radius *= f;
    m_ZCone->Update();
    m_ZPosition *= f;
  }

  if ( m_EditMode == EDIT_MODE_ROTATE )
  {
    m_Ring->m_RadiusSteps = 360;

    Math::Matrix4 m = m_ManipulatorPos;

    // compute the scaling factor
    float factor = m_View->GetCamera()->ScalingTo(Math::Vector3 (m.t.x, m.t.y, m.t.z));

    ResetManipulatorSize();

    m_Ring->m_Radius = 0.3f * factor;
    m_Ring->Update();
  }

  if ( m_EditMode == EDIT_MODE_CUBE_PUNCH_OUT )
  {
    switch( m_PunchOutEditMode )
    {
    case EDIT_MODE_MOVE:
      {
        m_PunchOutTranslator->Evaluate();
      }
      break;
    case EDIT_MODE_ROTATE:
      {
        m_PunchOutRotator->Evaluate();
      }
      break;
    case EDIT_MODE_SCALE:
      {
        m_PunchOutScaler->Evaluate();
      }
      break;
    }
  }

  __super::Evaluate();
}

void NavMeshCreateTool::ResetManipulatorSize()
{
  m_Axes->m_Length = 1.0f;
  m_Ring->m_Radius = 0.1f;

  m_XCone->m_Length = 0.2f;
  m_XCone->m_Radius = 0.04f;
  m_XPosition = Math::Vector3::BasisX;

  m_YCone->m_Length = 0.2f;
  m_YCone->m_Radius = 0.04f;
  m_YPosition = Math::Vector3::BasisY;

  m_ZCone->m_Length = 0.2f;
  m_ZCone->m_Radius = 0.04f;
  m_ZPosition = Math::Vector3::BasisZ;
}

void NavMeshCreateTool::Draw( DrawArgs* args )
{
  if ( m_EditMode == EDIT_MODE_MOVE && m_Instance->m_selected_verts.size() > 0 )
  {
    // draw the translate manipulator
    Math::Matrix4 m = m_ManipulatorPos;

    Math::Vector3 position = Math::Vector3 (m.t.x, m.t.y, m.t.z);

    Math::AxesFlags parallelAxis = m_View->GetCamera()->ParallelAxis(m, Math::CriticalDotProduct);

    m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&m);

    m_Axes->DrawAxes(args, (Math::AxesFlags)(~parallelAxis & Math::MultipleAxes::All));

    if (parallelAxis != Math::MultipleAxes::X)
    {
      SetAxisMaterial(Math::MultipleAxes::X);

      m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&(Math::Matrix4::RotateY(Math::HalfPi) * Math::Matrix4 (m_XPosition) * m));
      m_XCone->Draw(args);
    }

    if (parallelAxis != Math::MultipleAxes::Y)
    {
      SetAxisMaterial(Math::MultipleAxes::Y);

      m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&(Math::Matrix4::RotateX(-Math::HalfPi) * Math::Matrix4 (m_YPosition) * m));
      m_YCone->Draw(args);
    }

    if (parallelAxis != Math::MultipleAxes::Z)
    {
      SetAxisMaterial(Math::MultipleAxes::Z);

      m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&(Math::Matrix4 (m_ZPosition) * m));
      m_ZCone->Draw(args);
    }

    if (m_SelectedAxes == Math::MultipleAxes::All)
    {
      m_AxisMaterial.Ambient = Luna::Color::YELLOW;
    }
    else
    {
      m_AxisMaterial.Ambient = Luna::Color::LIGHTGRAY;
    }

    Math::Vector3 cameraPosition;
    m_View->GetCamera()->GetPosition(cameraPosition);
    Math::Matrix4 border = Math::Matrix4 (Math::AngleAxis::Rotation(Math::Vector3::BasisX, cameraPosition - position)) * Math::Matrix4 (position);

    // render sphere border m_Ring
    m_View->GetDevice()->SetMaterial(&m_AxisMaterial);
    m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&border);
    m_Ring->Draw(args);
  }

  if ( m_EditMode == EDIT_MODE_MARQUEE_SELECT )
  {
    m_SelectionFrame->Draw(args);
  }

  if ( m_EditMode == EDIT_MODE_ROTATE )
  {
    // fill current axis ring
    bool fill = false;

    // get the transform for our object
    Math::Matrix4 m = m_ManipulatorPos;

    Math::Vector3 position = Math::Vector3 (m.t.x, m.t.y, m.t.z);

    // rotation from the circle axis to the camera direction
    Math::Vector3 cameraPosition;
    m_View->GetCamera()->GetPosition(cameraPosition);
    Math::Matrix4 toCamera = Math::Matrix4 ( Math::AngleAxis::Rotation(Math::Vector3::BasisX, cameraPosition - position) ) * Math::Matrix4 (position);

    // render x
    Math::Matrix4 x = m;
    m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)(&x));
    fill = SetAxisMaterial(Math::MultipleAxes::X);
    m_Ring->DrawHiddenBack(args, m_View->GetCamera(), x);
    if (fill && m_Type == RotationTypes::Normal)
    {
      D3DMATERIAL9 mat = m_AxisMaterial;
      mat.Ambient = Luna::Color::RED;
      mat.Diffuse.a = 0.2f;
      m_View->GetDevice()->SetMaterial(&mat);
      m_Ring->DrawFill(args);
    }

    // render y
    Math::Matrix4 y = Math::Matrix4::RotateZ((float)(Math::HalfPi)) * m;
    m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)(&y));
    fill = SetAxisMaterial(Math::MultipleAxes::Y);
    m_Ring->DrawHiddenBack(args, m_View->GetCamera(), y);
    if (fill && m_Type == RotationTypes::Normal)
    {
      D3DMATERIAL9 mat = m_AxisMaterial;
      mat.Ambient = Luna::Color::GREEN;
      mat.Diffuse.a = 0.2f;
      m_View->GetDevice()->SetMaterial(&mat);
      m_Ring->DrawFill(args);
    }

    // render z
    Math::Matrix4 z = Math::Matrix4::RotateY(-(float)(Math::HalfPi)) * m;
    m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)(&z));
    fill = SetAxisMaterial(Math::MultipleAxes::Z);
    m_Ring->DrawHiddenBack(args, m_View->GetCamera(), z);
    if (fill && m_Type == RotationTypes::Normal)
    {
      D3DMATERIAL9 mat = m_AxisMaterial;
      mat.Ambient = Luna::Color::BLUE;
      mat.Diffuse.a = 0.2f;
      m_View->GetDevice()->SetMaterial(&mat);
      m_Ring->DrawFill(args);
    }

    // render arcball sphere
    m_AxisMaterial.Ambient = Luna::Color::LIGHTGRAY;
    m_View->GetDevice()->SetMaterial(&m_AxisMaterial);
    m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&toCamera);
    m_Ring->Draw(args);

    // draw the center filled if we are arcballing
    if (m_SelectedAxes == Math::MultipleAxes::All && m_Type == RotationTypes::ArcBall)
    {
      D3DMATERIAL9 mat = m_AxisMaterial;
      mat.Ambient = Luna::Color::YELLOW;
      mat.Diffuse.a = 0.2f;
      m_View->GetDevice()->SetMaterial(&mat);
      m_Ring->DrawFill(args);
    }

    // render camera plane ring
    if (m_SelectedAxes == Math::MultipleAxes::All && m_Type == RotationTypes::CameraPlane)
    {
      m_AxisMaterial.Ambient = Luna::Color::YELLOW;
    }
    else
    {
      m_AxisMaterial.Ambient = Luna::Color::SKYBLUE;
    }

    m_View->GetDevice()->SetMaterial(&m_AxisMaterial);
    m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&(Math::Matrix4 (Math::Scale(1.2f, 1.2f, 1.2f)) * toCamera));
    m_Ring->Draw(args);
  }

  if ( m_EditMode == EDIT_MODE_CUBE_PUNCH_OUT )
  {
    switch ( m_PunchOutEditMode )
    {
    case EDIT_MODE_MOVE:
      {
        m_PunchOutTranslator->Draw( args );
      }
      break;
    case EDIT_MODE_ROTATE:
      {
        m_PunchOutRotator->Draw( args );
      }
      break;
    case EDIT_MODE_SCALE:
      {
        m_PunchOutScaler->Draw( args );
      }
      break;
    }
  }

  __super::Draw( args );
}

bool NavMeshCreateTool::SetAxisMaterial(Math::AxesFlags axes)
{
  if ((m_SelectedAxes & axes) != Math::MultipleAxes::None)
  {
    m_View->GetDevice()->SetMaterial(&m_SelectedAxisMaterial);
    return m_SelectedAxes != Math::MultipleAxes::All;
  }
  else
  {
    switch (axes)
    {
    case Math::MultipleAxes::X:
      m_AxisMaterial.Ambient = Luna::Color::RED;
      break;

    case Math::MultipleAxes::Y:
      m_AxisMaterial.Ambient = Luna::Color::GREEN;
      break;

    case Math::MultipleAxes::Z:
      m_AxisMaterial.Ambient = Luna::Color::BLUE;
      break;
    }

    m_View->GetDevice()->SetMaterial(&m_AxisMaterial);
    return false;
  }
}

void NavMeshCreateTool::CreateProperties()
{
  __super::CreateProperties();

  m_Enumerator->PushPanel( TXT( "Create NavMesh" ), true );
  {
    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( TXT( "Surface Snap" ) );   
      m_Enumerator->AddCheckBox<bool>( new Nocturnal::MemberProperty<Luna::NavMeshCreateTool, bool> (this, &NavMeshCreateTool::GetSurfaceSnap, &NavMeshCreateTool::SetSurfaceSnap ) );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( TXT( "Object Snap" ) );   
      m_Enumerator->AddCheckBox<bool>( new Nocturnal::MemberProperty<Luna::NavMeshCreateTool, bool> (this, &NavMeshCreateTool::GetObjectSnap, &NavMeshCreateTool::SetObjectSnap ) );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( TXT( "Plane Snap" ) );
      Inspect::Choice* choice = m_Enumerator->AddChoice<int>( new Nocturnal::MemberProperty<Luna::NavMeshCreateTool, int> (this, &NavMeshCreateTool::GetPlaneSnap, &NavMeshCreateTool::SetPlaneSnap) );
      choice->SetDropDown( true );
      Inspect::V_Item items;

      {
        tostringstream str;
        str << IntersectionPlanes::View;
        items.push_back( Inspect::Item( TXT( "View" ), str.str() ) );
      }

      {
        tostringstream str;
        str << IntersectionPlanes::Ground;
        items.push_back( Inspect::Item( TXT( "Ground" ), str.str() ) );
      }

      choice->SetItems( items );
    }
    m_Enumerator->Pop();
  }
  m_Enumerator->Pop();

  if ( m_EditMode  == EDIT_MODE_MOVE)
  {

  }

  if ( m_EditMode == EDIT_MODE_CUBE_PUNCH_OUT )
  {
    switch ( m_PunchOutEditMode )
    {
    case EDIT_MODE_MOVE:
      {
        m_PunchOutTranslator->CreateProperties();
      }
      break;
    case EDIT_MODE_ROTATE:
      {
        m_PunchOutRotator->CreateProperties();
      }
      break;
    case EDIT_MODE_SCALE:
      {
        m_PunchOutScaler->CreateProperties();
      }
      break;
    }
  }
}

bool NavMeshCreateTool::GetSurfaceSnap() const
{
  return s_SurfaceSnap;
}

void NavMeshCreateTool::SetSurfaceSnap( bool snap )
{
  s_SurfaceSnap = snap;

  if (s_SurfaceSnap)
  {
    s_ObjectSnap = false;
    m_Enumerator->GetContainer()->Read();
  }

  m_Scene->Execute( true );
}

bool NavMeshCreateTool::GetObjectSnap() const
{
  return s_ObjectSnap;
}

void NavMeshCreateTool::SetObjectSnap( bool snap )
{
  s_ObjectSnap = snap;

  if (s_ObjectSnap)
  {
    s_SurfaceSnap = false;
    m_Enumerator->GetContainer()->Read();
  }

  m_Scene->Execute( true );
}

int NavMeshCreateTool::GetPlaneSnap() const
{
  return Luna::CreateTool::s_PlaneSnap;
}

void NavMeshCreateTool::SetPlaneSnap(int snap)
{
  Luna::CreateTool::s_PlaneSnap = (IntersectionPlane)snap;

  m_Scene->Execute(false);
}

void NavMeshCreateTool::SetEditMode(u32 mode)
{
  if ( m_EditMode == EDIT_MODE_CUBE_PUNCH_OUT )
  {
    SetPunchOutMode( EDIT_MODE_DISABLED );
    m_PunchOutVolume->SetHidden( true );
  }

  m_EditMode = mode; 

  if ( m_Instance.ReferencesObject() )
  {
    SceneEditor* editor = (SceneEditor*)(m_Scene->GetManager()->GetEditor());
    editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshManipulate, m_EditMode == EDIT_MODE_MOVE);
    editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshPunchOut, m_EditMode == EDIT_MODE_CUBE_PUNCH_OUT);
    editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshCreate, m_EditMode == EDIT_MODE_ADD);
    editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshRotate, m_EditMode == EDIT_MODE_ROTATE);

    m_Instance->m_DrawLocator = false;

    if (m_EditMode == EDIT_MODE_ADD)
    {
      m_Instance->m_DrawLocator = true;
    }
    else if ( m_EditMode == EDIT_MODE_MOVE )
    {
      switch( m_MouseHoverSelectionMode )
      {
      case MOUSE_HOVER_SELECT_VERT:
        {
          if ( m_Instance->m_selected_verts.size() > 0 )
          {
            m_ManipulatorPos.t = m_Instance->GetVertPosition( m_Instance->m_selected_verts.front() );
          }
        }
        break;
      case MOUSE_HOVER_SELECT_EDGE:
      case MOUSE_HOVER_SELECT_TRI:
        {
          // set the manipulator position to be the average of all selected points
          u32 numVerts = 0;
          Math::Vector3 vertsAdded = Math::Vector3::Zero;
          std::vector< u32 > selected_verts = m_Instance->GetSelectedVerts();
          for( u32 i = 0; i < selected_verts.size(); i++ )
          {
            numVerts++;
            vertsAdded += m_Instance->GetVertPosition( selected_verts.at(i) );
          }

          m_ManipulatorPos.t = vertsAdded/numVerts;
        }
        break;
      }
    }
    else if ( m_EditMode == EDIT_MODE_ROTATE )
    {
      // set the manipulator position to be the average of all selected points
      u32 numVerts = 0;
      Math::Vector3 vertsAdded = Math::Vector3::Zero;
      std::vector< u32 > selected_verts = m_Instance->GetSelectedVerts();
      for( u32 i = 0; i < selected_verts.size(); i++ )
      {
        numVerts++;
        vertsAdded += m_Instance->GetVertPosition( selected_verts.at(i) );
      }

      m_ManipulatorPos = Math::Matrix4::Identity;
      m_ManipulatorPos.t = vertsAdded/numVerts;

      m_RotationStartValue = Math::Vector3::Zero;
      m_StartValue = Math::Vector3::Zero;

      m_Scene->Execute( true );
    }
    else if (m_EditMode == EDIT_MODE_CUBE_PUNCH_OUT)
    {
      m_PunchOutVolume->SetHidden( false );
    }
  }

  m_Scene->Execute( true );
}

void NavMeshCreateTool::MoveManipulator( wxMouseEvent& e )
{
  __super::MouseMove(e);

  // our delta drag vector
  Math::Vector3 drag;

  // our starting coordinate
  Math::Vector3 startPoint = m_StartValue; //Math::Vector3(m_ManipulatorPos.t.x, m_ManipulatorPos.t.y, m_ManipulatorPos.t.z);

  if ( (m_SelectedAxes == Math::MultipleAxes::All) && ( GetObjectSnap() || GetSurfaceSnap() ) ) // == TranslateSnappingModes::Surface || GetSnappingMode() == TranslateSnappingModes::Object || GetSnappingMode() == TranslateSnappingModes::Vertex ) )
  {
    // point to set
    Math::Vector3 result;
    PickPosition( e.GetX(), e.GetY(), result );

    switch (m_SelectedAxes)
    {
    case Math::MultipleAxes::X:
      {
        drag.x = result.x - startPoint.x;
        break;
      }

    case Math::MultipleAxes::Y:
      {
        drag.y = result.y - startPoint.y;
        break;
      }

    case Math::MultipleAxes::Z:
      {
        drag.z = result.z - startPoint.z;
        break;
      }

    case Math::MultipleAxes::X | Math::MultipleAxes::Y:
      {
        drag.x = result.x - startPoint.x;
        drag.y = result.y - startPoint.y;
        break;
      }

    case Math::MultipleAxes::Y | Math::MultipleAxes::Z:
      {
        drag.y = result.y - startPoint.y;
        drag.z = result.z - startPoint.z;
        break;
      }

    case Math::MultipleAxes::Z | Math::MultipleAxes::X:
      {
        drag.z = result.z - startPoint.z;
        drag.x = result.x - startPoint.x;
        break;
      }

    case Math::MultipleAxes::All:
      {
        drag = result - startPoint;
        break;
      }
    }

    // update our starting point for the next frame
    m_StartValue = result;
  }
  else
  {
    Math::Vector3 reference;

    // start out with global manipulator axes
    reference = GetAxesNormal(m_SelectedAxes);

    if (reference == Math::Vector3::Zero)
    {
      return;
    }


    //
    // Setup reference vector
    //

    bool linear = false;

    switch (m_SelectedAxes)
    {
    case Math::MultipleAxes::X:
    case Math::MultipleAxes::Y:
    case Math::MultipleAxes::Z:
      {
        linear = true;
        break;
      }
    }

    m_ManipulatorPos.TransformNormal( reference );

    // Pick ray from our starting location
    Math::Line startRay;
    m_View->GetCamera()->ViewportToLine(m_StartX, m_StartY, startRay);

    // Pick ray from our current location
    Math::Line endRay;
    m_View->GetCamera()->ViewportToLine(e.GetX(), e.GetY(), endRay);

    // start and end points of the drag in world space, on the line or on the plane
    Math::Vector3 p1, p2;

    if (linear)
    {
      //
      // Linear insersections of the rays with the selected reference line
      //

      if (!startRay.IntersectsLine(startPoint, startPoint + reference, &p1))
      {
        return;
      }

      if (!endRay.IntersectsLine(startPoint, startPoint + reference, &p2))
      {
        return;
      }
    }
    else
    {
      //
      // Planar intersections of the rays with the selected reference plane
      //

      if (!startRay.IntersectsPlane(Math::Plane (startPoint, reference), &p1))
      {
        return;
      }

      if (!endRay.IntersectsPlane(Math::Plane (startPoint, reference), &p2))
      {
        return;
      }
    }

    // drag vector
    drag = p2 - p1;
    m_StartValue = p2;
    m_StartX = e.GetX();
    m_StartY = e.GetY();
  }

  Math::Vector3 newPos = Math::Vector3( m_ManipulatorPos.t.x, m_ManipulatorPos.t.y, m_ManipulatorPos.t.z );
  newPos += drag;
  m_ManipulatorPos.t = newPos;

  switch(m_MouseHoverSelectionMode)
  {
  case MOUSE_HOVER_SELECT_VERT:
    {
      m_Instance->MoveSelectedVertsByOffset( drag );
    }
    break;
  case MOUSE_HOVER_SELECT_EDGE:
    {
      m_Instance->MoveSelectedEdgeByOffset( drag );
    }
    break;
  case MOUSE_HOVER_SELECT_TRI:
    {
      m_Instance->MoveSelectedTriByOffset( drag );
    }
    break;
  }
}

Math::Vector3 NavMeshCreateTool::GetAxesNormal(Math::AxesFlags axes)
{
  Math::Vector3 result;

  if (axes == Math::MultipleAxes::All)
  {
    m_View->GetCamera()->GetDirection(result);

    return result;
  }

  if ((axes & Math::MultipleAxes::X) != Math::MultipleAxes::None)
  {
    if ((axes & Math::MultipleAxes::Y) != Math::MultipleAxes::None)
      return Math::Vector3::BasisZ;

    if ((axes & Math::MultipleAxes::Z) != Math::MultipleAxes::None)
      return Math::Vector3::BasisY;

    return Math::Vector3::BasisX;
  }

  if ((axes & Math::MultipleAxes::Y) != Math::MultipleAxes::None)
  {
    if ((axes & Math::MultipleAxes::X) != Math::MultipleAxes::None)
      return Math::Vector3::BasisZ;

    if ((axes & Math::MultipleAxes::Z) != Math::MultipleAxes::None)
      return Math::Vector3::BasisX;

    return Math::Vector3::BasisY;
  }

  if ((axes & Math::MultipleAxes::Z) != Math::MultipleAxes::None)
  {
    if ((axes & Math::MultipleAxes::X) != Math::MultipleAxes::None)
      return Math::Vector3::BasisY;

    if ((axes & Math::MultipleAxes::Y) != Math::MultipleAxes::None)
      return Math::Vector3::BasisX;

    return Math::Vector3::BasisZ;
  }

  return result;
}


void NavMeshCreateTool::SetResolutionMode(u32 res_mode)
{
  if (m_ResMode == res_mode)
  {
    return;
  }
  else
  {
    m_ResMode = res_mode;
  }

  if ( m_Instance.ReferencesObject() )
  {
    m_Instance->ClearEditingState();
  }

  OS_HierarchyNodeDumbPtr::Iterator childItr = m_Scene->GetRoot()->GetChildren().Begin();
  OS_HierarchyNodeDumbPtr::Iterator childEnd = m_Scene->GetRoot()->GetChildren().End();
  for ( ; childItr != childEnd; ++childItr )
  {
    Luna::NavMesh* mesh = Reflect::ObjectCast< Luna::NavMesh >( *childItr );
    if ( mesh )
    {
      Content::Mesh* c_navMesh = mesh->GetPackage<Content::Mesh>();
      if (c_navMesh)
      {
        int a = (c_navMesh->m_MeshOriginType == Content::Mesh::NavHiRes);
        if ((c_navMesh->m_MeshOriginType == Content::Mesh::NavHiRes) && m_ResMode != RES_MODE_HIGH_RES)
        {
          continue;
        }
        if ((c_navMesh->m_MeshOriginType == Content::Mesh::NavLowRes) && m_ResMode != RES_MODE_LOW_RES)
        {
          continue;
        }
      }
      // We only have one navmesh per scene so use the one that is already here
      m_Instance = mesh;
      m_Instance->SetSelected( true );
      //m_Instance->SetTransient( true );
      m_Instance->Evaluate( GraphDirections::Downstream );
      return;
    }
  }

  CreateInstance( Math::Vector3::Zero );
}

bool NavMeshCreateTool::PickRotator( PickVisitor* pick )
{
  // get the transform for our object
  Math::Matrix4 m = m_ManipulatorPos;
  Math::Vector3 position = Math::Vector3 (m.t.x, m.t.y, m.t.z);

  // setup the pick object
  LinePickVisitor* linePick = dynamic_cast<LinePickVisitor*>(pick);
  linePick->SetCurrentObject (this, m);
  linePick->ClearHits();

  // amount of error allowed to cause a pick hit
  f32 pickRingError = m_Ring->m_Radius / 10.f;

  // pick for a one of the axis ring using the pick transformed into the local space of the object
  m_SelectedAxes = PickRing(pick, pickRingError);

  // if we did not get an axis, check for intersection of our camera plane ring
  if (m_SelectedAxes == Math::MultipleAxes::None)
  {
    float dist, min = m_Ring->m_Radius * 1.2f;
    float stepAngle = (float)(Math::TwoPi) / (float)(m_Ring->m_RadiusSteps);

    // rotation from the circle axis to the camera direction
    Math::Vector3 cameraPosition;
    m_View->GetCamera()->GetPosition(cameraPosition);
    Math::Matrix4 fixup = Math::Matrix4 (Math::AngleAxis::Rotation(Math::Vector3::BasisX, cameraPosition - position)) * Math::Matrix4 (position) * m.Inverted();//bring the fix up into local space

    // for each point on the ring
    for (int x=0; x<m_Ring->m_RadiusSteps; x++)
    {
      float theta = (float)(x) * stepAngle;

      Math::Vector3 v (0.0f,
        (float)(cos(theta)) * m_Ring->m_Radius * 1.2f,
        (float)(sin(theta)) * m_Ring->m_Radius * 1.2f);

      v += Math::Vector3 (0.0f,
        (float)(cos(theta + stepAngle)) * m_Ring->m_Radius * 1.2f,
        (float)(sin(theta + stepAngle)) * m_Ring->m_Radius * 1.2f);

      v *= 0.5f;

      // point it toward the camera
      fixup.TransformVertex(v);

      // perform intersection, storing a hit when we are within our error and it is the closest one so far
      if ( linePick->PickPoint(v, pickRingError) )
      {
        dist = linePick->GetHits().back()->GetIntersectionDistance();

        if (dist >= 0.0f && dist < min)
        {
          min = dist;
        }
      }
    }

    // if we got a hit
    if (min < pickRingError * 1.2f)
    {
      // set camera plane manipulation mode
      m_SelectedAxes = Math::MultipleAxes::All;
      m_Type = RotationTypes::CameraPlane;
    }
  }

  // if we STILL dont have an axis to rotate around
  if (m_SelectedAxes == Math::MultipleAxes::None)
  {
    // check for sphere intersection to perform arcball rotation   
    if( linePick->GetWorldSpaceLine().IntersectsSphere(position, m_Ring->m_Radius) )
    {
      m_SelectedAxes = Math::MultipleAxes::All;
      m_Type = RotationTypes::ArcBall;
    }
  }

  // set fallback type
  if (m_SelectedAxes != Math::MultipleAxes::All)
  {
    m_Type = RotationTypes::Normal;
  }

  if (m_SelectedAxes != Math::MultipleAxes::None)
  {
    return true;
  }
  else
  {
    return false;
  }
}

Math::AxesFlags NavMeshCreateTool::PickRing(PickVisitor* pick, float err)
{
  float radius = m_Ring->m_Radius;
  float dist, minX = Math::BigFloat, minY = Math::BigFloat, minZ = Math::BigFloat;
  float stepAngle = (float)Math::TwoPi / (float)(m_Ring->m_RadiusSteps);

  Math::Matrix4 m = m_ManipulatorPos;
  Math::Vector3 position = Math::Vector3 (m.t.x, m.t.y, m.t.z);

  Math::Vector3 cameraPosition;
  m_View->GetCamera()->GetPosition(cameraPosition);
  Math::Vector3 cameraVector = cameraPosition - position;

  pick->SetCurrentObject (this, m);

  for (int x=0; x<m_Ring->m_RadiusSteps; x++)
  {
    float theta = (float)(x) * stepAngle;

    Math::Vector3 v (0.0f,
      (float)(cos(theta)) * radius,
      (float)(sin(theta)) * radius);

    v += Math::Vector3 (0.0f,
      (float)(cos(theta + stepAngle)) * radius,
      (float)(sin(theta + stepAngle)) * radius);

    v *= 0.5f;

    Math::Vector3 transformed = v;
    m.TransformVertex(transformed);

    if ((transformed - position).Dot(cameraVector) >= 0.0f)
    {
      if (pick->PickPoint(v, err) )       
      {
        dist = pick->GetHits().back()->GetIntersectionDistance();
        if (dist > 0.0f && dist < minX)
        {
          minX = dist;
        }
      }
    }
  }

  for (int x=0; x<m_Ring->m_RadiusSteps; x++)
  {
    float theta = (float)(x) * stepAngle;

    Math::Vector3 v ((float)(cos(theta)) * radius,
      0.0f,
      (float)(sin(theta)) * radius);

    v += Math::Vector3 ((float)(cos(theta + stepAngle)) * radius,
      0.0f,
      (float)(sin(theta + stepAngle)) * radius);

    v *= 0.5f;

    Math::Vector3 transformed = v;
    m.TransformVertex(transformed);

    if ((transformed - position).Dot(cameraVector) >= 0.0f)
    {
      if (pick->PickPoint(v, err))
      {
        dist = pick->GetHits().back()->GetIntersectionDistance();
        if (dist > 0.0f && dist < minY)
        {
          minY = dist;
        }
      }
    }
  }

  for (int x=0; x<m_Ring->m_RadiusSteps; x++)
  {
    float theta = (float)(x) * stepAngle;

    Math::Vector3 v ((float)(cos(theta)) * radius,
      (float)(sin(theta)) * radius,
      0.0f);

    v += Math::Vector3 ((float)(cos(theta + stepAngle)) * radius,
      (float)(sin(theta + stepAngle)) * radius,
      0.0f);

    v *= 0.5f;

    Math::Vector3 transformed = v;
    m.TransformVertex(transformed);

    if ((transformed - position).Dot(cameraVector) >= 0.0f)
    {
      if (pick->PickPoint(v, err))
      {
        dist = pick->GetHits().back()->GetIntersectionDistance();
        if (dist > 0.0f && dist < minZ)
        {      
          minZ = dist;
        }
      }
    }
  }

  if ((minX == minY) && (minY == minZ))
  {
    return Math::MultipleAxes::None;
  }

  if (minX <= minY && minX <= minZ)
  {
    return Math::MultipleAxes::X;
  }

  if (minY <= minX && minY <= minZ)
  {
    return Math::MultipleAxes::Y;
  }

  if (minZ <= minX && minZ <= minY)
  {
    return Math::MultipleAxes::Z;
  }

  return Math::MultipleAxes::None;
}

void NavMeshCreateTool::MoveRotator( wxMouseEvent& e )
{
  __super::MouseMove(e);

  Math::Vector3 startPoint;
  m_ManipulatorPos.TransformVertex(startPoint);

  Math::Vector3 cameraPosition;
  m_View->GetCamera()->ViewportToWorldVertex(e.GetX(), e.GetY(), cameraPosition);


  //
  // Compute our reference vector in global space, from the object
  //  This is an axis normal (direction) for single axis manipulation, or a plane normal for multi-axis manipulation
  //

  // start out with global manipulator axes
  Math::Vector3 reference = GetAxesNormal(m_SelectedAxes);

  switch (m_SelectedAxes)
  {
  case Math::MultipleAxes::X:
  case Math::MultipleAxes::Y:
  case Math::MultipleAxes::Z:
    {
      // use local axes to manipulate
      m_ManipulatorPos.TransformNormal(reference);
      break;
    }
  }

  if (m_SelectedAxes != Math::MultipleAxes::All && reference == Math::Vector3::Zero)
  {
    return;
  }

  // Pick ray from our starting location
  Math::Line startRay;
  m_View->GetCamera()->ViewportToLine(m_StartX, m_StartY, startRay);

  // Pick ray from our current location
  Math::Line endRay;
  m_View->GetCamera()->ViewportToLine(e.GetX(), e.GetY(), endRay);

  // Our from and to vectors for angle axis rotation about a rotation plane
  Math::Vector3 p1, p2;
  Math::Vector3 intersection;

  if (m_SelectedAxes != Math::MultipleAxes::All)
  {
    //
    // Axis-Specific
    //

    p1 = reference;
    p1.Normalize();
    p2 = startPoint - cameraPosition;
    p2.Normalize();

    float dot = p1.Dot(p2);
    bool lowAngle = fabs(dot) < 0.15;

    // if our ray intersects the manipulation sphere
    if (lowAngle && ClosestSphericalIntersection(startRay, startPoint, m_Ring->m_Radius, cameraPosition, intersection))
    {
      // Project onto rotation plane
      Math::Line projection = Math::Line (intersection, intersection + reference);
      projection.IntersectsPlane(Math::Plane (startPoint, reference), &p1);
    }
    // else we are outside the manpiulation sphere
    else if (!lowAngle)
    {
      // Intersection with rotation plane
      if (!startRay.IntersectsPlane(Math::Plane (startPoint, reference), &p1))
      {
        return;
      }
    }
    else
    {
      return;
    }

    // if our ray intersects the manipulation sphere
    if (lowAngle && ClosestSphericalIntersection(endRay, startPoint, m_Ring->m_Radius, cameraPosition, intersection))
    {
      // Project onto rotation plane
      Math::Line projection = Math::Line (intersection, intersection + reference);
      projection.IntersectsPlane(Math::Plane (startPoint, reference), &p2);
    }
    // else we are outside the manpiulation sphere
    else if (!lowAngle)
    {
      // Intersection with rotation plane
      if (!endRay.IntersectsPlane(Math::Plane (startPoint, reference), &p2))
      {
        return;
      }
    }
    else
    {
      return;
    }
  }
  else
  {
    //
    // ArcBall and View Plane
    //

    if (m_Type == RotationTypes::ArcBall)
    {
      if (ClosestSphericalIntersection(startRay, startPoint, m_Ring->m_Radius, cameraPosition, intersection))
      {
        p1 = intersection;
      }
      else
      {
        return;
      }

      if (ClosestSphericalIntersection(endRay, startPoint, m_Ring->m_Radius, cameraPosition, intersection))
      {
        p2 = intersection;
      }
      else
      {
        return;
      }
    }
    else
    {
      // we are rotating in the camera plane, get the reference vector (camera dir)
      m_View->GetCamera()->GetDirection(reference);

      // Intersection with rotation plane
      if (!startRay.IntersectsPlane(Math::Plane (startPoint, reference), &p1))
      {
        return;
      }

      // Intersection with rotation plane
      if (!endRay.IntersectsPlane(Math::Plane (startPoint, reference), &p2))
      {
        return;
      }
    }
  }


  //
  // Now we have our to/from vectors, get the differential rotation around reference by the angle betweeen them
  //

  Math::Vector3 a = p1 - startPoint;
  Math::Vector3 b = p2 - startPoint;

  a.Normalize();
  b.Normalize();

  float angle = (float)(acos(a.Dot(b)));

  // m_ArcBall rotation axis is the axis from vector a to vector b
  if (m_SelectedAxes == Math::MultipleAxes::All && m_Type == RotationTypes::ArcBall)
  {
    reference = a.Cross(b);
    reference.Normalize();
  }

  // always spin the right way, regardless of vector orientation
  if (reference.Dot(a.Cross(b)) < 0.0f)
  {
    angle = -angle;
  }

  switch (m_SelectedAxes)
  {
  case Math::MultipleAxes::X:
  case Math::MultipleAxes::Y:
  case Math::MultipleAxes::Z:
    if ( m_AxisSnap )
    {
      float minAngle = m_SnapDegrees * Math::DegToRad;
      float absAngle = fabs( angle );
      int count = absAngle / minAngle;
      if ( angle < 0.0f )
      {
        count = -count;
      }
      angle = count * minAngle;
    }
  }

  // perform rotation
  Math::Matrix4 rotation = Math::Matrix4 (Math::AngleAxis (angle, reference));

  //
  // Set Value
  //

  // append the current to the starting rotation
  Math::EulerAngles eulerAngle(m_RotationStartValue);
  Math::Matrix4 total_rotation( eulerAngle );
  total_rotation *= rotation;

  Math::Matrix4 cached_pos = m_ManipulatorPos;

  m_StartValue = Math::EulerAngles(total_rotation).angles;
  m_ManipulatorPos = Math::EulerAngles(total_rotation); 
  m_ManipulatorPos.t = cached_pos.t;

  // Apply the rotation to the selected verts
  std::vector< u32 > selected_verts = m_Instance->GetSelectedVerts();
  for ( u32 i = 0; i < selected_verts.size(); i++ )
  {
    Math::Vector3 new_pos = m_CachedPositions.at(i);
    new_pos.x -= m_ManipulatorPos.t.x;
    new_pos.y -= m_ManipulatorPos.t.y;
    new_pos.z -= m_ManipulatorPos.t.z;
    rotation.TransformVertex( new_pos );
    new_pos.x += m_ManipulatorPos.t.x;
    new_pos.y += m_ManipulatorPos.t.y;
    new_pos.z += m_ManipulatorPos.t.z;
    m_Instance->UpdateVert( selected_verts.at(i), new_pos );
  }

  // apply modification
  m_Scene->Execute(true);
}

bool NavMeshCreateTool::ClosestSphericalIntersection(Math::Line line, Math::Vector3 spherePosition, float sphereRadius, Math::Vector3 cameraPosition, Math::Vector3& intersection)
{
  Math::V_Vector3 intersections;

  // if our ray intersects the sphere
  if (line.IntersectsSphere(spherePosition, sphereRadius, &intersections))
  {
    // Get point on sphere
    Math::Vector3 closest = intersections[0];

    if (intersections.size() > 1)
    {
      if ((intersections[0] - cameraPosition).Length() < (intersections[1] - cameraPosition).Length())
      {
        closest = intersections[0];
      }
      else
      {
        closest = intersections[1];
      }
    }

    intersection = closest;

    return true;
  }

  intersection = Math::Vector3::Zero;

  return false;
}

void NavMeshCreateTool::SetPunchOutMode( u32 mode ) 
{ 
  m_PunchOutEditMode = mode; 

  SceneEditor* editor = (SceneEditor*)(m_Scene->GetManager()->GetEditor());
  editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshPunchOut, m_PunchOutEditMode == EDIT_MODE_DISABLED);
  editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshPunchOutTranslate, m_PunchOutEditMode == EDIT_MODE_MOVE);
  editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshPunchOutRotate, m_PunchOutEditMode == EDIT_MODE_ROTATE);
  editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshPunchOutScale, m_PunchOutEditMode == EDIT_MODE_SCALE);

  if ( mode == EDIT_MODE_DISABLED )
  {
    // clear the selection from the scene but we still want the volume to draw hilighted
    m_Scene->GetSelection().Clear();
    m_PunchOutVolume->SetSelected( true );
  }
  else
  {
    // Make sure the punch out volume is the only thing selected in the scene
    m_Scene->GetSelection().Clear();

    OS_SelectableDumbPtr selected;
    selected.Append( m_PunchOutVolume );

    m_Scene->GetSelection().SetItems( selected );
  }
}

void NavMeshCreateTool::SetHoverSelectMode( u32 mode )
{
  m_MouseHoverSelectionMode = mode;

  if ( m_Instance )
  {
    m_Instance->ClearEditingState();
    m_Instance->SetSelectionMode( m_MouseHoverSelectionMode );
    m_Scene->Execute( true );
  }

  SceneEditor* editor = (SceneEditor*)(m_Scene->GetManager()->GetEditor());
  editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshVertexSelect, mode == MOUSE_HOVER_SELECT_VERT);
  editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshEdgeSelect, mode == MOUSE_HOVER_SELECT_EDGE);
  editor->GetNavToolBar()->ToggleTool( SceneEditorIDs::ID_ToolsNavMeshTriSelect, mode == MOUSE_HOVER_SELECT_TRI);
}

void NavMeshCreateTool::CopySelected()
{
  m_CopiedPositions.clear();

  std::vector< u32 > selected_verts = m_Instance->GetSelectedVerts();

  for ( u32 i = 0; i < selected_verts.size(); i++ )
  {
    m_CopiedPositions.push_back( m_Instance->GetVertPosition( selected_verts.at(i) ) );
  }
}

void NavMeshCreateTool::Paste()
{
  Undo::BatchCommandPtr batch = new Undo::BatchCommand();
  batch->Push( new AddNavMeshCommand( m_Instance, m_CopiedPositions, AddNavMeshCommand::kPaste ));
  m_Scene->Push( batch );
}
