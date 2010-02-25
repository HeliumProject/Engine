#pragma once

#ifdef NOCTURNAL_STATIC
# define CONFIG_API
#else
# ifdef CONFIG_EXPORTS
#   define CONFIG_API __declspec (dllexport)
# else
#   define CONFIG_API __declspec (dllimport)
# endif
#endif