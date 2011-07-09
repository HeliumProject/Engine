#pragma once

#include "Pipeline/API.h"

#include "Foundation/File/Path.h"

#include "SceneGraph/Mesh.h"

namespace Helium
{
    namespace Importers
    {
        HELIUM_PIPELINE_API SceneGraph::Mesh* ImportOBJ( const Path& path, bool flipWinding = false );
    }
}