#include "Precompile.h"

#include "ExportJoint.h"
#include "MayaContentCmd.h"
#include "Foundation/Math/EulerAngles.h"

using namespace MayaContent;

void ExportJoint::GatherMayaData( V_ExportBase &newExportObjects )
{
  EXPORT_SCOPE_TIMER( ("") );
  MFnIkJoint jointFn (m_MayaObject);
  MStatus status;

  // doing a dangerous cast, because there should be no way for m_ContentObject to NOT be a Content::JointTransform
  Content::JointTransform* joint = Reflect::DangerousCast< Content::JointTransform >( m_ContentObject );

  joint->m_DefaultName = jointFn.name().asTChar();

  //
  // Grab the special "segmentScaleCompensate" attribute
  //  this attribute causes the inverse of the parent's scaling to be applied
  //  so that each joint/joint segment can be scaled without effecting child segments
  //

  MPlug segmentPlug = jointFn.findPlug("ssc");
  segmentPlug.getValue(joint->m_SegmentScaleCompensate);

  MPlug bspherePlug = jointFn.findPlug("AddsToBSphere", &status);
  if (status)
  {
//    bspherePlug.getValue(joint->m_ContributesTOBSphere);
  }

  //
  // Get local and global matrices from teh m and wm plugs
  //

    MayaContentCmd::ConvertMatrix( jointFn.transformationMatrix(), joint->m_ObjectTransform );
    joint->m_ObjectTransform.t.x *= Math::CentimetersToMeters;
    joint->m_ObjectTransform.t.y *= Math::CentimetersToMeters;
    joint->m_ObjectTransform.t.z *= Math::CentimetersToMeters;

    MayaContentCmd::ConvertMatrix( MDagPath::getAPathTo(jointFn.object()).inclusiveMatrix(), joint->m_GlobalTransform );
    joint->m_GlobalTransform.t.x *= Math::CentimetersToMeters;
    joint->m_GlobalTransform.t.y *= Math::CentimetersToMeters;
    joint->m_GlobalTransform.t.z *= Math::CentimetersToMeters;


  //
  // Since joints can be (and probably will be) constrained against another control
  //  skeleton, we just decompose the overall matrix attribute to obtain the components
  //

  Math::Scale scale;
  Math::EulerAngles rotate;
  Math::Vector3 translate;
  joint->m_ObjectTransform.Decompose( scale, rotate, translate );

  joint->m_Scale.x = scale.x;
  joint->m_Scale.y = scale.y;
  joint->m_Scale.z = scale.z;

  joint->m_Rotate.x = rotate.I();
  joint->m_Rotate.y = rotate.J();
  joint->m_Rotate.z = rotate.H();

  joint->m_Translate.x = translate.x;
  joint->m_Translate.y = translate.y;
  joint->m_Translate.z = translate.z;

  // link it into the hierarchy
  joint->m_ParentID = Maya::GetNodeID(jointFn.parent(0));

  __super::GatherMayaData( newExportObjects );
}
