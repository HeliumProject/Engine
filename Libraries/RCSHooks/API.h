#pragma once

#ifdef RCSHOOKS_EXPORTS
# define RCSHOOKS_API __declspec(dllexport)
#else
# define RCSHOOKS_API __declspec(dllimport)
#endif
