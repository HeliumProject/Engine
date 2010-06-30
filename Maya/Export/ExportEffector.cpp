#include "Precompile.h"
#include "ExportEffector.h"
#include "ExportPivotTransform.h"
#include "MayaContentCmd.h"

using namespace MayaContent;

void ExportEffector::GatherMayaData( V_ExportBase &newExportObjects )
{
  EXPORT_SCOPE_TIMER( ("") );
  Content::Effector* effector = Reflect::DangerousCast< Content::Effector >( m_ContentObject );

  // retrieve the global matrix for this object
  MayaContentCmd::ConvertMatrix( MDagPath::getAPathTo( m_MayaObject ).inclusiveMatrix(), effector->m_GlobalTransform );
  effector->m_GlobalTransform.t.x *= Math::CentimetersToMeters;
  effector->m_GlobalTransform.t.y *= Math::CentimetersToMeters;
  effector->m_GlobalTransform.t.z *= Math::CentimetersToMeters;

  // get the effector's index
  MFnDagNode nodeFn( m_MayaObject );
  MPlug effectorIdPlug = nodeFn.findPlug("EffectorID");
  effectorIdPlug.getValue( effector->m_EffectorIndex );

  // link it into the hierarchy
  MObject parent = nodeFn.parent(0);
  effector->m_ParentID = Maya::GetNodeID(parent);

  // make sure the parent transform gets exported
  ExportPivotTransformPtr parentTransform = new ExportPivotTransform( parent, Maya::GetNodeID(parent) );
  newExportObjects.push_back( parentTransform );

  __super::GatherMayaData( newExportObjects );
}
