#pragma once

#include "Capsule.h"

class LooseAttachmentCapsule : public Capsule
{
public:
  static const MTypeId s_TypeID;
  static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() {return new LooseAttachmentCapsule;}
};

class LooseAttachmentCapsuleChild : public CapsuleChild
{
public: 
  static const MTypeId s_TypeID;
  static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() {return new LooseAttachmentCapsuleChild;}
};