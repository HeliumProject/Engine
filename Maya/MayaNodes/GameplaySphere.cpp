#include "stdafx.h"
#include "GameplaySphere.h"

#include "MayaUtils/NodeTypes.h"

const MTypeId GameplaySphere::s_TypeID( IGL_GP_SPHERE );
const char* GameplaySphere::s_TypeName = "gpSphere";

MStatus GameplaySphere::Initialize()
{ 
  return inheritAttributesFrom( Sphere::s_TypeName );
}