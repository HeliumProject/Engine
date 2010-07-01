#include "Precompile.h"
#include "CenterOfMass.h"

#include "Maya/NodeTypes.h"

const MTypeId CenterOfMass::s_TypeID(IGL_CENTER_OF_MASS);
const char* CenterOfMass::s_TypeName = "centerOfMass";

MStatus CenterOfMass::Initialize()
{ 
  return inheritAttributesFrom( Locator::s_TypeName );
}
