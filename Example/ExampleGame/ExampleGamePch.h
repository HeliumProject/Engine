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
#include "Framework/Components.h"
#include "Engine/Package.h"
#include "Engine/Asset.h"
#include "Framework/TaskScheduler.h"
#include "Framework/Components.h"
#include "Framework/ComponentQuery.h"
#include "Framework/ComponentDefinition.h"
#include "Framework/Entity.h"
#include "Framework/EntityDefinition.h"
#include "Framework/Slice.h"
#include "Framework/SceneDefinition.h"

EXAMPLE_GAME_API void ForceLoadExampleGameDll();
