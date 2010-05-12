#include "stdafx.h"
#include "NavClueCuboid.h"

#include "MayaUtils/NodeTypes.h"

const MTypeId NavClueCuboid::s_TypeID(IGL_NAV_CLUE_CUBOID_ID);
const char* NavClueCuboid::s_TypeName = "igNavClueCuboid";

MStatus NavClueCuboid::Initialize()
{
  return inheritAttributesFrom( Cuboid::s_TypeName );
}
