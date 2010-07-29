#pragma once

#include "Foundation/Math/Vector3.h"

namespace Editor
{
  const extern Math::Axis SideAxis;
  const extern Math::Vector3 SideVector;

  const extern Math::Axis UpAxis;
  const extern Math::Vector3 UpVector;

  const extern Math::Axis OutAxis;
  const extern Math::Vector3 OutVector;

  inline Math::Vector3 SetupVector(float sideValue, float upValue, float outValue)
  {
    return (SideVector * sideValue) + (UpVector * upValue) + (OutVector * outValue);
  }
}
