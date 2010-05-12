#include "stdafx.h"
#include "ExportCurve.h"
#include "ExportPoint.h"
#include "MayaContentCmd.h"

using namespace MayaContent;

void ExportCurve::GatherMayaData( V_ExportBase& newExportObjects )
{
  MStatus status;
  MFnNurbsCurve curveFn( m_MayaObject, &status );
  if ( status != MS::kSuccess )
  {
    Console::Error( "Unable to convert maya object to an ExportCurve.\n" );
    return;
  }

  Content::Curve* curve = Reflect::DangerousCast< Content::Curve >( m_ContentObject );

  curve->m_DefaultName = curveFn.name().asChar();

  switch ( curveFn.form() )
  {
  case MFnNurbsCurve::kOpen:
    curve->m_Closed = false;
    break;
  case MFnNurbsCurve::kClosed:
    curve->m_Closed = true;
    break;
  }

  curve->m_Type = Content::CurveTypes::BSpline;

  const int numCVs = curveFn.numCVs();

  curve->m_ControlPointOrder.clear();
  curve->m_ControlPointOrder.reserve( numCVs );

  for ( int i = 0; i < numCVs; i++ )
  {
    MPoint point;
    curveFn.getCV( i, point );

    ExportPointPtr controlPoint = new ExportPoint( point );
    controlPoint->GetContentPoint()->m_ParentID = curve->m_ID;
    newExportObjects.push_back( controlPoint );
    curve->m_ControlPointOrder.push_back( controlPoint->GetContentPoint()->m_ID );
  }

  // link it into the hierarchy
  curve->m_ParentID = Maya::GetNodeID( curveFn.parent( 0 ) );
}