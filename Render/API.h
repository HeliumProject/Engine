#pragma once

#ifdef RENDER_DLL
# ifdef RENDER_EXPORTS
#  define RENDER_API __declspec (dllexport)
# else
#  define RENDER_API __declspec (dllimport)
# endif
#else
# define RENDER_API
#endif
