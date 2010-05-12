#include "stdafx.h"

#include "ExportPhysicsJoint.h"
#include "MayaContentCmd.h"
#include "Math/EulerAngles.h"

using namespace MayaContent;

void ExportPhysicsJoint::GatherMayaData( V_ExportBase &newExportObjects )
{
  EXPORT_SCOPE_TIMER( ("") );

  MStatus status;
  MFnIkJoint jointFn (m_MayaObject);

  // try cast, because there should be no way for m_ContentObject to NOT be a Content::PhysicsJoint
  // and we need it to be a PhysicsJoint
  // 
  Content::PhysicsJoint* joint = Reflect::TryCast< Content::PhysicsJoint >( m_ContentObject );

  joint->m_DefaultName = jointFn.name().asChar();

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
  
  double x, y, z;
  MFnNumericData numericFn;
  MObject numericData = numericFn.create( MFnNumericData::k3Double, &status );

  MPlug bspherePlug = jointFn.findPlug("AddsToBSphere", &status);
  if (status)
  {
    bspherePlug.getValue(joint->m_ContributesTOBSphere);
  }
  // Fill in data physics jointfields from Maya attributes
  MPlug plug( m_MayaObject, jointFn.attribute( "GameRotLimitMin", &status ) );
  status = plug.getValue( numericData );
  status = numericFn.setObject( numericData );
  status = numericFn.getData( x, y, z );
  joint->m_GameRotLimitMin.x = (float)x;
  joint->m_GameRotLimitMin.y = (float)y;
  joint->m_GameRotLimitMin.z = (float)z;

  plug = jointFn.findPlug( "GameRotLimitMax", &status );
  plug.getValue( numericData );
  numericFn.setObject( numericData );
  numericFn.getData( x, y, z );
  joint->m_GameRotLimitMax.x = (float)x;
  joint->m_GameRotLimitMax.y = (float)y;
  joint->m_GameRotLimitMax.z = (float)z;

  plug = jointFn.findPlug("ResistMinLimit", &status);
  plug.getValue(numericData);
  numericFn.setObject(numericData);
  numericFn.getData( x, y, z );
  joint->m_ResistMinLimit.x = (float)x;
  joint->m_ResistMinLimit.y = (float)y;
  joint->m_ResistMinLimit.z = (float)z;

  plug = jointFn.findPlug("ResistMaxLimit", &status);
  plug.getValue(numericData);
  numericFn.setObject(numericData);
  numericFn.getData( x, y, z );
  joint->m_ResistMaxLimit.x = (float)x;
  joint->m_ResistMaxLimit.y = (float)y;
  joint->m_ResistMaxLimit.z = (float)z;

  plug = jointFn.findPlug("EnableGameRotLimitMin", &status);
  plug.getValue(numericData);
  numericFn.setObject(numericData);
  numericFn.getData( joint->m_EnableGameRotLimitMin.x, joint->m_EnableGameRotLimitMin.y, joint->m_EnableGameRotLimitMin.z );

  plug = jointFn.findPlug("EnableGameRotLimitMax", &status);
  plug.getValue(numericData);
  numericFn.setObject(numericData);
  numericFn.getData( joint->m_EnableGameRotLimitMax.x, joint->m_EnableGameRotLimitMax.y, joint->m_EnableGameRotLimitMax.z );


  plug     =  jointFn.findPlug("IsRotation",&status);
  status   =  plug.getValue( joint->m_IsRotational );

  plug     =  jointFn.findPlug("CoefRest",&status);
  status   =  plug.getValue( joint->m_CoefRest );

  plug     =  jointFn.findPlug("MinVelCoefRest",&status);
  status   =  plug.getValue( joint->m_MinVelCoefRest );

  plug     =  jointFn.findPlug("SpringConst",&status);
  status   =  plug.getValue( joint->m_SpringConst );

  plug     =  jointFn.findPlug("SpringDamp",&status);
  status   =  plug.getValue( joint->m_SpringDamp );

  //Not all physics joints have below attrs so we dont error here just find them if exist or else they go default
  plug = jointFn.findPlug("EnableForceMin",&status); 
  if ( status )
    plug.getValue( joint->m_EnableForceMin );

  plug = jointFn.findPlug("EnableForceMax",&status); 
  if ( status )
    plug.getValue( joint->m_EnableForceMax );

  plug = jointFn.findPlug("BreakForceMin",&status); 
  if ( status )
    plug.getValue( joint->m_BreakForceMin );

  plug = jointFn.findPlug("BreakForceMax",&status); 
  if ( status )
    plug.getValue( joint->m_BreakForceMax );

  plug = jointFn.findPlug( "Mass", &status ); 
  if ( status )
    plug.getValue( joint->m_Mass );

  plug = jointFn.findPlug( "VolumeScale", &status ); 
  if ( status )
    plug.getValue( joint->m_VolumeScale );

  plug = jointFn.findPlug( "IsPerminantEnable", &status );
  if ( status )
    plug.getValue( joint->m_IsPerminantEnable );

  plug = jointFn.findPlug( "DeleteOnBreak",&status );
  if ( status )
    plug.getValue( joint->m_DeleteOnBreak );

  plug = jointFn.findPlug( "IsDestructable", &status ); 
  if ( status )
    plug.getValue( joint->m_IsDestructable );

  plug = jointFn.findPlug( "InheritPhysics", &status );
  if ( status )
    plug.getValue( joint->m_InheritPhysics );

  plug = jointFn.findPlug( "EnableIK", &status ); 
  if ( status )
    plug.getValue( joint->m_EnableIK ); 

  __super::GatherMayaData( newExportObjects );
}
