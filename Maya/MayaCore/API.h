#pragma once

#ifdef MAYACORE_EXPORT
# define MAYACORE_API __declspec (dllexport)
#else
# define MAYACORE_API __declspec (dllimport)
#endif