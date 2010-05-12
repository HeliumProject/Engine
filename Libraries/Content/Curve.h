#pragma once

#include "PivotTransform.h"
#include "Point.h"

namespace Content
{
  //
  // Curve
  //

  namespace CurveTypes
  {
    enum CurveType
    {
      Linear,
      BSpline,
      CatmullRom,
    };

    static void CurveTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Linear, "Linear");
      info->AddElement(BSpline, "BSpline");
      info->AddElement(CatmullRom, "CatmullRom");
    }
  }

  typedef CurveTypes::CurveType CurveType;

  namespace ControlPointLabels
  {
    enum ControlPointLabel
    {
      None,
      CurveAndIndex,
      IndexOnly
    };

    static void ControlPointLabelEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement( None, "None" );
      info->AddElement( CurveAndIndex, "Curve and Index" );
      info->AddElement( IndexOnly, "Index only" );
    }
  }
  typedef ControlPointLabels::ControlPointLabel ControlPointLabel;

  class CONTENT_API Curve : public PivotTransform
  {
  private:
    // (LEGACY CURVE) The 3D locations of the control points (CVs)
    Math::V_Vector3 m_ControlPoints;

  public:
    // Is the curve closed?
    bool m_Closed;

    // The degree of the curve basis function
    CurveType m_Type;

    // The resolution of the points to compute
    u32 m_Resolution;

    // How to display labels for control points
    ControlPointLabel m_ControlPointLabel;

    // The 3D locations of the computed curve points (CVs)
    Math::V_Vector3 m_Points;

    // Order that control points should be imported in.  This is a bit hacky.
    // Would be better to have Content::Scene track child order on nodes.
    V_tuid m_ControlPointOrder;


    Curve ()
      : m_Closed( false )
      , m_Type( CurveTypes::Linear )
      , m_Resolution( 10 )
      , m_ControlPointLabel( ControlPointLabels::None )
    {

    }

    Curve (const UniqueID::TUID& id)
      : PivotTransform( id )
      , m_Closed( false )
      , m_Type( CurveTypes::Linear )
      , m_Resolution( 10 )
      , m_ControlPointLabel( ControlPointLabels::None )
    {

    }

    virtual void PostLoad( Reflect::V_Element& elements ) NOC_OVERRIDE;

    REFLECT_DECLARE_CLASS(Curve, PivotTransform);

    static void EnumerateClass( Reflect::Compositor<Curve>& comp );

    void ProjectPointOnCurve( const Math::Vector3& point, Math::Vector3& projectedPoint ) const;

    f32 DistanceSqrToCurve( const Math::Vector3& point ) const;

    f32 DistanceToCurve( const Math::Vector3& point ) const;
  };

  typedef Nocturnal::SmartPtr<Curve> CurvePtr;
}