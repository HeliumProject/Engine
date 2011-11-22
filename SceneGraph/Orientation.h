#pragma once

#include "Foundation/Math/Axes.h"
#include "Foundation/Math/FpuVector3.h"

namespace Helium
{
    namespace SceneGraph
    {
        const extern Axis SideAxis;
        const extern Vector3 SideVector;

        const extern Axis UpAxis;
        const extern Vector3 UpVector;

        const extern Axis OutAxis;
        const extern Vector3 OutVector;

        inline Vector3 SetupVector(float sideValue, float upValue, float outValue)
        {
            return (SideVector * sideValue) + (UpVector * upValue) + (OutVector * outValue);
        }
    }
}