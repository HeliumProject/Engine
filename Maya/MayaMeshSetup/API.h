#pragma once

#ifdef MAYAMESHSETUP_EXPORTS
#define MAYAMESHSETUP_API	__declspec (dllexport)
#else
#define MAYAMESHSETUP_API	__declspec (dllimport)
#endif