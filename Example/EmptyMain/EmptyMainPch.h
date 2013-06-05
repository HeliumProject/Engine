//----------------------------------------------------------------------------------------------------------------------
// EmptyMainPch.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef EXAMPLE_EMPTY_MAIN_PCH_H
#define EXAMPLE_EMPTY_MAIN_PCH_H

#include "EmptyMain/EmptyMain.h"

#include "Platform/Trace.h"
#include "Framework/GameSystem.h"
#include "FrameworkWin/MemoryHeapPreInitializationWin.h"
#include "Framework/NullRendererInitialization.h"
#include "FrameworkWin/CommandLineInitializationWin.h"
#include "FrameworkWin/AssetLoaderInitializationWin.h"
#include "FrameworkWin/ConfigInitializationWin.h"
#include "FrameworkWin/WindowManagerInitializationWin.h"
#include "FrameworkWin/RendererInitializationWin.h"
#include "Foundation/FilePath.h"
#include "Engine/FileLocations.h"
#include "Engine/CacheManager.h"

#include <tchar.h>

#endif  // EXAMPLE_EMPTY_MAIN_PCH_H
