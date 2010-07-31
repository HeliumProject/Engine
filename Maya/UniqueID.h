#pragma once

#include "API.h"

#include <maya/MObject.h>

#include "Foundation/TUID.h"

namespace Helium
{
    namespace Maya
    {
        MAYA_API Helium::TUID GetNodeID (const MObject& node, bool create = true);
        MAYA_API MStatus SetNodeID (const MObject& node, const Helium::TUID& id);
    }
}