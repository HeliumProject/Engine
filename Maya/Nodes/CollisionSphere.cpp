#include "Precompile.h"
#include "CollisionSphere.h"

#include "MayaUtils/NodeTypes.h"

const MTypeId CollisionSphere::s_TypeID( IGL_COLL_SPHERE );
const char* CollisionSphere::s_TypeName = "igSphere";

MStatus CollisionSphere::Initialize()
{ 
  return inheritAttributesFrom( Sphere::s_TypeName );
}