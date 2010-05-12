#pragma once

#include "Capsule.h"

class GameplayCapsule : public Capsule
{
public:
  static const MTypeId	s_TypeID;
  static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() {return new GameplayCapsule;}
};
