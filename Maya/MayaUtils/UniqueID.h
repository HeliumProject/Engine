#pragma once

#include "API.h"

#include <maya/MObject.h>

#include "UID/TUID.h"

namespace Maya
{
  MAYAUTILS_API Nocturnal::UID::TUID GetNodeID (const MObject& node, bool create = true);
  MAYAUTILS_API MStatus SetNodeID (const MObject& node, const Nocturnal::UID::TUID& id);
}