#pragma once

#include "Cuboid.h"

class NavEffectorCuboid : public Cuboid
{
public:
  static const MTypeId s_TypeID;
	static const char* s_TypeName;
	static MStatus Initialize();
  static void* Creator() {return new NavEffectorCuboid;}
};