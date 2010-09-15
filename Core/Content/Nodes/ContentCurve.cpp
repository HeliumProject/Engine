#include "ContentCurve.h"
#include "Foundation/Math/Line.h"

using namespace Helium;
using namespace Helium::Content;

REFLECT_DEFINE_CLASS(Curve);

void Curve::EnumerateClass( Reflect::Compositor<Curve>& comp )
{
  Reflect::Field* fieldClosed = comp.AddField( &Curve::m_Closed, "m_Closed" );
  Reflect::EnumerationField* curveType = comp.AddEnumerationField( &Curve::m_Type, "m_Type" );
  Reflect::EnumerationField* controlPointLabel = comp.AddEnumerationField( &Curve::m_ControlPointLabel, "m_ControlPointLabel" );
  Reflect::Field* fieldResolution = comp.AddField( &Curve::m_Resolution, "m_Resolution" );
}

void Curve::ProjectPointOnCurve( const Math::Vector3& point, Math::Vector3& projectedPoint ) const
{
  Math::Vector3 closestPoint;

  u32 size = (u32)m_Points.size();
  HELIUM_ASSERT( size >= 2 );

  Math::Line segment( m_Points[0], m_Points[1] );
  segment.Transform( m_GlobalTransform );
  segment.ProjectPointOnSegment( point, closestPoint );
  f32 closestDistSqr = ( point - closestPoint ).LengthSquared();

  for( u32 i = 1; i < size-1; ++i )
  {
    segment.m_Origin = m_Points[i];
    segment.m_Point  = m_Points[i+1];
    segment.Transform( m_GlobalTransform );
    segment.ProjectPointOnSegment( point, projectedPoint );

    f32 distSqr = (point - projectedPoint ).LengthSquared();

    if(  distSqr < closestDistSqr )
    {
      closestDistSqr = distSqr;
      closestPoint = projectedPoint;
    }
  }
  if( m_Closed )
  {
    segment.m_Origin = m_Points[size-1];
    segment.m_Point  = m_Points[0];
    segment.Transform( m_GlobalTransform );
    segment.ProjectPointOnSegment( point, projectedPoint );

    f32 distSqr = (point - projectedPoint ).LengthSquared();

    if(  distSqr < closestDistSqr )
    {
      closestDistSqr = distSqr;
      closestPoint = projectedPoint;
    }
  }
  projectedPoint = closestPoint;
}

 f32 Curve::DistanceSqrToCurve( const Math::Vector3& point ) const
 {
   Math::Vector3 projectedPoint;
   ProjectPointOnCurve( point, projectedPoint );

   return (point - projectedPoint).LengthSquared();
 }

 f32 Curve::DistanceToCurve( const Math::Vector3& point ) const
 {
   return sqrt( DistanceSqrToCurve(point) );
 }