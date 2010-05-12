#pragma once

#include "Common/Runtime.h"

#ifdef MANIFESTDLL_EXPORTS
# define MANIFEST_API __declspec(dllexport)
#else
# define MANIFEST_API __declspec(dllimport)
#endif