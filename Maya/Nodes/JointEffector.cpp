#include "Precompile.h"
#include "JointEffector.h"

#include "Maya/NodeTypes.h"

const MTypeId JointEffector::s_TypeID(IGL_JOINT_EFFECTOR);
const char* JointEffector::s_TypeName = "igJointEffector";
MObject JointEffector::effectorId;

MStatus JointEffector::Initialize()
{
  MStatus stat;
  MFnNumericAttribute nAttr; 

	effectorId = nAttr.create("EffectorID", "eid", MFnNumericData::kInt);
  nAttr.setDefault(-1.0);

  stat = addAttribute(effectorId);
  if (!stat) 
  {
    stat.perror("addAttribute");
    return stat;
  }

  return inheritAttributesFrom( Sphere::s_TypeName );
}
