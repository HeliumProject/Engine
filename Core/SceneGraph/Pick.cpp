/*#include "Precompile.h"*/
#include "Core/SceneGraph/Pick.h"
#include "Core/SceneGraph/Camera.h"

#include "Core/SceneGraph/Viewport.h"

#include <map>

using namespace Helium;
using namespace Helium::SceneGraph;


//
//  PickVisitor HELIUM_ABSTRACT class
//

PickVisitor::PickVisitor(const SceneGraph::Camera* camera)
: m_Flags (0x0)
, m_Camera (camera)
, m_CurrentObject (NULL)

{

}

PickHit* PickVisitor::AddHit()
{
  //  If you are hitting this assert, then you did not correctly set the current object
  //  you are trying to pick against or its transform.  Please go back and set this before
  //  you call the intersection functions.
  HELIUM_ASSERT (m_CurrentObject);

  // allocate the hit
  PickHit* hit = new PickHit (m_CurrentObject);

  // add it to the list
  m_PickHits.push_back (hit);

  // return it so people can add data
  return hit;
}


//
//  LinePickVisitor
// 

LinePickVisitor::LinePickVisitor(const SceneGraph::Camera* camera, int x, int y)
: PickVisitor (camera)
{
  camera->ViewportToLine((f32)x, (f32)y, m_WorldSpaceLine);
}

LinePickVisitor::LinePickVisitor(const SceneGraph::Camera* camera, const Line& line)
: PickVisitor (camera)
, m_WorldSpaceLine (line)
{

}

void LinePickVisitor::Transform()
{
  m_PickSpaceLine = m_WorldSpaceLine;
  m_PickSpaceLine.Transform(m_CurrentInverseWorldTransform);
}

bool LinePickVisitor::PickPoint(const Vector3& point, const float err)
{
  Vector3 offset;

  if (m_PickSpaceLine.IntersectsPoint(point, err, NULL, &offset))
  {
    return AddHitPoint(point, offset);
  }

  return false;
}

bool LinePickVisitor::PickSegment(const Vector3& p1, const Vector3& p2, const float err)
{
  f32 mu;
  Vector3 offset;

  if (m_PickSpaceLine.IntersectsSegment (p1, p2, err, &mu, &offset))
  {
    return AddHitSegment(p1, p2, mu, offset);
  }

  return false;
}

bool LinePickVisitor::PickTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const float err)
{
  f32 u = 0.f;
  f32 v = 0.f;
  bool interior = true;
  bool success = m_PickSpaceLine.IntersectsTriangle (v0, v1, v2, &u, &v);

  Vector3 vertex;
  Vector3 intersection;
  f32 distance = (f32)BigFloat;

  if (!success)
  {
    interior = false;

    float mu;
    Vector3 offset;

    if (m_PickSpaceLine.IntersectsSegment(v0, v1, err, &mu, &offset))
    {
      float len = offset.Length();
      if (len < distance)
      {
        vertex = mu < 0.5f ? v0 : v1;
        distance = len;
        success = true;
      }
    }

    if (m_PickSpaceLine.IntersectsSegment(v1, v2, err, &mu, &offset))
    {
      float len = offset.Length();
      if (len < distance)
      {
        vertex = mu < 0.5f ? v1 : v2;
        distance = len;
        success = true;
      }
    }

    if (m_PickSpaceLine.IntersectsSegment(v2, v0, err, &mu, &offset))
    {
      float len = offset.Length();
      if (len < distance)
      {
        vertex = mu < 0.5f ? v2 : v0;
        distance = len;
        success = true;
      }
    }
  }

  if (success)
  {
    success =  AddHitTriangle(v0, v1, v2, u, v, interior, vertex, intersection, distance);
  }

  return false;
}

bool LinePickVisitor::PickSphere(const Vector3& center, const float radius)
{
  return PickPoint (center, radius);
}

bool LinePickVisitor::PickBox(const AlignedBox& box)
{
  Vector3 intersection;

  if (m_PickSpaceLine.IntersectsBox(box, &intersection))
  {
    return AddHitBox(box, intersection);
  }

  return false;
} 

bool LinePickVisitor::IntersectsBox(const AlignedBox& box) const
{
  return m_PickSpaceLine.IntersectsBox(box);
} 

bool LinePickVisitor::AddHitPoint(const Vector3& p, Vector3& offset)
{
  // allocate a hit
  PickHit* hit = AddHit ();

  // our intersection point is the point itself
  Vector3 intersection (p);

  // transform values into world space
  m_CurrentWorldTransform.TransformNormal(offset);
  m_CurrentWorldTransform.TransformVertex(intersection);

  // set vertex in world space
  if (!HasFlags(PickFlags::IgnoreVertex))
  {
    hit->SetVertex(intersection, offset.Length());
  }

  // set the intersection in world space
  if (!HasFlags(PickFlags::IgnoreIntersection))
  {
    hit->SetIntersection(intersection, offset.Length());
  }

  return true;
}

bool LinePickVisitor::AddHitSegment(const Vector3& p1,const Vector3& p2, f32 mu, Vector3& offset)
{
  // allocate a hit
  PickHit* hit = AddHit ();

  // the closest segment vertex
  Vector3 vertex (mu < 0.5f ? p1 : p2);

  // the actual intersection point
  Vector3 intersection (p1 + (p2 - p1) * mu);

  // transform values into world space
  m_CurrentWorldTransform.TransformNormal(offset);
  m_CurrentWorldTransform.TransformVertex(vertex);
  m_CurrentWorldTransform.TransformVertex(intersection);

  // set vertex in world space
  if (!HasFlags(PickFlags::IgnoreVertex))
  {
    hit->SetVertex(vertex, (vertex - intersection).Length());
  }

  // set intersection in world space
  if (!HasFlags(PickFlags::IgnoreIntersection))
  {
    hit->SetIntersection(intersection, offset.Length());
  }

  return true;
}

bool LinePickVisitor::AddHitTriangle(const Vector3& v0,const Vector3& v1,const Vector3& v2, f32 u, f32 v, bool interior, Vector3& vertex, Vector3& intersection, float distance)
{
  f32 dot = 0.f;
  Vector3 normal ( (v1 - v0).Cross(v2 - v1) );

  if (m_Camera->GetShadingMode() != ShadingModes::Wireframe && m_Camera->IsBackFaceCulling())
  {
    Vector3 cameraDir;
    m_Camera->GetDirection (cameraDir);
    m_CurrentInverseWorldTransform.TransformNormal (cameraDir);
    dot = normal.Dot (cameraDir);
  }

  if ( dot < 0.f  )
  {
    // allocate a hit
    PickHit* hit = AddHit();

    if (interior)
    {
      // make BaryCentric coefficients
      f32 a = 1.0f - u - v;
      f32 b = u;
      f32 c = v;

      // triangulate intersection from BaryCentric
      intersection = v0*a + v1*b + v2*c;

      // find closest vertex
      if (a>b && a>c)
      {
        vertex = v0;
      }
      else if (b>a && b>c)
      {
        vertex = v1;
      }
      else
      {
        vertex = v2;
      }
    }

    // transform values into world space
    m_CurrentWorldTransform.TransformVertex(vertex);

    // set vertex in world space (using distance from the intersection point to the vertex)
    if (!HasFlags(PickFlags::IgnoreVertex))
    {
      hit->SetVertex(vertex, interior ? (vertex - intersection).Length() : distance);
    }

    if (interior)
    {
      m_CurrentWorldTransform.TransformNormal(normal);

      // set normal in world space
      if (!HasFlags(PickFlags::IgnoreNormal))
      {
        hit->SetNormal(normal);
      }

      m_CurrentWorldTransform.TransformVertex(intersection);

      // set intersection in world space
      if (!HasFlags(PickFlags::IgnoreIntersection))
      {
        Vector3 cam_pos;
        m_Camera->GetPosition(cam_pos);
        hit->SetIntersection(intersection, 0);
      }
    }

    return true;
  }
  
  return false;
}

bool LinePickVisitor::AddHitTriangleClosestPoint(const Vector3& v0,const Vector3& v1,const Vector3& v2, const Vector3& point)
{
  f32 dot = 0.f;
  Vector3 normal ( (v2 - v1).Cross (v1 - v0) );

  if (m_Camera->GetShadingMode() != ShadingModes::Wireframe && m_Camera->IsBackFaceCulling())
  {
    Vector3 cameraDir;
    m_Camera->GetDirection (cameraDir);
    m_CurrentInverseWorldTransform.TransformNormal (cameraDir);
    dot = normal.Dot (cameraDir);
  }

  if ( dot >= 0.f )
  {
    // allocate a hit
    PickHit* hit = AddHit();

    // our intersection point
    Vector3 intersection = point;

    // transform values into world space
    m_CurrentWorldTransform.TransformVertex(intersection);

    // set intersection in world space (we use BigFloat for the distance since the distance is spacial)
    if (!HasFlags(PickFlags::IgnoreIntersection))
    {
      hit->SetIntersection( intersection );
    }

    return true;
  }

  return false;
}

bool LinePickVisitor::AddHitBox(const AlignedBox& box, Vector3& intersection)
{
  // allocate a hit
  PickHit* hit = AddHit();

  // the corner of the box we are closest to
  Vector3 corner = box.ClosestCorner(intersection);

  // transform values into world space
  m_CurrentWorldTransform.TransformVertex(corner);
  m_CurrentWorldTransform.TransformVertex(intersection);

  // set vertex in world space
  if (!HasFlags(PickFlags::IgnoreVertex))
  {
    hit->SetVertex(corner, (corner - intersection).Length());
  }

  // set intersection in world space (we use BigFloat for the distance since the distance is spacial)
  if (!HasFlags(PickFlags::IgnoreIntersection))
  {
    hit->SetIntersection(intersection);
  }

  return true;
}


//
//  LFrustumPickVisitoir
//

FrustumPickVisitor::FrustumPickVisitor(const SceneGraph::Camera* camera, const int pixelX, const int pixelY, const float pixelBoxSize)
: PickVisitor (camera)
{
  // the center of the pixel
  Vector2 pixelCenter (pixelX + 0.5f, pixelY + 0.5f);

  // the offset for the pixel integers
  f32 pixelOffset = (pixelBoxSize < 1.f ? 16.0f : pixelBoxSize)/2.f;

  // if there is fuzziness in this selection window, then we will need to change to having mid pixel values
  camera->ViewportToFrustum(pixelCenter.x - pixelOffset, pixelCenter.y - pixelOffset, pixelCenter.x + pixelOffset, pixelCenter.y + pixelOffset, m_WorldSpaceFrustum);
}

FrustumPickVisitor::FrustumPickVisitor(const SceneGraph::Camera* camera, const Frustum& worldSpaceFrustum)
: PickVisitor (camera)
, m_WorldSpaceFrustum (worldSpaceFrustum)
{

}

void FrustumPickVisitor::Transform()
{
  m_PickSpaceFrustum = m_WorldSpaceFrustum;
  m_PickSpaceFrustum.Transform(m_CurrentInverseWorldTransform);
}

bool FrustumPickVisitor::PickPoint(const Vector3& p, const float err)
{
  if (m_PickSpaceFrustum.IntersectsPoint(p))
  {
    return AddHitPoint(p);
  }

  return false;
}

bool FrustumPickVisitor::PickSegment(const Vector3& p1,const Vector3& p2, const float err)
{
  if (m_PickSpaceFrustum.IntersectsSegment (p1, p2))
  {
    return AddHitSegment(p1, p2);
  }

  return false;
}

bool FrustumPickVisitor::PickTriangle(const Vector3& v0,const Vector3& v1,const Vector3& v2, const float err)
{
  if (m_PickSpaceFrustum.IntersectsTriangle (v0, v1, v2))
  {
    return AddHitTriangle(v0, v1, v2);
  }

  return false;
}

bool FrustumPickVisitor::PickSphere(const Vector3& center, const float radius)
{
  if (m_PickSpaceFrustum.IntersectsPoint(center, radius))
  {
    return AddHitSphere(center);
  }

  return false;
}

bool FrustumPickVisitor::PickBox(const AlignedBox& box)
{
  if (m_PickSpaceFrustum.IntersectsBox(box, true))
  {
    return AddHitBox(box);
  }

  return false;
}

bool FrustumPickVisitor::IntersectsBox(const AlignedBox& box) const
{
  return m_PickSpaceFrustum.IntersectsBox(box);
}

bool FrustumPickVisitor::AddHitPoint(const Vector3& p)
{
  // allocate a hit
  PickHit* hit = AddHit();

  // our intersection point is the point itself
  Vector3 intersection (p);

  // transform values into world space
  m_CurrentWorldTransform.TransformVertex(intersection);

  // set intersection in world space (we use BigFloat for the distance since the distance is spacial)
  if (!HasFlags(PickFlags::IgnoreIntersection))
  {
    hit->SetIntersection(intersection);
  }
  
  return true;
}

bool FrustumPickVisitor::AddHitSegment(const Vector3& p1,const Vector3& p2)
{
  // allocate a hit
  PickHit* hit = AddHit();

  // our intersection point is bisection of the segment (HACK)
  Vector3 intersection ((p1 + p2) / 2.0f);

  // transform values into world space
  m_CurrentWorldTransform.TransformVertex(intersection);

  // set intersection in world space (we use BigFloat for the distance since the distance is spacial)
  if (!HasFlags(PickFlags::IgnoreIntersection))
  {
    hit->SetIntersection(intersection);
  }
  
  return true;
}

bool FrustumPickVisitor::AddHitTriangle(const Vector3& v0,const Vector3& v1,const Vector3& v2)
{
  f32 dot = 0.f;
  Vector3 normal ( (v2 - v1).Cross (v1 - v0) );

  if (m_Camera->GetShadingMode() != ShadingModes::Wireframe && m_Camera->IsBackFaceCulling())
  {
    Vector3 cameraDir;
    m_Camera->GetDirection (cameraDir);
    m_CurrentInverseWorldTransform.TransformNormal (cameraDir);
    dot = normal.Dot (cameraDir);
  }

  if ( dot >= 0.f )
  {
    // allocate a hit
    PickHit* hit = AddHit();

    // our intersection point is center of the triangle (HACK)
    Vector3 intersection ( ( v0 + v1 + v2 ) / 3.f );
    // transform values into world space
    m_CurrentWorldTransform.TransformVertex(intersection);

    // set intersection in world space (we use BigFloat for the distance since the distance is spacial)
    if (!HasFlags(PickFlags::IgnoreIntersection))
    {
      hit->SetIntersection( intersection );
    }
    
    return true;
  }

  return false;
}

bool FrustumPickVisitor::AddHitSphere(const Vector3& center)
{
  // allocate a hit
  PickHit* hit = AddHit();

  // our intersection point is center point (HACK)
  Vector3 intersection (center);

  // transform values into world space
  m_CurrentWorldTransform.TransformVertex(intersection);

  // set intersection in world space (we use BigFloat for the distance since the distance is spacial)
  if (!HasFlags(PickFlags::IgnoreIntersection))
  {
    hit->SetIntersection(intersection);
  }

  return true;
}

bool FrustumPickVisitor::AddHitBox(const AlignedBox& box)
{
  // allocate a hit
  PickHit* hit = AddHit();

  // our intersection point is the center point (HACK)
  Vector3 intersection (box.Center());

  // transform values into world space
  m_CurrentWorldTransform.TransformVertex(intersection);

  // set intersection in world space (we use BigFloat for the distance since the distance is spacial)
  if (!HasFlags(PickFlags::IgnoreIntersection))
  {
    hit->SetIntersection(intersection);
  }

  return true;
}


//
//  FrustumLinePickVisitor
//

FrustumLinePickVisitor::FrustumLinePickVisitor(const SceneGraph::Camera* camera, const int pixelX, const int pixelY, const float pixelBoxSize)
: PickVisitor(camera)
, LinePickVisitor (camera, pixelX, pixelY)
, FrustumPickVisitor (camera, pixelX, pixelY, pixelBoxSize)
{

}

FrustumLinePickVisitor::FrustumLinePickVisitor(const SceneGraph::Camera* camera, const Line& line, const Frustum& worldSpaceFrustum)
: PickVisitor(camera)
, LinePickVisitor (camera, line)
, FrustumPickVisitor (camera, worldSpaceFrustum)
{

}

void FrustumLinePickVisitor::Transform()
{
  LinePickVisitor::Transform();
  FrustumPickVisitor::Transform();
}

bool FrustumLinePickVisitor::PickPoint(const Vector3& point, const float err)
{
  if (m_PickSpaceFrustum.IntersectsPoint(point))
  {
    if (!LinePickVisitor::PickPoint(point, (f32)BigFloat))
    {
      return FrustumPickVisitor::AddHitPoint(point);
    }

    return true;
  }

  return false;
}

bool FrustumLinePickVisitor::PickSegment(const Vector3& p1,const Vector3& p2, const float err)
{
  if (m_PickSpaceFrustum.IntersectsSegment (p1, p2))
  {
    if (!LinePickVisitor::PickSegment(p1, p2, (f32)BigFloat))
    {
      return FrustumPickVisitor::AddHitSegment(p1, p2);
    }

    return true;
  }

  return false;
}

// returns dist_square to the closest pt
f32 GetClosestPointOnEdge(const Vector3& edge_start, const Vector3& edge_end, const Vector3& pt, Vector3& closest_pt)
{
  Vector3 edge_dir = edge_end - edge_start;
  Vector3 edge_start_to_pt = pt - edge_start;
  f32 edge_dir_len_sqr = edge_dir.LengthSquared();
  f32 dot = (edge_dir.Dot(edge_start_to_pt))/edge_dir_len_sqr;
  if (dot < 0.0f) dot = 0.0f;
  if (dot > 1.0f) dot = 1.0f;
  closest_pt = edge_start + edge_dir*dot;
  Vector3 temp = pt - closest_pt;
  return temp.LengthSquared();
}

// finds closest point on the specified triangle to the specified line
bool GetClosestPointOnTri(const Line& line, const Vector3& v0, const Vector3& v1, const Vector3& v2, Vector3& result)
{
   Vector3 normal ( (v1 - v0).Cross(v2 - v1) );
   if (normal.LengthSquared() > ValueNearZero)//should remove this redundant work shared here and in normalized
   {
     normal.Normalized();
     Vector3 line_dir = line.m_Point - line.m_Origin;
     if (fabs(line_dir.Dot(normal)) < ValueNearZero )
     {
       return false;
     }
     f32 plane_w = normal.Dot(v0);
     //find the pt on tri
     f32 origin_t = normal.Dot(line.m_Origin) - plane_w;
     f32 end_t = normal.Dot(line.m_Point) - plane_w;
     f32 plane_pt_t = origin_t/(origin_t-end_t);
     Vector3 pt_on_plane = line.m_Origin + line_dir*plane_pt_t;
     //now the pt is guaranteed to be not inside the lines so blindly i will find the closest pt on each edge and pick the one closest among the 3
     //better than doing a cross for each edge and directly narrow down which edge or vert it is closest to
     Vector3 closest_pt_on_edges[3];
     f32 dist_sqr_to_closest_pt_on_edges[3];
     dist_sqr_to_closest_pt_on_edges[0] = GetClosestPointOnEdge(v0, v1, pt_on_plane, closest_pt_on_edges[0]);
     dist_sqr_to_closest_pt_on_edges[1] = GetClosestPointOnEdge(v1, v2, pt_on_plane, closest_pt_on_edges[1]);
     dist_sqr_to_closest_pt_on_edges[2] = GetClosestPointOnEdge(v2, v0, pt_on_plane, closest_pt_on_edges[2]);
     result = closest_pt_on_edges[0];
     f32 closest_dist_sqr = dist_sqr_to_closest_pt_on_edges[0];
     if (closest_dist_sqr > dist_sqr_to_closest_pt_on_edges[1])
     {
       closest_dist_sqr = dist_sqr_to_closest_pt_on_edges[1];
       result = closest_pt_on_edges[1];
     }
     if (closest_dist_sqr > dist_sqr_to_closest_pt_on_edges[2])
     {
       closest_dist_sqr = dist_sqr_to_closest_pt_on_edges[2];
       result = closest_pt_on_edges[2];
     }

     return true;
   }
   else
   {
     return false;
   }
}

bool FrustumLinePickVisitor::PickTriangle(const Vector3& v0,const Vector3& v1,const Vector3& v2, const float err)
{
  if (m_PickSpaceFrustum.IntersectsTriangle (v0, v1, v2))
  {
    if (!LinePickVisitor::PickTriangle(v0, v1, v2, (f32)BigFloat))
    {
      Vector3 point;
      if (GetClosestPointOnTri(m_PickSpaceLine, v0, v1, v2, point))
      {
        return AddHitTriangleClosestPoint(v0, v1, v2, point);
      }
      else
      {
        return FrustumPickVisitor::AddHitTriangle(v0, v1, v2);
      }
    }
    return true;
  }

  return false;
}

bool FrustumLinePickVisitor::PickSphere(const Vector3& center, const float radius)
{
  if (m_PickSpaceFrustum.IntersectsPoint(center, radius))
  {
    if (!LinePickVisitor::PickSphere(center, (f32)BigFloat))
    {
      return FrustumPickVisitor::AddHitSphere(center);
    }

    return true;
  }

  return false;
}

bool FrustumLinePickVisitor::PickBox(const AlignedBox& box)
{
  if (m_PickSpaceFrustum.IntersectsBox(box, true))
  {
    if (!LinePickVisitor::PickBox(box))
    {
      return FrustumPickVisitor::AddHitBox(box);
    }

    return true;
  }

  return false;
}

bool FrustumLinePickVisitor::IntersectsBox(const AlignedBox& box) const
{
  return m_PickSpaceFrustum.IntersectsBox(box);
}


//
// PickHit
//

struct SortKey
{
  SortKey()
    : m_DistanceFromCamera (0.f)
    , m_DistanceFromPick (0.f)
  {

  }

  SortKey( PickHitPtr hit, f32 distanceFromCamera, f32 distanceFromPick )
    : m_Hit (hit)
    , m_DistanceFromCamera (distanceFromCamera)
    , m_DistanceFromPick (distanceFromPick)
  {

  }

  PickHitPtr  m_Hit;
  f32         m_DistanceFromCamera;
  f32         m_DistanceFromPick;
};

typedef std::vector<SortKey> V_SortKey;

bool CompareCameraDistance( const SortKey& lhs, const SortKey& rhs )
{
  return lhs.m_DistanceFromCamera < rhs.m_DistanceFromCamera;
}

bool ComparePickDistance( const SortKey& lhs, const SortKey& rhs )
{
  return lhs.m_DistanceFromPick < rhs.m_DistanceFromPick;
}

bool ComparePickDistanceThenCameraDistance( const SortKey& lhs, const SortKey& rhs )
{
  if ( (fabs(lhs.m_DistanceFromPick - rhs.m_DistanceFromPick) < ValueNearZero) )
  {
    return CompareCameraDistance(lhs, rhs);
  }
  else
  {
    return ComparePickDistance(lhs, rhs);
  }
}

void PickHit::Sort(SceneGraph::Camera* camera, const V_PickHitSmartPtr& hits, V_PickHitSmartPtr& sorted, PickSortType sortType)
{
  // early out if we have no hits
  if (hits.empty())
    return;

  // get our camera distance
  Vector3 cameraPosition;
  camera->GetPosition(cameraPosition);

  V_SortKey sortKeys;

  // for each hit object
  for ( V_PickHitSmartPtr::const_iterator itr = hits.begin(), end = hits.end(); itr != end; ++itr )
  {
    PickHitPtr hit = *itr;
    Vector3 intersection;
    f32 pickDistance;

    switch (sortType)
    {
    case PickSortTypes::Intersection:
    case PickSortTypes::Surface:
      {
        if (!hit->HasIntersection())
        {
          continue;
        }

        intersection = hit->GetIntersection();
        pickDistance = hit->GetIntersectionDistance();

        break;
      }

    case PickSortTypes::Vertex:
      {
        if (!hit->HasVertex())
        {
          continue;
        }

        intersection = hit->GetVertex();
        pickDistance = hit->GetVertexDistance();

        break;
      }
    }

    f32 cameraDistance = (cameraPosition - intersection).LengthSquared();

    sortKeys.push_back( SortKey (hit, cameraDistance, pickDistance) );
  }

  switch ( sortType )
  {
  case PickSortTypes::Intersection:
    {
      std::sort( sortKeys.begin(), sortKeys.end(), &CompareCameraDistance );
      break;
    }

  case PickSortTypes::Surface:
    {
      std::sort( sortKeys.begin(), sortKeys.end(), &ComparePickDistanceThenCameraDistance );
      break;
    }

  case PickSortTypes::Vertex:
    {
      std::sort( sortKeys.begin(), sortKeys.end(), &ComparePickDistance );
      break;
    }

  default:
    {
      HELIUM_BREAK();
      break;
    }
  }

  // clear
  sorted.clear();

  // reserve data for our result
  sorted.reserve(sortKeys.size());

  // dump
  V_SortKey::const_iterator itr = sortKeys.begin();
  V_SortKey::const_iterator end = sortKeys.end();
  for( ;itr != end; ++itr )
  {
    sorted.push_back(itr->m_Hit);
  }
}
