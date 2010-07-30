#include "Precompile.h"
#include "Curve.h"

#include "Pick.h"
#include "PrimitiveLocator.h"
#include "PrimitiveCone.h"
#include "Color.h"
#include "Scene.h"
#include "Statistics.h"
#include "HierarchyNodeType.h"
#include "ReverseChildrenCommand.h"

#include "Application/UI/ArtProvider.h"
#include "PropertiesGenerator.h"
#include "Application/Undo/PropertyCommand.h"
#include "Foundation/Log.h"
#include "Orientation.h"

#include "Foundation/Math/Curve.h"
#include "Foundation/Math/AngleAxis.h"

#include <algorithm>

using namespace Helium;
using namespace Helium::Editor;

EDITOR_DEFINE_TYPE( Editor::Curve );

D3DMATERIAL9 Editor::Curve::s_Material;
D3DMATERIAL9 Editor::Curve::s_HullMaterial;


class SelectionDataObject : public Reflect::Object
{
public:

  OS_SelectableDumbPtr m_Selection;

  SelectionDataObject( const OS_SelectableDumbPtr& selection )
    : m_Selection( selection)
  {
  }

};


void Curve::InitializeType()
{
  Reflect::RegisterClass< Editor::Curve >( TXT( "Editor::Curve" ) );

  ZeroMemory(&s_Material, sizeof(s_Material));
  s_Material.Ambient = Editor::Color::FORESTGREEN;

  ZeroMemory(&s_HullMaterial, sizeof(s_HullMaterial));
  s_HullMaterial.Ambient = Editor::Color::GRAY;

  PropertiesGenerator::InitializePanel( TXT( "Curve" ), CreatePanelSignature::Delegate( &Curve::CreatePanel ) );
}

void Curve::CleanupType()
{
  Reflect::UnregisterClass< Editor::Curve >();
}

Curve::Curve( Editor::Scene* scene, Content::Curve* curve )
: Editor::PivotTransform ( scene, curve )
, m_Locator ( NULL )
, m_Cone ( NULL )
{
  m_Locator = new Editor::PrimitiveLocator( m_Scene->GetViewport()->GetResources() );
  m_Locator->Update();

  m_Cone = new Editor::PrimitiveCone( m_Scene->GetViewport()->GetResources() );
  m_Cone->m_Radius = 0.2f;
  m_Cone->SetSolid(true);
  m_Cone->Update();

  m_Vertices = new VertexResource ( scene->GetViewport()->GetResources() );
  m_Vertices->SetElementType( ElementTypes::Position );
  m_Vertices->SetPopulator( PopulateSignature::Delegate( this, &Curve::Populate ) );
}

Curve::~Curve()
{
  delete m_Locator;
  delete m_Cone;
}

i32 Curve::GetImageIndex() const
{
  return Helium::GlobalFileIconsTable().GetIconID( TXT( "curve" ) );
}

tstring Curve::GetApplicationTypeName() const
{
  return TXT( "Curve" );
}

void Curve::Initialize()
{
  __super::Initialize();

  OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
  OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
  for ( ; childItr != childEnd; ++childItr )
  {
    Editor::Point* point = Reflect::ObjectCast< Editor::Point >( *childItr );
    if ( point )
    {
      point->AddParentChangingListener( ParentChangingSignature::Delegate( this, &Curve::ChildChangingParents ) );
    }
  }
}

int Curve::GetCurveType() const 
{ 
  return GetPackage<Content::Curve>()->m_Type;
}

void Curve::SetCurveType( int value )
{
  GetPackage<Content::Curve>()->m_Type = (Content::CurveType)value;

  Dirty();
}

bool Curve::GetClosed() const
{  
  return GetPackage<Content::Curve>()->m_Closed;
}

void Curve::SetClosed( bool value )
{
  GetPackage<Content::Curve>()->m_Closed = value;

  Dirty();
}

u32 Curve::GetResolution() const
{
  return GetPackage<Content::Curve>()->m_Resolution;
}

void Curve::SetResolution( u32 value )
{
  GetPackage<Content::Curve>()->m_Resolution = value;

  Dirty();
}

int Curve::GetControlPointLabel() const
{
  return GetPackage<Content::Curve>()->m_ControlPointLabel;
}

void Curve::SetControlPointLabel( int value )
{
  GetPackage<Content::Curve>()->m_ControlPointLabel = (Content::ControlPointLabel)value;
  Dirty();
}

int Curve::ClosestControlPoint( PickVisitor* pick )
{
  int bestIndex = -1;
  float distance = 0.0f;
  float bestDistance = 0.0f;

  pick->SetCurrentObject (this, GetGlobalTransform(), GetInverseGlobalTransform() );

  OS_HierarchyNodeDumbPtr::Iterator itr = GetChildren().Begin();
  OS_HierarchyNodeDumbPtr::Iterator end = GetChildren().End();
  for ( u32 i = 0; itr != end; ++itr, ++i )
  {
    Editor::Point* point = Reflect::ObjectCast< Editor::Point >( *itr );
    if ( point )
    {
      if ( pick->PickPoint( point->GetPosition(), Math::BigFloat ) )
      {
        distance = pick->GetHits().back()->GetIntersectionDistance();

        if (bestIndex == -1 || distance < bestDistance )
        {
          bestDistance = distance;
          bestIndex = i;
        }
      }
    }
  }

  return bestIndex;
}

bool Curve::ClosestControlPoints( PickVisitor* pick, std::pair<u32, u32>& result )
{
  Editor::Point* previous = NULL;
  Editor::Point* current = NULL;
  i32 previousIndex = -1;
  i32 currentIndex = -1;
  i32 bestIndex = -1;
  i32 bestPreviousIndex = -1;
  f32 distance = 0.0f;
  f32 bestDistance = 0.0f;

  // need to have at least 2 points!
  if ( GetNumberControlPoints() < 2 )
  {
    return false;
  }

  pick->SetCurrentObject ( this, GetGlobalTransform(), GetInverseGlobalTransform() );

  OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
  OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
  for ( u32 index = 0; childItr != childEnd; ++childItr )
  {
    current = Reflect::ObjectCast< Editor::Point >( *childItr );
    if ( current )
    {
      currentIndex = index;
      if ( previous )
      {
        Math::Vector3 point1 = previous->GetPosition();
        Math::Vector3 point2 = current->GetPosition();

        if ( pick->PickSegment( point1, point2, -1.0f ) )
        {
          distance = pick->GetHits().back()->GetIntersectionDistance();

          if ( bestIndex == -1 || distance < bestDistance )
          {
            bestDistance = distance;
            bestIndex = currentIndex;
            bestPreviousIndex = previousIndex;
          }
        }
      }

      previousIndex = currentIndex;
      previous = current;
      ++index;
    }
  }

  if ( bestIndex != -1 ) 
  {
    result.first = bestPreviousIndex;
    result.second = bestIndex;
    return true;
  }

  return false;
}

i32 Curve::ClosestPoint( PickVisitor* pick )
{
  i32 bestIndex = -1;
  f32 distance = Math::BigFloat;
  f32 bestDistance = Math::BigFloat;
  Math::Vector3 point;

  pick->SetCurrentObject ( this, GetGlobalTransform(), GetInverseGlobalTransform() );

  Content::Curve* curve = GetPackage<Content::Curve>();

  u32 curvePointCount = (u32)curve->m_Points.size();
  for ( u32 i = 0; i < curvePointCount; ++i )
  {
    point = curve->m_Points[i];  

    if ( pick->PickPoint (point, Math::BigFloat ) )
    {
      distance = pick->GetHits().back()->GetIntersectionDistance();

      if ( bestIndex == -1 || distance < bestDistance )
      {
        bestDistance = distance;
        bestIndex = i;
      }
    }
  }

  return bestIndex;
}

i32 Curve::ClosestPoint(Math::Vector3& pos)
{
  int bestIndex = -1;
  float distance = 0.0f;
  float bestDistance = 0.0f;
  Math::Vector3 point;

  const Math::Matrix4& globalTransform = this->GetGlobalTransform();
  Content::Curve* curve = GetPackage<Content::Curve>();

  Math::V_Vector3::iterator itr = curve->m_Points.begin();
  Math::V_Vector3::iterator end  = curve->m_Points.end();
  for ( u32 i = 0; itr != end; ++itr, ++i )
  {
    point = *itr;
    globalTransform.TransformVertex( point ); 

    distance = (pos - point).Length();

    if ( bestIndex == -1 || distance < bestDistance )
    {
      bestDistance = distance;
      bestIndex = i;
    }
  }

  return bestIndex;
}

u32 Curve::GetNumberControlPoints() const
{
  u32 count = 0;
  OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
  OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
  for ( ; childItr != childEnd; ++childItr )
  {
    if ( ( *childItr )->HasType( Reflect::GetType< Editor::Point >() ) )
    {
      ++count;
    }
  }

  return count;
}

Editor::Point* Curve::GetControlPointByIndex( u32 index )
{
  Editor::Point* controlPoint = NULL;

  OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
  OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
  for ( u32 i = 0; childItr != childEnd; ++childItr )
  {
    Editor::Point* point = Reflect::ObjectCast< Editor::Point >( *childItr );
    if ( point )
    {
      if ( i == index )
      {
        controlPoint = point;
        break;
      }
      ++i;
    }
  }

  return controlPoint;
}

i32 Curve::GetIndexForControlPoint( Editor::Point* pc )
{
  i32 index = -1;

  OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
  OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
  for ( u32 i = 0; childItr != childEnd; ++childItr )
  {
    Editor::Point* point = Reflect::ObjectCast< Editor::Point >( *childItr );
    if ( point )
    {
      if ( point == pc )
      {
        index = i;
        break;
      }
      ++i;
    }
  }

  return index;
}

Undo::CommandPtr Curve::RemoveControlPointAtIndex( u32 index )
{
  OS_HierarchyNodeDumbPtr::Iterator childItr = m_Children.Begin();
  OS_HierarchyNodeDumbPtr::Iterator childEnd = m_Children.End();
  for ( u32 i = 0; childItr != childEnd; ++childItr )
  {
    Editor::Point* point = Reflect::ObjectCast< Editor::Point >( *childItr );
    if ( point )
    {
      if ( i == index )
      {
        Undo::CommandPtr command = new SceneNodeExistenceCommand( Undo::ExistenceActions::Remove, m_Scene, point );
        Dirty();
        return command;
      }
      ++i;
    }
  }
  return NULL;
}

Undo::CommandPtr Curve::InsertControlPointAtIndex( u32 index, Editor::Point* pc )
{
  Editor::HierarchyNode* previous = NULL;
  Editor::HierarchyNode* next = NULL;
  OS_HierarchyNodeDumbPtr::Iterator childItr = m_Children.Begin();
  OS_HierarchyNodeDumbPtr::Iterator childEnd = m_Children.End();
  for ( u32 i = 0; childItr != childEnd; ++childItr )
  {
    Editor::Point* point = Reflect::ObjectCast< Editor::Point >( *childItr );
    if ( point )
    {
      if ( i == index )
      {
        next = point;
        break;
      }
      ++i;
    }
    previous = *childItr;
  }

  pc->SetPrevious( previous );
  pc->SetNext( next );
  pc->SetParent( this );

  Undo::CommandPtr command = new SceneNodeExistenceCommand( Undo::ExistenceActions::Add, m_Scene, pc );
  Dirty();
  return command;
}

Undo::CommandPtr Curve::ReverseControlPoints()
{
  return new ReverseChildrenCommand( this );
}

void Curve::Create()
{
  __super::Create();

  m_Vertices->Create();
  m_Locator->Create();
  m_Cone->Create();
}

void Curve::Delete()
{
  __super::Delete();

  // If the curve is added back, it's control points might not be added yet,
  // so we need to zero the element count and it will be recalculated in 
  // Evaluate(), after all the control points are re-added to the scene.
  // If we don't do this, there will be an assert about a buffer underrun in
  // Populate().
  m_Vertices->SetElementCount( 0 );
  m_Vertices->Delete();

  m_Locator->Delete();
  m_Cone->Delete();
}

void Curve::Populate( PopulateArgs* args )
{
  Content::Curve* curve = GetPackage<Content::Curve>();

  switch( args->m_Type )
  {
  case ResourceTypes::Vertex:
    {
      if ( args->m_Buffer != NULL )
      {
        m_ObjectBounds.Reset();

        const u32 vertexCount = m_Vertices->GetElementCount();

        if ( vertexCount == 0 )
          break;

        Position* bufferStart = reinterpret_cast<Position*>( args->m_Buffer + args->m_Offset );
        
        // go over the control points
        u32 countControlPoints = 0;
        Editor::Point* firstPoint = NULL;
        OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
        OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
        for ( ; childItr != childEnd; ++childItr )
        {
          Editor::Point* point = Reflect::ObjectCast< Editor::Point >( *childItr );
          if ( point )
          {
            if ( !firstPoint )
            {
              firstPoint = point;
            }
            bufferStart[countControlPoints].m_Position = m_ObjectBounds.Test( point->GetPosition() );
            args->m_Offset += sizeof(Position); 
            ++countControlPoints;
          }
        }

        // loop back for closed curves
        if ( countControlPoints > 0 ) 
        {
          bufferStart[countControlPoints].m_Position = m_ObjectBounds.Test( firstPoint->GetPosition() );
          args->m_Offset += sizeof(Position); 
        }

        // reset the buffer start to its new location
        bufferStart = reinterpret_cast<Position*>( args->m_Buffer + args->m_Offset );
        
        // go over the calculated curve points
        u32 countCurvePoints = (u32)curve->m_Points.size();
        for ( u32 i = 0; i < countCurvePoints; ++i )
        {
          bufferStart[i].m_Position = m_ObjectBounds.Test( curve->m_Points[ i ] );
          args->m_Offset += sizeof(Position); 
        }

        // loop back for closed curves
        if ( countCurvePoints > 0 ) 
        {
          bufferStart[countCurvePoints].m_Position = m_ObjectBounds.Test( curve->m_Points[ 0 ] );        
          args->m_Offset += sizeof(Position); 
        }
      }
      break;
    }
  }
}

Undo::CommandPtr Curve::CenterTransform()
{
  Undo::BatchCommandPtr batch = new Undo::BatchCommand();

  batch->Push( __super::CenterTransform() );

  if ( GetNumberControlPoints() == 0 )
  {
    return batch;
  }


  //
  // We are going to move all control points, so just snap shot
  //

  batch->Push( SnapShot() );


  //
  // Compute the centered position
  //

  Math::Vector3 position = Math::Vector3::Zero;

  {
    u32 controlPointCount = 0;
    OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
    OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
    for ( ; childItr != childEnd; ++childItr )
    {
      Editor::Point* point = Reflect::ObjectCast< Editor::Point >( *childItr );
      if ( point )
      {
        ++controlPointCount;
        position += point->GetPosition();
      }
    }
    position /= controlPointCount;
  }

  m_GlobalTransform.TransformVertex( position );


  //
  // Offset the control points
  //

  Math::Matrix4 m = m_GlobalTransform;

  m.t = position;

  m = m_GlobalTransform * m.Inverted();

  {
    OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
    OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
    for ( ; childItr != childEnd; ++childItr )
    {
      Editor::Point* point = Reflect::ObjectCast< Editor::Point >( *childItr );
      if ( point )
      {
        Math::Vector3 p = point->GetPosition();

        m.TransformVertex( p );

        batch->Push( new Undo::PropertyCommand< Math::Vector3 >( new Helium::MemberProperty< Editor::Point, Math::Vector3 >( point, &Editor::Point::GetPosition, &Editor::Point::SetPosition ), p ) );
      }
    }
  }


  //
  // Recompute global transform
  //

  // our new global transform is just translated to the new position
  m_GlobalTransform.t = position;

  // this will recompute the local components
  SetGlobalTransform( m_GlobalTransform );

  // update the transform object
  Evaluate( GraphDirections::Downstream );

  // update each child's local transform to stay in the same global position
  for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
  {
    Editor::HierarchyNode* n = *itr;

    Editor::Transform* t = Reflect::ObjectCast<Editor::Transform>( n );

    if ( !t )
    {
      continue;
    }

    batch->Push( t->ComputeObjectComponents() );
  }

  Dirty();

  return batch;
}

f32 Curve::CalculateCurveLength() const
{
  const Math::Matrix4& globalTransform = this->GetGlobalTransform();
  Math::V_Vector3 points = GetPackage<Content::Curve>()->m_Points;
  f32 curveLength = 0.f;
  for ( u32 i = 1; i < points.size() ; ++i )
  {
    Math::Vector3 endPoint = points[i];
    Math::Vector3 startPoint = points[i-1];
    globalTransform.TransformVertex( endPoint );
    globalTransform.TransformVertex( startPoint );
    curveLength += (endPoint - startPoint).Length();
  }
  return curveLength;
}

void Curve::Evaluate( GraphDirection direction )
{
  Content::Curve* curve = GetPackage<Content::Curve>();

  u32 controlCount = 0;
  Math::V_Vector3 points;
  {
    OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
    OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
    for ( ; childItr != childEnd; ++childItr )
    {
      Editor::Point* point = Reflect::ObjectCast< Editor::Point >( *childItr );
      if ( point )
      {
        points.push_back( point->GetPosition() );
        ++controlCount;
      }
    }
  }

  if ( controlCount < 4  || curve->m_Type == Content::CurveTypes::Linear ) 
  {     
    curve->m_Points = points;
  }
  else if ( curve->m_Type == Content::CurveTypes::BSpline )
  {
    Math::Curve::ComputeCurve( points, curve->m_Resolution, curve->m_Closed, Math::Curve::kBSpline, curve->m_Points ); 
  }
  else if ( curve->m_Type == Content::CurveTypes::CatmullRom )
  {
    Math::Curve::ComputeCurve( points, curve->m_Resolution, curve->m_Closed, Math::Curve::kCatmullRom, curve->m_Points ); 
  }
  else
  {
    //whoa, did we get a bum curve type?
    HELIUM_BREAK();
  }


  //
  // Update buffer
  //

  if ( controlCount > 0 ) 
  {
    controlCount++;
  }

  u32 pointCount = (u32)curve->m_Points.size();
  if ( pointCount > 0 ) 
  {
    pointCount++;
  }

  m_Vertices->SetElementCount( controlCount + pointCount );
  m_Vertices->Update();

  __super::Evaluate(direction);
}

void Curve::Render( RenderVisitor* render )
{
  RenderEntry* entry = render->Allocate(this);

  entry->m_Location = render->State().m_Matrix;
  entry->m_Center = m_ObjectBounds.Center();
  entry->m_Draw = &Curve::Draw;

  __super::Render( render );
}

void Curve::Draw( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Editor::HierarchyNode* node = Reflect::ConstAssertCast<Editor::HierarchyNode>( object );
  const Editor::Curve* curve = Reflect::ConstAssertCast< Editor::Curve > ( node );
  const Content::Curve* data = curve->GetPackage<Content::Curve>();

  const VertexResource* vertices = curve->m_Vertices;

  Editor::Viewport* view = node->GetScene()->GetViewport();
  Editor::Camera* camera = view->GetCamera();

  u32 countCurvePoints    = (u32) data->m_Points.size();
  u32 countControlPoints  = curve->GetNumberControlPoints();

  //
  //  Draw start end end locators
  //

  curve->SetMaterial( curve->s_Material );

  const Math::Matrix4& globalTransform = curve->GetGlobalTransform();

  if ( !data->m_Closed )
  {
    Math::Matrix4 m;

    if ( countCurvePoints > 0 )
    {
      m = Math::Matrix4( data->m_Points[ 0 ] ) * globalTransform;
      device->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&m );
      curve->m_Locator->Draw( args );
    }

    if ( countCurvePoints > 1 )
    {
      m = Math::Matrix4( data->m_Points[ countCurvePoints - 1 ] ) * globalTransform;
      device->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&m );
      curve->m_Locator->Draw( args );

      Math::Vector3 p1 = data->m_Points[ countCurvePoints - 2 ];
      Math::Vector3 p2 = data->m_Points[ countCurvePoints - 1 ];
      Math::Vector3 dir = (p2 - p1).Normalized();
      m = Math::Matrix4 ( Math::AngleAxis::Rotation( OutVector, dir ) );
      m.t = p2 - (dir * (curve->m_Cone->m_Length / 2.0f));
      m *= globalTransform;

      device->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&m );
      curve->m_Cone->Draw( args );
    }
  }

  if ( !vertices->SetState() )
  {
    return;
  }

  device->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&globalTransform );

  if ( countCurvePoints > 0 ) 
  {
    //
    //  Draw Curve
    //
    u32 countCurveLines = data->m_Closed ? countCurvePoints : countCurvePoints - 1;

    if ( countCurveLines > 0 )
    {
      device->DrawPrimitive( D3DPT_LINESTRIP, (u32)vertices->GetBaseIndex() + countControlPoints + 1, countCurveLines );
      args->m_LineCount += countCurveLines;
    }

    //
    //  Draw Curve points 
    //
    static float curvePointSize = 5.0f;
    device->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
    device->SetRenderState( D3DRS_POINTSIZE, *( (DWORD*) &curvePointSize ) );
    device->DrawPrimitive( D3DPT_POINTLIST, (u32)vertices->GetBaseIndex() + countControlPoints + 1, countCurvePoints );
    device->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
  }


  //
  //  Draw Control points
  //

  if ( countControlPoints > 0 )
  {
    //
    // Draw points hull
    //

    if ( data->m_Type != Content::CurveTypes::Linear )
    {
      u32 countControlLines = data->m_Closed ? countControlPoints : countControlPoints - 1;
      device->SetMaterial( &curve->s_HullMaterial );

      if ( countControlLines > 0 )
      {
        device->DrawPrimitive( D3DPT_LINESTRIP, (u32)vertices->GetBaseIndex(), countControlLines  );
        args->m_LineCount += countControlLines;
      }
    }


    //
    // Draw all points
    //

    static float controlPointSize = 5.0f;
    device->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
    device->SetMaterial( &Editor::Viewport::s_ComponentMaterial );
    device->DrawPrimitive( D3DPT_POINTLIST, (u32)vertices->GetBaseIndex(), countControlPoints );


    //
    //  Overdraw selected points
    //
    {
      Editor::Camera* camera = curve->GetScene()->GetViewport()->GetCamera();
      const Math::Matrix4& viewMatrix = camera->GetViewport();
      const Math::Matrix4& projMatrix = camera->GetProjection();
      ID3DXFont* font = curve->GetScene()->GetViewport()->GetStatistics()->GetFont();
      DWORD color = D3DCOLOR_ARGB(255, 255, 255, 255);
      tchar textBuf[256];

      device->SetMaterial( &Editor::Viewport::s_SelectedComponentMaterial );
      OS_HierarchyNodeDumbPtr::Iterator childItr = curve->GetChildren().Begin();
      OS_HierarchyNodeDumbPtr::Iterator childEnd = curve->GetChildren().End();
      for ( u32 i = 0; childItr != childEnd; ++childItr )
      {
        Editor::Point* point = Reflect::ObjectCast< Editor::Point >( *childItr );
        if ( point )
        {
          if ( point->IsSelected() )
          {
            device->DrawPrimitive( D3DPT_POINTLIST, (u32)vertices->GetBaseIndex() + i, 1 );
          }

          if ( curve->GetControlPointLabel() != Content::ControlPointLabels::None )
          {
            tstringstream label;
            switch ( curve->GetControlPointLabel() )
            {
            case Content::ControlPointLabels::CurveAndIndex:
              label << curve->GetName() << TXT( "[" ) << i << TXT( "]" );
              break;

            case Content::ControlPointLabels::IndexOnly:
              label << "[" << i << "]";
              break;
            }

            Math::Vector3 position ( point->GetPosition() );

            // local to global
            curve->GetGlobalTransform().TransformVertex( position );

            // map to screen space
            float screenX = 0.0f;
            float screenY = 0.0f;
            camera->WorldToScreen( position, screenX, screenY );

            const float offsetX = 0;
            const float offsetY = 15;

            RECT rect;
            rect.top = screenY - offsetY;
            rect.left = screenX - offsetX;
            rect.right = screenX;
            rect.bottom = screenY - 2;

            _stprintf_s( textBuf, sizeof( textBuf ), label.str().c_str(), i );
            font->DrawText( NULL, textBuf, -1, &rect, DT_NOCLIP, color );
          }
          ++i;
        }
      }
    }

    //
    //  Overdraw highlighted points
    // 
    {
      device->SetMaterial (&Editor::Viewport::s_HighlightedMaterial);
      OS_HierarchyNodeDumbPtr::Iterator childItr = curve->GetChildren().Begin();
      OS_HierarchyNodeDumbPtr::Iterator childEnd = curve->GetChildren().End();
      for ( u32 i = 0; childItr != childEnd; ++childItr )
      {
        Editor::Point* point = Reflect::ObjectCast< Editor::Point >( *childItr );
        if ( point )
        {
          if ( point->IsHighlighted() )
          {
            device->DrawPrimitive( D3DPT_POINTLIST, (u32)vertices->GetBaseIndex() + i, 1 );
          }
          ++i;
        }
      }
    }

    //
    // Restore render state
    //

    device->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
  }
}

bool Curve::Pick( PickVisitor* pick )
{  
  Content::Curve* curve = GetPackage<Content::Curve>();

  bool pickHit = false;

  Math::Vector3 normal ( UpVector );
  pick->State().m_Matrix.TransformNormal( normal );

  pick->SetCurrentObject (this, pick->State().m_Matrix);

  if ( !curve->m_Points.empty() )
  {
    //
    // Pick Curve Points
    //

    for ( size_t i=0; i < curve->m_Points.size() && !pickHit; ++i )
    {
      pickHit |= pick->PickPoint (curve->m_Points[ i ]); 
    }


    //
    // Pick Curve Lines
    //

    for ( size_t i=0; i < curve->m_Points.size() - 1 && !pickHit; ++i )
    {
      pickHit |= pick->PickSegment (curve->m_Points[ i ], curve->m_Points[ i + 1 ]); 
    }


    //
    // Pick locators
    //

    if ( !pickHit )
    {
      Math::Vector3 startLocator (curve->m_Points[ 0 ]);
      pick->State().m_Matrix.TransformVertex (startLocator);
      pick->SetCurrentObject (this, Math::Matrix4 (startLocator));
      pickHit = m_Locator->Pick (pick);
    }

    if ( !pickHit )
    {
      Math::Vector3 endLocator (curve->m_Points[ curve->m_Points.size() - 1 ]);
      pick->State().m_Matrix.TransformVertex (endLocator);
      pick->SetCurrentObject (this, Math::Matrix4 (endLocator));
      pickHit = m_Locator->Pick (pick);
    }
  }

  return pickHit;
}

bool Curve::ValidatePanel( const tstring& name )
{
  if ( name == TXT( "Curve" ) )
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

void Curve::CreatePanel( CreatePanelArgs& args )
{
  args.m_Generator->PushPanel( TXT( "Curve" ), true);
  {
    args.m_Generator->PushContainer();
    {
      args.m_Generator->AddLabel( TXT( "Type" ) );
      args.m_Generator->AddChoice<Editor::Curve, int>( args.m_Selection, Reflect::Registry::GetInstance()->GetEnumeration( TXT( "CurveType" ) ), &Curve::GetCurveType, &Curve::SetCurveType );
    }
    args.m_Generator->Pop();

    args.m_Generator->PushContainer();
    {
      args.m_Generator->AddLabel( TXT( "Control Point Label" ) );
      args.m_Generator->AddChoice<Editor::Curve, int>( args.m_Selection, Reflect::Registry::GetInstance()->GetEnumeration( TXT( "ControlPointLabel" ) ), &Curve::GetControlPointLabel, &Curve::SetControlPointLabel );
    }
    args.m_Generator->Pop();

    args.m_Generator->PushContainer();
    {
      args.m_Generator->AddLabel( TXT( "Resolution" ) );
      Inspect::Slider* slider = args.m_Generator->AddSlider<Editor::Curve, u32>( args.m_Selection, &Curve::GetResolution, &Curve::SetResolution );
      slider->SetRangeMin( 1.0f, false );
      slider->SetRangeMax( 20.0f, false );
    }
    args.m_Generator->Pop();

    args.m_Generator->PushContainer();
    {
      args.m_Generator->AddLabel( TXT( "Closed" ) );
      args.m_Generator->AddCheckBox<Editor::Curve, bool>( args.m_Selection, &Curve::GetClosed, &Curve::SetClosed );
    }
    args.m_Generator->Pop();

    args.m_Generator->PushContainer();
    {
      args.m_Generator->AddLabel( TXT( "Reverse Control Points" ) );
      Inspect::Action* button = args.m_Generator->AddAction( Inspect::ActionSignature::Delegate( &Curve::OnReverseControlPoints ) );
      button->SetIcon( TXT( "reverse" ) );
      button->SetClientData( new SelectionDataObject( args.m_Selection ) );
    }
    args.m_Generator->Pop();

    args.m_Generator->PushContainer();
    {
      args.m_Generator->AddLabel( TXT( "Curve Length" ) );

      typedef f32 ( Curve::*Getter )() const;
      typedef void ( Curve::*Setter )( const f32& );
      Inspect::Value* textBox = args.m_Generator->AddValue< Editor::Curve, f32, Getter, Setter >( args.m_Selection, &Curve::CalculateCurveLength );
      textBox->SetReadOnly( true );
    }
    args.m_Generator->Pop();
  }
  args.m_Generator->Pop();
}

void Curve::OnReverseControlPoints( Inspect::Button* button )
{
  SelectionDataObject* selectionData = static_cast< SelectionDataObject* >( button->GetClientData() );
  if ( selectionData )
  {
    Editor::Scene* scene = NULL;
    Undo::BatchCommandPtr batch = new Undo::BatchCommand();
    OS_SelectableDumbPtr& selection = selectionData->m_Selection;
    OS_SelectableDumbPtr::Iterator selItr = selection.Begin();
    OS_SelectableDumbPtr::Iterator selEnd  = selection.End();
    for ( ; selItr != selEnd; ++selItr )
    {
      Editor::Curve* curve = Reflect::ObjectCast< Editor::Curve >( *selItr );
      if ( curve )
      {
        scene = curve->GetScene();
        batch->Push( curve->ReverseControlPoints() );
      }
    }

    if ( scene && !batch->IsEmpty() )
    {
      scene->Push( batch );
      scene->Execute( false );
    }
  }
}

bool Curve::ChildChangingParents( const ParentChangingArgs& args )
{
  // Prevent rearranging points (for now)
  return false;
}
