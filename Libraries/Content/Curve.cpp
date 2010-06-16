#include "Curve.h"
#include "Scene.h"
#include "Foundation/Math/Line.h"

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_CLASS(Curve);

void Curve::EnumerateClass( Reflect::Compositor<Curve>& comp )
{
  Reflect::Field* fieldClosed = comp.AddField( &Curve::m_Closed, "m_Closed" );
  Reflect::EnumerationField* curveType = comp.AddEnumerationField( &Curve::m_Type, "m_Type" );
  Reflect::EnumerationField* controlPointLabel = comp.AddEnumerationField( &Curve::m_ControlPointLabel, "m_ControlPointLabel" );
  Reflect::Field* fieldResolution = comp.AddField( &Curve::m_Resolution, "m_Resolution" );
#pragma TODO( "LEGACY CURVE" )
  Reflect::Field* fieldControlPoints = comp.AddField( &Curve::m_ControlPoints, "m_ControlPoints" );
  Reflect::Field* fieldPoints = comp.AddField( &Curve::m_Points, "m_Points" );

#pragma TODO( "This is for copy/paste from Maya and it would be better to remove this member and have Content::Scene track child order." )
  Reflect::Field* fieldControlPointOrder = comp.AddField( &Curve::m_ControlPointOrder, "m_ControlPointOrder" );
}

void Curve::PostLoad( Reflect::V_Element& elements )
{
#pragma TODO( "LEGACY CURVE" )
  if ( !m_ControlPoints.empty() )
  {
    elements.reserve( elements.size() + m_ControlPoints.size() );
    Math::V_Vector3::const_iterator itr = m_ControlPoints.begin();
    Math::V_Vector3::const_iterator end = m_ControlPoints.end();
    for ( ; itr != end; ++itr )
    {
      PointPtr point = new Point();
      point->m_Position = *itr;
      point->m_ParentID = m_ID;
      elements.push_back( point );
    }
    m_ControlPoints.clear();
  }
}

void Curve::ProjectPointOnCurve( const Math::Vector3& point, Math::Vector3& projectedPoint ) const
{
  Math::Vector3 closestPoint;

  u32 size = (u32)m_Points.size();
  NOC_ASSERT( size >= 2 );

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