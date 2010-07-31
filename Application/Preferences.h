#pragma once

#include "Application/API.h"

#include "Foundation/File/Path.h"

namespace Helium
{
    namespace Application
    {
        APPLICATION_API bool GetPreferencesDirectory( Helium::Path& preferencesDirectory );
    }
}