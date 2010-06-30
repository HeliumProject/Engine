#include "Precompile.h"
#include "ExportPivotTransform.h"
#include "MayaContentCmd.h"

using namespace MayaContent;

void ExportPivotTransform::GatherMayaData( V_ExportBase &newExportObjects )
{
  EXPORT_SCOPE_TIMER( ("") );

  MFnTransform transformFn( m_MayaObject );

  // doing a dangerous cast, because there should be no way for m_ContentObject to NOT be a Content::Transform
  Content::PivotTransform* transform = Reflect::DangerousCast< Content::PivotTransform >( m_ContentObject );

  transform->m_DefaultName = transformFn.name().asChar();

  //
  // Get local and global matrices from teh m and wm plugs
  //

    MayaContentCmd::ConvertMatrix( transformFn.transformationMatrix(), transform->m_ObjectTransform );
    transform->m_ObjectTransform.t.x *= Math::CentimetersToMeters;
    transform->m_ObjectTransform.t.y *= Math::CentimetersToMeters;
    transform->m_ObjectTransform.t.z *= Math::CentimetersToMeters;

    MayaContentCmd::ConvertMatrix( MDagPath::getAPathTo(transformFn.object()).inclusiveMatrix(), transform->m_GlobalTransform );
    transform->m_GlobalTransform.t.x *= Math::CentimetersToMeters;
    transform->m_GlobalTransform.t.y *= Math::CentimetersToMeters;
    transform->m_GlobalTransform.t.z *= Math::CentimetersToMeters;

  //
  // Get local components from the transform nodes' MTransformationMatrix
  //

  MTransformationMatrix matrix = transformFn.transformation();

  double scale[3];
  matrix.getScale(scale, MSpace::kTransform);
  transform->m_Scale.x = (float)scale[0];
  transform->m_Scale.y = (float)scale[1];
  transform->m_Scale.z = (float)scale[2];

  double shear[3];
  matrix.getShear(shear, MSpace::kTransform);
  transform->m_Shear.xy = (float)shear[0];
  transform->m_Shear.xz = (float)shear[1];
  transform->m_Shear.yz = (float)shear[2];

  MTransformationMatrix::RotationOrder order = MTransformationMatrix::kXYZ;
  matrix.reorderRotation( order );
  MEulerRotation rotate = matrix.eulerRotation();
  transform->m_Rotate.x = (float)rotate.x;
  transform->m_Rotate.y = (float)rotate.y;
  transform->m_Rotate.z = (float)rotate.z;

  MPoint scalePivot = matrix.scalePivot( MSpace::kTransform );
  transform->m_ScalePivot.x = (float)scalePivot.x * Math::CentimetersToMeters;
  transform->m_ScalePivot.y = (float)scalePivot.y * Math::CentimetersToMeters;
  transform->m_ScalePivot.z = (float)scalePivot.z * Math::CentimetersToMeters;

  MVector scalePivotTranslate = matrix.scalePivotTranslation( MSpace::kTransform );
  transform->m_ScalePivotTranslate.x = (float)scalePivotTranslate.x * Math::CentimetersToMeters;
  transform->m_ScalePivotTranslate.y = (float)scalePivotTranslate.y * Math::CentimetersToMeters;
  transform->m_ScalePivotTranslate.z = (float)scalePivotTranslate.z * Math::CentimetersToMeters;

  MPoint rotatePivot = matrix.rotatePivot( MSpace::kTransform );
  transform->m_RotatePivot.x = (float)rotatePivot.x * Math::CentimetersToMeters;
  transform->m_RotatePivot.y = (float)rotatePivot.y * Math::CentimetersToMeters;
  transform->m_RotatePivot.z = (float)rotatePivot.z * Math::CentimetersToMeters;

  MVector rotatePivotTranslate = matrix.rotatePivotTranslation( MSpace::kTransform );
  transform->m_RotatePivotTranslate.x = (float)rotatePivotTranslate.x * Math::CentimetersToMeters;
  transform->m_RotatePivotTranslate.y = (float)rotatePivotTranslate.y * Math::CentimetersToMeters;
  transform->m_RotatePivotTranslate.z = (float)rotatePivotTranslate.z * Math::CentimetersToMeters;

  MVector translate = matrix.getTranslation( MSpace::kTransform );
  transform->m_Translate.x = (float)translate.x * Math::CentimetersToMeters;
  transform->m_Translate.y = (float)translate.y * Math::CentimetersToMeters;
  transform->m_Translate.z = (float)translate.z * Math::CentimetersToMeters;

  // link it into the hierarchy
  transform->m_ParentID = Maya::GetNodeID(transformFn.parent(0));

  __super::GatherMayaData( newExportObjects );
}
