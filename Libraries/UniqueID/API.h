#pragma once

#ifdef UNIQUEID_EXPORTS
# define UNIQUEID_API __declspec (dllexport)
#else
# define UNIQUEID_API __declspec (dllimport)
#endif
