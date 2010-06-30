#pragma once

#include "Sphere.h"

class CollisionSphere : public Sphere
{
public:
  static const MTypeId s_TypeID;
  static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() {return new CollisionSphere;}
};
