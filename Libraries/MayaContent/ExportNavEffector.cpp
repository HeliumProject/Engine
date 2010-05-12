#include "stdafx.h"

#include "ExportNavEffector.h"
#include "MayaContentCmd.h"
#include "ExportPivotTransform.h"

using namespace Content;
using namespace MayaContent;

void ExportNavEffector::GatherMayaData( V_ExportBase &newExportObjects )
{
  EXPORT_SCOPE_TIMER( ("") );

  // doing a dangerous cast, because there should be no way for m_ContentObject to NOT be a Content::NavEffector
  Content::NavEffector* navEffector = Reflect::DangerousCast< Content::NavEffector >( m_ContentObject );

  MStatus status;

  MFnDagNode nodeFn( m_MayaObject );
  
  navEffector->m_DefaultName = nodeFn.name().asChar();

  // this will retrieve and setup the global matrix for this mesh (because meshs are transforms in Content but not Maya)
  MayaContentCmd::ConvertMatrix( MDagPath::getAPathTo( m_MayaObject ).inclusiveMatrix(), navEffector->m_GlobalTransform );
  navEffector->m_GlobalTransform.t.x *= Math::CentimetersToMeters;
  navEffector->m_GlobalTransform.t.y *= Math::CentimetersToMeters;
  navEffector->m_GlobalTransform.t.z *= Math::CentimetersToMeters;

  u32 typeID = nodeFn.typeId().id();

  switch( typeID )
  {
  case IGT_NAV_SPHERE_ID:
    navEffector->m_Shape = NavEffectorShapes::Sphere; 
    break;

  case IGT_NAV_CYLINDER_ID:
    navEffector->m_Shape = NavEffectorShapes::Cylinder;
    break;

  case IGT_NAV_CUBOID_ID:
    navEffector->m_Shape = NavEffectorShapes::Cube; 
    break;
  }

  if( typeID == IGT_NAV_CYLINDER_ID )
  {
    MFnDagNode parentFn(nodeFn.parent(0));

    MStatus mstatus;
    MPlug Plug = parentFn.findPlug("scaleX");

    MPlugArray test;
    if(Plug.connectedTo(test, true, true))
    {
      //2 expression and one connection to child sphere
      if(test.length() == 3)
      {
        MFnDagNode dn;
        //get the transform of the child spshere
        for (int i=0; i<3; i++)
        {
          dn.setObject(test[i].node());
          if (stricmp(dn.typeName().asChar(), "transform") == 0)
          {
            dn.setObject(dn.child(0));
            //Maya::FullPathNameWithoutNamespace( dn, primitive->m_ChildName );
            navEffector->m_ChildID = Maya::GetNodeID(dn.object());
              break;
          }
        }
      }
    }
  }

  // link it into the hierarchy
  MObject parent = nodeFn.parent(0);
  navEffector->m_ParentID = Maya::GetNodeID( parent );

  ExportPivotTransformPtr parentTransform = new ExportPivotTransform( parent, Maya::GetNodeID(parent) );
  newExportObjects.push_back( parentTransform );

  __super::GatherMayaData( newExportObjects );
}