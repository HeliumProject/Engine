#pragma once

#include "API.h"

#include <maya/MObject.h>

#include "Foundation/TUID.h"

namespace Maya
{
  MAYAUTILS_API Nocturnal::TUID GetNodeID (const MObject& node, bool create = true);
  MAYAUTILS_API MStatus SetNodeID (const MObject& node, const Nocturnal::TUID& id);
}