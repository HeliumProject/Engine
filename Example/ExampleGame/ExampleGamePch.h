//----------------------------------------------------------------------------------------------------------------------
// ExampleGamePch.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ExampleGame/ExampleGame.h"

#include "Platform/Assert.h"
#include "Platform/Trace.h"
#include "Platform/Memory.h"
#include "Foundation/DynamicArray.h"
#include "Reflect/Structure.h"
#include "Engine/Components.h"
#include "Engine/Package.h"
#include "Engine/Asset.h"
#include "Engine/AssetType.h"
#include "Engine/TaskScheduler.h"
#include "Engine/Components.h"
#include "Engine/ComponentQuery.h"
#include "Framework/ComponentDefinition.h"
#include "Framework/Entity.h"
#include "Framework/EntityDefinition.h"
#include "Framework/Slice.h"
#include "Framework/SceneDefinition.h"

EXAMPLE_GAME_API void ForceLoadExampleGameDll();
