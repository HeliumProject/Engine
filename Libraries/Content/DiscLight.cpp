#include "DiscLight.h"
#include "ContentVisitor.h"

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_CLASS(DiscLight)


void DiscLight::Host(ContentVisitor* visitor)
{
  visitor->VisitDiscLight(this); 
}

