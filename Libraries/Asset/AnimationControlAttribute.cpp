#include "stdafx.h"
#include "AnimationControlAttribute.h"

using namespace Entity;

DEFINE_ELEMENT( AnimationControlAttribute )

u32 AnimationControlAttribute::GetAttributeUsage() const
{
  return Asset::AttributeUsages::Class;
}