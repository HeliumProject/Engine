#pragma once

#if HELIUM_SHARED
# ifdef HELIUM_PIPELINE_EXPORTS
#  define HELIUM_PIPELINE_API __declspec (dllexport)
# else
#  define HELIUM_PIPELINE_API __declspec (dllimport)
# endif
#else
# define HELIUM_PIPELINE_API
#endif
