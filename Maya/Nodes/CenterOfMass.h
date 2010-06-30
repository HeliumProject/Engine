#pragma once

#include "Locator.h"

class CenterOfMass : public Locator
{
public:
  static const MTypeId s_TypeID;
	static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() {return new CenterOfMass;}
};;
