
#include "EnginePch.h"
#include "Engine/Facets.h"

using namespace Helium;

REFLECT_DEFINE_ABSTRACT(Helium::Facets::IFacet);

void Helium::Facets::Initialize()
{
  Reflect::RegisterClassType<Facets::IFacet>(TXT("IFacet"));
}

void Helium::Facets::Cleanup()
{
  Reflect::UnregisterClassType<Facets::IFacet>();
}