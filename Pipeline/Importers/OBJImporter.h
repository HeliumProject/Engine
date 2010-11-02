#pragma once

#include "Pipeline/API.h"

#include "Pipeline/SceneGraph/Mesh.h"
#include "Foundation/File/Path.h"

namespace Helium
{
    namespace Importers
    {
        PIPELINE_API SceneGraph::Mesh* ImportOBJ( const Path& path, bool flipWinding = false );
    }
}