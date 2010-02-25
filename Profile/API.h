#pragma once

#ifdef WIN32
# ifdef NOCTURNAL_STATIC
#  define PROFILE_API
# else
#  ifdef PROFILEDLL_EXPORTS
#   define PROFILE_API __declspec(dllexport)
#  else
#   define PROFILE_API __declspec(dllimport)
#  endif
# endif
#else
# define PROFILE_API
#endif
