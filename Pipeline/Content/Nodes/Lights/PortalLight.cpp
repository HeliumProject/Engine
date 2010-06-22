#include "PortalLight.h"
#include "Pipeline/Content/ContentVisitor.h"

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_CLASS(PortalLight)

void PortalLight::Host(ContentVisitor* visitor)
{
  visitor->VisitPortalLight(this); 
}
