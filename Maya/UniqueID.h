#pragma once

#include "API.h"

#include <maya/MObject.h>

#include "Foundation/TUID.h"

namespace Maya
{
  MAYA_API Nocturnal::TUID GetNodeID (const MObject& node, bool create = true);
  MAYA_API MStatus SetNodeID (const MObject& node, const Nocturnal::TUID& id);
}