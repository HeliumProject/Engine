#include "Precompile.h"
#include "PrimitiveRadius.h"

using namespace Luna;

PrimitiveRadius::PrimitiveRadius(ResourceTracker* tracker)
: PrimitiveTemplate(tracker)
{
  SetElementType( ElementTypes::Position );

  m_Radius = 1.0f;
  m_RadiusSteps = 36;
}

void PrimitiveRadius::Update()
{
  __super::Update();
}