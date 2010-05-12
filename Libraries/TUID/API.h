#pragma once

#ifdef TUID_EXPORTS
# define TUID_API __declspec (dllexport)
#else
# define TUID_API __declspec (dllimport)
#endif
