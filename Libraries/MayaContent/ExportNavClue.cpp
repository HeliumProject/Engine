#include "stdafx.h"

#include "ExportNavClue.h"
#include "MayaContentCmd.h"
#include "ExportPivotTransform.h"

using namespace Content;
using namespace MayaContent;

void ExportNavClue::GatherMayaData( V_ExportBase &newExportObjects )
{
  EXPORT_SCOPE_TIMER( ("") );

  // doing a dangerous cast, because there should be no way for m_ContentObject to NOT be a Content::NavClue
  Content::NavClue* navClue = Reflect::DangerousCast< Content::NavClue >( m_ContentObject );

  MStatus status;

  MFnDagNode nodeFn( m_MayaObject );
  
  navClue->m_DefaultName = nodeFn.name().asChar();

  // this will retrieve and setup the global matrix for this mesh (because meshs are transforms in Content but not Maya)
  MayaContentCmd::ConvertMatrix( MDagPath::getAPathTo( m_MayaObject ).inclusiveMatrix(), navClue->m_GlobalTransform );
  navClue->m_GlobalTransform.t.x *= Math::CentimetersToMeters;
  navClue->m_GlobalTransform.t.y *= Math::CentimetersToMeters;
  navClue->m_GlobalTransform.t.z *= Math::CentimetersToMeters;

  u32 typeID = nodeFn.typeId().id();

  switch( typeID )
  {
  case IGL_NAV_CLUE_CUBOID_ID:
    navClue->m_Shape = NavClueShapes::Cube; 
    break;
  }

  // link it into the hierarchy
  MObject parent = nodeFn.parent(0);
  navClue->m_ParentID = Maya::GetNodeID( parent );

  ExportPivotTransformPtr parentTransform = new ExportPivotTransform( parent, Maya::GetNodeID(parent) );
  newExportObjects.push_back( parentTransform );

  __super::GatherMayaData( newExportObjects );
}