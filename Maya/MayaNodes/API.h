#pragma once

#ifdef MAYANODES_EXPORT
#define MAYANODES_API	__declspec (dllexport)
#else
#define MAYANODES_API	__declspec (dllimport)
#endif