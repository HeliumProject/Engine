#include "Precompile.h"
#include "GameplayCuboid.h"

#include "MayaUtils/NodeTypes.h"

const MTypeId GameplayCuboid::s_TypeID( IGL_GP_CUBOID );
const char* GameplayCuboid::s_TypeName = "gpCuboid";

MStatus GameplayCuboid::Initialize()
{
  return inheritAttributesFrom( Cuboid::s_TypeName );
}