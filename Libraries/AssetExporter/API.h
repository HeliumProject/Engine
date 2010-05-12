#pragma once

#include "Common/Compiler.h"

#ifdef ASSETEXPORTER_EXPORTS
#define ASSETEXPORTER_API __declspec(dllexport)
#else
#define ASSETEXPORTER_API __declspec(dllimport)
#endif