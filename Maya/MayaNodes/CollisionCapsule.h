#pragma once

#include "Capsule.h"

class CollisionCapsule : public Capsule
{
public:
  static const MTypeId	s_TypeID;
  static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() {return new CollisionCapsule;}
};

class CollisionCapsuleChild : public CapsuleChild
{
public: 
  static const MTypeId	s_TypeID;
  static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() {return new CollisionCapsuleChild;}
};