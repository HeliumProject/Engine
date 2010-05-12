#pragma once

#include "Common/Compiler.h"

#ifdef MAYASHADERMANAGERDLL_EXPORTS
#define MAYASHADERMANAGER_API __declspec(dllexport)
#else
#define MAYASHADERMANAGER_API __declspec(dllimport)
#endif