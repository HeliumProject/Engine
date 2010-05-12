#pragma once

#ifdef WORKER_EXPORTS
#  define WORKER_API __declspec (dllexport)
#else
#  define WORKER_API __declspec (dllimport)
#endif