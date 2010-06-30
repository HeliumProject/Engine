#include "Precompile.h"

#include "ExportCollision.h"
#include "MayaContentCmd.h"
#include "ExportPivotTransform.h"

using namespace Content;
using namespace MayaContent;

void ExportCollision::GatherMayaData( V_ExportBase &newExportObjects )
{
  MFnDagNode nodeFn (m_MayaObject);

  // doing a dangerous cast, because there should be no way for m_ContentObject to NOT be a Content::CollisionPrimitive
  Content::CollisionPrimitive* primitive = Reflect::DangerousCast< Content::CollisionPrimitive >( m_ContentObject );

  primitive->m_DefaultName = nodeFn.name().asChar();

  // this will retrieve and setup the global matrix for this mesh (because meshs are transforms in Content but not Maya)
  MayaContentCmd::ConvertMatrix( MDagPath::getAPathTo( m_MayaObject ).inclusiveMatrix(), primitive->m_GlobalTransform );
  primitive->m_GlobalTransform.t.x *= Math::CentimetersToMeters;
  primitive->m_GlobalTransform.t.y *= Math::CentimetersToMeters;
  primitive->m_GlobalTransform.t.z *= Math::CentimetersToMeters;

  u32 typeID = nodeFn.typeId().id();

  switch( typeID )
  {
  case IGL_COLL_SPHERE:
    primitive->m_Shape = CollisionShapes::Sphere; 
    break;

  case IGL_COLL_CAPSULE:
    primitive->m_Shape = CollisionShapes::Capsule;
    break;

  case IGL_COLL_CAPSULE_CHILD:
    primitive->m_Shape = CollisionShapes::CapsuleChild; 
    break;

  case IGL_COLL_CYLINDER:
    primitive->m_Shape = CollisionShapes::Cylinder; 
    break;

  case IGL_COLL_CYLINDER_CHILD:
    primitive->m_Shape = CollisionShapes::CylinderChild; 
    break;

  case IGL_COLL_CUBOID:
    primitive->m_Shape = CollisionShapes::Cube; 
    break;

  }

  MStatus status;

  if( typeID == IGL_COLL_CAPSULE || typeID == IGL_COLL_CYLINDER  ||  typeID == IGL_LOOSE_ATTACHMENT_CAPSULE)
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
            primitive->m_ChildID = Maya::GetNodeID(dn.object());
            break;
          }
        }
      }
    }
  }

  // link it into the hierarchy
  MObject parent = nodeFn.parent(0);
  primitive->m_ParentID = Maya::GetNodeID(parent);

  ExportPivotTransformPtr parentTransform = new ExportPivotTransform( parent, Maya::GetNodeID(parent) );
  newExportObjects.push_back( parentTransform );

  __super::GatherMayaData( newExportObjects );
}
