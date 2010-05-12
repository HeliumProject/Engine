#include "stdafx.h"
#include "CollisionCylinder.h"

#include "MayaUtils/NodeTypes.h"

const char* CollisionCylinder::s_TypeName = "igCylinder";
const MTypeId CollisionCylinder::s_TypeID( IGL_COLL_CYLINDER );

MStatus CollisionCylinder::Initialize()
{ 
  return inheritAttributesFrom( Cylinder::s_TypeName );
}

const char* CollisionCylinderChild::s_TypeName = "igCylinderChild";
const MTypeId CollisionCylinderChild::s_TypeID( IGL_COLL_CYLINDER_CHILD );

MStatus CollisionCylinderChild::Initialize()
{
  return inheritAttributesFrom( CylinderChild::s_TypeName );
}
