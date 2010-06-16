#pragma once

#ifdef NOCTURNAL_STATIC
# define PIPELINE_API
#else
# ifdef PIPELINE_EXPORTS
#  define PIPELINE_API __declspec (dllexport)
# else
#  define PIPELINE_API __declspec (dllimport)
# endif
#endif
