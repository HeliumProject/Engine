#include "SphereLight.h"
#include "ContentVisitor.h"

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_CLASS(SphereLight)



void SphereLight::Host(ContentVisitor* visitor)
{
  visitor->VisitSphereLight(this); 
}

