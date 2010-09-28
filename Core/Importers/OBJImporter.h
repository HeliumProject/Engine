#pragma once

#include "Core/API.h"

#include "Core/SceneGraph/Mesh.h"
#include "Foundation/File/Path.h"

namespace Helium
{
    namespace Importers
    {
        CORE_API SceneGraph::Mesh* ImportOBJ( const Path& path );
    }
}