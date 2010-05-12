#pragma once

#include "Cylinder.h"

class CollisionCylinder : public Cylinder
{
public:
  static const MTypeId s_TypeID;
	static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() {return new CollisionCylinder;}
};

class CollisionCylinderChild : public CylinderChild
{
public: 
  static const MTypeId s_TypeID;
	static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() { return new CollisionCylinderChild; }
};