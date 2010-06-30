#include "Precompile.h"
#include "NavEffectorSphere.h"

#include "MayaUtils/NodeTypes.h"

const MTypeId NavEffectorSphere::s_TypeID(IGT_NAV_SPHERE_ID);
const char* NavEffectorSphere::s_TypeName = "igNavEffectorSphere";

MStatus NavEffectorSphere::Initialize()
{
  return inheritAttributesFrom( Sphere::s_TypeName );
}
