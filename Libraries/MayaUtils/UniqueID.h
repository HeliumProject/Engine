#pragma once

#include "API.h"

#include <maya/MObject.h>

#include "UniqueID/TUID.h"

namespace Maya
{
  MAYAUTILS_API UniqueID::TUID GetNodeID (const MObject& node, bool create = true);
  MAYAUTILS_API MStatus SetNodeID (const MObject& node, const UniqueID::TUID& id);
}