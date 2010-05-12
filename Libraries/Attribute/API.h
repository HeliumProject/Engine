#pragma once

#include "Common/Compiler.h"

#ifdef ATTRIBUTEDLL_EXPORTS
#define ATTRIBUTE_API __declspec(dllexport)
#else
#define ATTRIBUTE_API __declspec(dllimport)
#endif
