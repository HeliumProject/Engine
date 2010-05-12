#include "stdafx.h"
#include "NavEffectorCylinder.h"

#include "MayaUtils/NodeTypes.h"

const MTypeId NavEffectorCylinder::s_TypeID(IGT_NAV_CYLINDER_ID);
const char* NavEffectorCylinder::s_TypeName = "igNavEffectorCylinder";

MStatus NavEffectorCylinder::Initialize()
{
  return inheritAttributesFrom( Cylinder::s_TypeName );
}
