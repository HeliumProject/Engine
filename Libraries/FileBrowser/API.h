#pragma once

#include "Common/Compiler.h"

#ifdef FILEBROWSER_DLL
# ifdef FILEBROWSER_EXPORTS
#  define FILEBROWSER_API __declspec(dllexport)
# else
#  define FILEBROWSER_API __declspec(dllimport)
# endif
#else
# define FILEBROWSER_API
#endif