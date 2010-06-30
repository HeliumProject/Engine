#pragma once

#include "Cylinder.h"

class GameplayCylinder : public Cylinder
{
public:
  static const MTypeId s_TypeID;
	static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() {return new GameplayCylinder;}
};
