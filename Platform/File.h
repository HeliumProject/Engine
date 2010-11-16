#pragma once

#ifdef WIN32
# include "Platform/Windows/File.h"
#else
# include "Platform/POSIX/File.h"
#endif

namespace Helium
{

}