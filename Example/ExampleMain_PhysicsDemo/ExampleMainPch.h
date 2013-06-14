//----------------------------------------------------------------------------------------------------------------------
// ExampleMainPch.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef EXAMPLE_EXAMPLE_MAIN_PCH_H
#define EXAMPLE_EXAMPLE_MAIN_PCH_H

#include "ExampleMain_PhysicsDemo/ExampleMain.h"

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
#include "ExampleGame/ExampleGamePch.h"

#include <tchar.h>

#endif  // EXAMPLE_EXAMPLE_MAIN_PCH_H
