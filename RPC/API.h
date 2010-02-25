#pragma once

#ifdef WIN32
# ifdef RPC_EXPORTS
#  define RPC_API __declspec(dllexport)
# else
#  define RPC_API __declspec(dllimport)
# endif
#else
# define RPC_API
#endif
