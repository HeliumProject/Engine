#pragma once

#include "Cuboid.h"

class NavClueCuboid : public Cuboid
{
public:
  static const MTypeId s_TypeID;
  static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() {return new NavClueCuboid;}
};
