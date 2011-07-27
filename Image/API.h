#pragma once

#if HELIUM_SHARED
# ifdef HELIUM_IMAGE_EXPORTS
#  define HELIUM_IMAGE_API __declspec (dllexport)
# else
#  define HELIUM_IMAGE_API __declspec (dllimport)
# endif
#else
# define HELIUM_IMAGE_API
#endif
