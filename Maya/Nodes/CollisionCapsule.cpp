#include "Precompile.h"
#include "CollisionCapsule.h"

#include "MayaUtils/NodeTypes.h"

const MTypeId CollisionCapsule::s_TypeID( IGL_COLL_CAPSULE );
const char* CollisionCapsule::s_TypeName = "igCapsule";

MStatus CollisionCapsule::Initialize()
{ 
  return inheritAttributesFrom( Capsule::s_TypeName );
}

const MTypeId CollisionCapsuleChild::s_TypeID( IGL_COLL_CAPSULE_CHILD );
const char* CollisionCapsuleChild::s_TypeName = "igCapsuleChild";

MStatus CollisionCapsuleChild::Initialize()
{
  return inheritAttributesFrom( CapsuleChild::s_TypeName );
}