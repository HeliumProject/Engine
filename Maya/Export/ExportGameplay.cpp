#include "Precompile.h"

#include "ExportGameplay.h"
#include "MayaContentCmd.h"
#include "ExportPivotTransform.h"

using namespace Content;
using namespace MayaContent;

void ExportGameplay::GatherMayaData( V_ExportBase &newExportObjects )
{
  MFnDagNode nodeFn (m_MayaObject);

  // doing a dangerous cast, because there should be no way for m_ContentObject to NOT be a Content::Volume
  Content::Volume* primitive = Reflect::DangerousCast< Content::Volume >( m_ContentObject );

  primitive->m_DefaultName = nodeFn.name().asChar();

  // this will retrieve and setup the global matrix for this mesh (because meshs are transforms in Content but not Maya)
  MayaContentCmd::ConvertMatrix( MDagPath::getAPathTo( m_MayaObject ).inclusiveMatrix(), primitive->m_GlobalTransform );
  primitive->m_GlobalTransform.t.x *= Math::CentimetersToMeters;
  primitive->m_GlobalTransform.t.y *= Math::CentimetersToMeters;
  primitive->m_GlobalTransform.t.z *= Math::CentimetersToMeters;

  u32 typeID = nodeFn.typeId().id();

  switch( typeID )
  {
  case IGL_GP_SPHERE:
    primitive->m_Shape = VolumeShapes::Sphere; 
    break;

  case IGL_GP_CAPSULE:                
    primitive->m_Shape = VolumeShapes::Capsule; 
    break;

  case IGL_GP_CYLINDER:               
    primitive->m_Shape = VolumeShapes::Cylinder; 
    break;

  case IGL_GP_CUBOID:
    primitive->m_Shape = VolumeShapes::Cube; 
    break;
  }

  // link it into the hierarchy
  MObject parent = nodeFn.parent(0);
  primitive->m_ParentID = Maya::GetNodeID(parent);

  ExportPivotTransformPtr parentTransform = new ExportPivotTransform( parent, Maya::GetNodeID(parent) );
  newExportObjects.push_back( parentTransform );

  __super::GatherMayaData( newExportObjects );
}
