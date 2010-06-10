#pragma once

#ifdef UID_EXPORTS
# define UID_API __declspec (dllexport)
#else
# define UID_API __declspec (dllimport)
#endif
