#pragma once

#include "Common/Compiler.h"

#ifdef TEXPACKBUILDERDLL_EXPORTS
#define TEXPACKBUILDER_API __declspec(dllexport)
#else
#define TEXPACKBUILDER_API __declspec(dllimport)
#endif