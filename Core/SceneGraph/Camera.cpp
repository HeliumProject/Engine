/*#include "Precompile.h"*/
#include "Camera.h"

#include "Foundation/Math/AngleAxis.h"
#include "Foundation/Math/Matrix3.h"

#include "Core/SceneGraph/Orientation.h"

#include <d3d9.h>
#include <d3dx9.h>

using namespace Helium;
using namespace Helium::SceneGraph;

const float Camera::NearClipDistance = 0.05f;
const float Camera::FarClipDistance = 10000.0f;
const float Camera::FieldOfView = 72.0f * DegToRad;

REFLECT_DEFINE_ABSTRACT(Camera);

Camera::Camera()
: m_ProjectionMode( ProjectionModes::Perspective )
, m_MovementMode( MovementModes::Orbit )
, m_Offset ( 0.0 )
, m_Sensitivity ( 0.5f )
, m_WireframeOnMesh ( false )
, m_WireframeOnShaded ( false )
, m_ViewFrustumCulling ( true )
, m_BackFaceCulling ( true )
{

}

void Camera::LoadSettings( CameraSettings* prefs )
{
  SetShadingMode( (ShadingMode)prefs->m_ShadingMode ); 
  SetWireframeOnMesh( prefs->m_WireframeOnMesh ); 
  SetWireframeOnShaded( prefs->m_WireframeOnShaded ); 
  SetViewFrustumCulling( prefs->m_ViewFrustumCulling ); 
  SetBackFaceCulling( prefs->m_BackFaceCulling ); 
}

void Camera::SaveSettings( CameraSettings* prefs )
{
  prefs->m_ShadingMode = GetShadingMode(); 
  prefs->m_WireframeOnMesh = GetWireframeOnMesh(); 
  prefs->m_WireframeOnShaded = GetWireframeOnShaded(); 
  prefs->m_ViewFrustumCulling = IsViewFrustumCulling(); 
  prefs->m_BackFaceCulling = IsBackFaceCulling(); 
}

void Camera::Setup(ProjectionMode mode, const Vector3& dir, const Vector3& up)
{
  m_ProjectionMode = mode;

  if (m_ProjectionMode == ProjectionModes::Orthographic)
  {
    m_Orientation.SetBasis(UpAxis, Vector4(up));
    m_Orientation.SetBasis(SideAxis, Vector4(dir.Cross(up)));
    m_Orientation.SetBasis(OutAxis, Vector4(Vector3::Zero - dir));
    m_Orientation.Invert();
  }

  Reset();
}

void Camera::Reset()
{
  m_Pivot = Vector3 (0.0f,0.0f,0.0f);
  m_Offset = 10.0f;
  m_Sensitivity = 0.5f;

  if (m_ProjectionMode == ProjectionModes::Perspective)
  {
    AngleAxis up (-0.78539f, UpVector);
    AngleAxis side (0.39269f, SideVector);
    m_Orientation = Matrix3( up ) * Matrix3( side );
  }

  m_ViewFrustum = Frustum ();

  m_MovementMode = MovementModes::None;
  m_Prev = Point::Zero;

  m_WireframeOnMesh = true;
  m_WireframeOnShaded = false;
  m_ShadingMode = ShadingModes::Texture;

  m_ViewFrustumCulling = true;
  m_BackFaceCulling = true;

  Update();
}

void Camera::GetUpAxisTransform(Matrix4& m) const
{
  m = Matrix4::Identity;

  m.x = SideVector;
  m.y = UpVector;
  m.z = OutVector;

  m.Invert();
}

Matrix4& Camera::SetProjection(int w, int h)
{
  m_Size.x = w;
  m_Size.y = h;

  if (m_Size.x != 0 && m_Size.y != 0)
  {
    switch (m_ProjectionMode)
    {
    case ProjectionModes::Perspective:
      {
        GetPerspectiveProjection(m_Projection);
        break;
      }

    case ProjectionModes::Orthographic:
      {
        GetOrthographicProjection(m_Projection);
        break;
      }
    }

    m_InverseProjection = m_Projection;
    m_InverseProjection.t.x = 0.0f;
    m_InverseProjection.t.y = 0.0f;
    m_InverseProjection.t.z = 0.0f;
    m_InverseProjection.t.w = 1.0f;
    m_InverseProjection.Invert();

    Update();
  }

  return m_Projection;
}

void Camera::GetPerspectiveProjection(Matrix4& m) const
{
  const float aspect = (float)m_Size.x / (float)m_Size.y;

  m = Matrix4::Identity;

  GetUpAxisTransform(m);

  float fov = 0.f;

  if (aspect < 1.f)
  {
    fov = FieldOfView; // use as vertical FOV
  }
  else
  {
    fov = FieldOfView / aspect; // use as horizontal FOV
  }

  Matrix4 persp;
  D3DXMatrixPerspectiveFovRH((D3DXMATRIX*)&persp, fov, aspect, NearClipDistance, FarClipDistance);

  m *= persp;
}

void Camera::GetOrthographicProjection(Matrix4& m) const
{
  const float aspect = (float)m_Size.x / (float)m_Size.y;

  m = Matrix4::Identity;

  GetUpAxisTransform(m);

  Matrix4 ortho;
  D3DXMatrixOrthoRH((D3DXMATRIX*)&ortho, aspect * m_Offset, m_Offset, NearClipDistance, FarClipDistance);

  m *= ortho;
}

void Camera::GetDirection(Vector3& v) const
{
  v = OutVector * -1.f;

  m_InverseView.TransformNormal(v);
}

void Camera::GetPosition(Vector3& v) const
{
  v = Vector3::Zero;

  m_InverseView.TransformVertex(v);
}

void Camera::SetTransform( const Matrix4& transform )
{
  m_View = transform.Inverted();

  m_MovementMode = MovementModes::Free;

  Vector4 pivot = transform.t;
  Vector4 forward = -transform.z;

  pivot = pivot + forward*100.0f;

  // fudge the pivot, orientation and offset so when we go back to the other camera modes, things look right
  m_Pivot.Set( pivot.x, pivot.y, pivot.z );
  m_Orientation = m_View;
  m_Orientation.t = Vector4( 0, 0, 0, 1 );
  m_Offset = 100.0f;

  Update();
}

float Camera::ScalingTo(const Vector3& location) const
{
  switch (m_ProjectionMode)
  {
  case ProjectionModes::Perspective:
    {
      Vector3 pos;
      GetPosition(pos);
      return (pos - location).Length();
    }

  case ProjectionModes::Orthographic:
    {
      return m_Offset;
    }
  }

  return 1.0f;
}

void Camera::MouseDown( const MouseButtonInput& e )
{
  // we have changed movement mode, so reset our delta
  m_Prev = Point (e.GetPosition().x, e.GetPosition().y);
}

void Camera::MouseUp( const MouseButtonInput& e )
{
  // we have changed movement mode, so reset our delta
  m_Prev = Point (e.GetPosition().x, e.GetPosition().y);
}

void Camera::MouseMove( const MouseMoveInput& e )
{
  if (e.LeftIsDown() && !e.MiddleIsDown())
  {
    m_MovementMode = MovementModes::Orbit;
  }
  else if (e.MiddleIsDown() && !e.LeftIsDown())
  {
    m_MovementMode = MovementModes::Track;
  }
  else if ((e.LeftIsDown() && e.MiddleIsDown()) || e.RightIsDown())
  {
    m_MovementMode = MovementModes::Dolly;
  }
  else
  {
    return;
  }

  int deltaX = e.GetPosition().x - m_Prev.x;
  int deltaY = e.GetPosition().y - m_Prev.y;

  switch (m_MovementMode)
  {
    case MovementModes::Orbit:
    {
      switch (m_ProjectionMode)
      {
      case ProjectionModes::Perspective:
        {
          Matrix4 inverseOrientation = m_Orientation.Inverted();

          // newO = inv(oldO) * eulerY * oldO * eulerX
          // this puts eulerY in the world frame, and keeps eulerX in the camera's frame
          m_Orientation *= inverseOrientation * Matrix4 (AngleAxis (deltaX * m_Sensitivity * 0.01f, UpVector)) * m_Orientation * Matrix4(AngleAxis (deltaY * m_Sensitivity * 0.01f, SideVector));
          m_Orientation.Orthogonalize();
          m_Orientation.Normalize();

          break;
        }
      }
      break;
    }

    case MovementModes::Track:
    {
      Vector3 p1;
      ViewportToPlaneVertex( (float32_t)m_Prev.x, (float32_t)m_Prev.y, IntersectionPlanes::Viewport, p1);
      Vector3 p2;
      ViewportToPlaneVertex( (float32_t)e.GetPosition().x, (float32_t)e.GetPosition().y, IntersectionPlanes::Viewport, p2);

      // Track vector is the translation of the m_Pivot from the starting planar intersection to the current planar intersection
      m_Pivot += p1 - p2;

      break;
    }

    case MovementModes::Dolly:
    {
      // Dolly distance is the mouse distance traveled
      float dolly = (float32_t)(-deltaX) + (float32_t)(-deltaY);

      // Factor dolly distance by the distance to our pivot point
      dolly *= m_Offset / 200.0f;

      // Update our distance to the pivot point
      if (m_Offset + dolly > FarClipDistance)
      {
        m_Offset = FarClipDistance;
      }
      else if (m_Offset + dolly < NearClipDistance)
      {
        m_Offset = NearClipDistance;
      }
      else
      {
        m_Offset += dolly;
      }

      break;
    }
  }

  Update( true );

  m_Prev = Point(e.GetPosition().x, e.GetPosition().y);
}

void Camera::MouseScroll( const MouseScrollInput& e )
{
#ifdef MOUSE_ZOOM

  // Dolly distance is the mouse wheel delta
  float dolly = ((float)e.GetWheelRotation() / (float)e.GetWheelDelta());
  
  // Factor dolly distance by the distance to our pivot point
  dolly *= m_Offset / 10.f;

  // Update our distance to the pivot point
  if (m_Offset + dolly > FarClipDistance)
  {
    m_Offset = FarClipDistance;
  }
  else if (m_Offset + dolly < NearClipDistance)
  {
    m_Offset = NearClipDistance;
  }
  else
  {
    m_Offset += dolly;
  }

#else

  // Dolly distance is the mouse wheel delta
  float dolly = ((float)e.GetWheelRotation() / (float)e.GetWheelDelta());
  
  // Factor dolly distance by the distance to our pivot point
  dolly *= m_Offset / 10.f;

  // Move pivot through space
  Vector3 dir;
  GetDirection(dir);
  m_Pivot += dir.Normalize() * dolly;

#endif
  
  Update(true);
}

void Camera::Update( bool updateRemote )
{
  //
  // Compute Viewport Transform
  //

  if ( m_MovementMode != MovementModes::Free )
  {
    switch (m_ProjectionMode)
    {
    case ProjectionModes::Perspective:
      {
        m_View = Matrix4 (m_Pivot * -1) * m_Orientation * Matrix4 (OutVector * -m_Offset);
        break;
      }

    case ProjectionModes::Orthographic:
      {
        m_View = Matrix4 (m_Pivot * -1) * m_Orientation * Matrix4 (OutVector * (-FarClipDistance/2.0f));
        break;
      }
    }
  }

  m_InverseView = m_View;
  m_InverseView.Invert();

  m_ViewFrustum = Frustum (m_View * m_Projection);

  if ( updateRemote )
  {
    m_Moved.Raise( CameraMovedArgs( m_InverseView ) );
  }
}

void Camera::WorldToScreen(const Vector3& p, float& x, float& y)
{
  Vector4 v ( p.x, p.y, p.z, 1.f );
                                              
  // global to camera
  m_View.Transform( v );

  // camera to projection
  m_Projection.Transform( v );

  // apply projection from w component
  ViewportToScreen( Vector3 ( v.x / v.w, v.y / v.w, v.z / v.w ), x, y );
}

void Camera::ViewportToScreen(const Vector3& v, float& x, float& y)
{
  x = ( (v.x + 1) * m_Size.x ) / 2.0f;
  y = ( (-v.y + 1) * m_Size.y ) / 2.0f;
}

void Camera::ScreenToViewport(float x, float y, Vector3& v) const
{
  v.x = (((2.0f * x) / m_Size.x) - 1);
  v.y = -(((2.0f * y) / m_Size.y) - 1);
}

void Camera::ViewportToWorldVertex(float x, float y, Vector3& v) const
{
  switch (m_ProjectionMode)
  {
  case ProjectionModes::Perspective:
    {
      GetPosition( v );

      break;
    }

  case ProjectionModes::Orthographic:
    {
      ScreenToViewport( x, y, v );
 
      // treat this as a x/y coordinate only (at the camera location)
      v.z = 0.0f;

      // unproject our screen space coordinate
      m_InverseProjection.TransformVertex(v);

      // orient the view vector
      m_InverseView.TransformVertex(v);

      break;
    }
  }
}

void Camera::ViewportToWorldNormal(float x, float y, Vector3& n) const
{
  switch (m_ProjectionMode)
  {
  case ProjectionModes::Perspective:
    {
      ScreenToViewport( x, y, n );

      // 1.0f is the back of viewport space, will be projected to the rear clipping plane
      n.z = 1.0f;

      // unproject our screen space coordinate
      m_InverseProjection.TransformVertex(n);

      // orient the m_View normal
      m_InverseView.TransformNormal(n);
      
      break;
    }

  case ProjectionModes::Orthographic:
    {
      GetDirection(n);

      break;
    }
  }
}

void Camera::ViewportToPlaneVertex(float x, float y, IntersectionPlane p, Vector3& v) const
{
  v = Vector3::Zero;

  // unit length dir
  Vector3 dir;
  GetDirection(dir);

  // persp/ortho pos
  Vector3 pos;
  ViewportToWorldVertex(x, y, pos);

  // re-use v to be our camera normal
  ViewportToWorldNormal(x, y, v);

  // extend to clip distance
  v *= FarClipDistance;    

  // Pick ray from our starting location
  Line line = Line (pos, pos + v);

  // plane to pick against
  Plane plane = Plane::Null;

  // compute plane
  switch (p)
  {
    case IntersectionPlanes::Viewport:
    {
      switch (m_ProjectionMode)
      {
      case ProjectionModes::Perspective:
        {
          // Planar intersections of the ray with the direction plane (camera plane)
          plane = Plane (pos + (dir * m_Offset), dir);

          break;
        }

      case ProjectionModes::Orthographic:
        {
          // Planar intersections of the ray with the direction plane (camera plane)
          plane = Plane (Vector3::Zero, dir);

          break;
        }
      }

      break;
    }

    case IntersectionPlanes::Ground:
    {
      plane = Plane (Vector3::Zero, UpVector);
      break;
    }
  }

  plane.GetNormal(v);

  // shouldn't happen
  if (v != Vector3::Zero)
  {
    // Planar intersections of the ray with the direction plane (camera plane)
    line.IntersectsPlane(plane, &v);
  }
}

void Camera::ViewportToLine(float x, float y, Line& l) const
{
  // get origin
  Vector3 pos;
  ViewportToWorldVertex(x, y, pos);

  // get direction
  Vector3 dir;
  ViewportToWorldNormal(x, y, dir);

  // extend to clip distance
  dir *= FarClipDistance;    

  // construct pick ray
  l = Line (pos, pos + dir);
}

bool Camera::ViewportToFrustum(float startx, float starty, float endx, float endy, Frustum& f) const
{
  //
  // Determine min/max in control space
  //
  Vector2 min;
  Vector2 max;

  if (startx < endx)
    min.x = startx;
  else
    min.x = endx;

  if (starty < endy)
    min.y = starty;
  else
    min.y = endy;

  if (startx > endx)
    max.x = startx;
  else
    max.x = endx;

  if (starty > endy)
    max.y = starty;
  else
    max.y = endy;

  // degenerate case, fall back to line pick
  if ( (fabs(min.x - max.x) < ValueNearZero) || (fabs(min.y - max.y) < ValueNearZero))
    return false;

  switch (m_ProjectionMode)
  {
  case ProjectionModes::Perspective:
    {
      Vector3 tln;
      ViewportToWorldNormal(min.x, min.y, tln);
      tln.Normalize();

      Vector3 trn;
      ViewportToWorldNormal(max.x, min.y, trn);
      trn.Normalize();

      Vector3 brn;
      ViewportToWorldNormal(max.x, max.y, brn);
      brn.Normalize();

      Vector3 bln;
      ViewportToWorldNormal(min.x, max.y, bln);
      bln.Normalize();

      Vector3 tlv;
      ViewportToWorldVertex(min.x, min.y, tlv);

      Vector3 brv;
      ViewportToWorldVertex(max.x, max.y, brv);

      // top left point
      f.left = Plane (tlv, bln.Cross(tln));
      f.top = Plane (tlv, tln.Cross(trn));

      // bottom right point
      f.right = Plane (brv, trn.Cross(brn));
      f.bottom = Plane (brv, brn.Cross(bln));

      break;
    }

  case ProjectionModes::Orthographic:
    {
      Vector3 tln;
      ViewportToWorldNormal(min.x, min.y, tln);
      Vector3 trn;
      ViewportToWorldNormal(max.x, min.y, trn);
      Vector3 brn;
      ViewportToWorldNormal(max.x, max.y, brn);
      Vector3 bln;
      ViewportToWorldNormal(min.x, max.y, bln);

      Vector3 tlv;
      ViewportToWorldVertex(min.x, min.y, tlv);
      Vector3 trv;
      ViewportToWorldVertex(max.x, min.y, trv);
      Vector3 brv;
      ViewportToWorldVertex(max.x, max.y, brv);
      Vector3 blv;
      ViewportToWorldVertex(min.x, max.y, blv);

      // top left point
      f.left = Plane (tlv, (blv - tlv).Cross(tln));
      f.top = Plane (tlv, (tlv - trv).Cross(trn));

      // bottom right point
      f.right = Plane (brv, (trv - brv).Cross(brn));
      f.bottom = Plane (brv, (brv - blv).Cross(bln));

      break;
    }
  }

  f.front = m_ViewFrustum.front;
  f.back = m_ViewFrustum.back;

  return true;
}

AxesFlags Camera::ParallelAxis(const Matrix4& m, float criticalDotProduct) const
{
  float dot;

  Vector3 c;
  GetPosition(c);
  c -= Vector3 (m.t.x, m.t.y, m.t.z);
  c.Normalize();

  Vector3 v;

  v = Vector3 (m.x.x, m.x.y, m.x.z);
  v.Normalize();

  dot = c.Dot(v);
  if (dot > criticalDotProduct || dot < -criticalDotProduct)
    return MultipleAxes::X;

  v = Vector3 (m.y.x, m.y.y, m.y.z);
  v.Normalize();

  dot = c.Dot(v);
  if (dot > criticalDotProduct || dot < -criticalDotProduct)
    return MultipleAxes::Y;

  v = Vector3 (m.z.x, m.z.y, m.z.z);
  v.Normalize();

  dot = c.Dot(v);
  if (dot > criticalDotProduct || dot < -criticalDotProduct)
    return MultipleAxes::Z;

  return MultipleAxes::None;
}

void Camera::Frame(const AlignedBox& box)
{
  SetPivot(box.Center());
  SetOffset((box.maximum - box.minimum).Length());
  Update(true);
}