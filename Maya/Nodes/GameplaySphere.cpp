#include "Precompile.h"
#include "GameplaySphere.h"

#include "Maya/NodeTypes.h"

const MTypeId GameplaySphere::s_TypeID( IGL_GP_SPHERE );
const char* GameplaySphere::s_TypeName = "gpSphere";

MStatus GameplaySphere::Initialize()
{ 
  return inheritAttributesFrom( Sphere::s_TypeName );
}