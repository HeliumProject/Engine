#include "Precompile.h"
#include "ExportCurve.h"
#include "ExportPoint.h"
#include "MayaContentCmd.h"

using namespace Helium;
using namespace Helium::MayaContent;

void ExportCurve::GatherMayaData( V_ExportBase& newExportObjects )
{
  MStatus status;
  MFnNurbsCurve curveFn( m_MayaObject, &status );
  if ( status != MS::kSuccess )
  {
    Log::Error( TXT("Unable to convert maya object to an ExportCurve.\n") );
    return;
  }

  Content::Curve* curve = Reflect::DangerousCast< Content::Curve >( m_ContentObject );

  curve->m_DefaultName = curveFn.name().asTChar();

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

  for ( int i = 0; i < numCVs; i++ )
  {
    MPoint point;
    curveFn.getCV( i, point );

    ExportPointPtr controlPoint = new ExportPoint( point );
    controlPoint->GetContentPoint()->m_ParentID = curve->m_ID;
    newExportObjects.push_back( controlPoint );
  }

  // link it into the hierarchy
  curve->m_ParentID = Maya::GetNodeID( curveFn.parent( 0 ) );
}