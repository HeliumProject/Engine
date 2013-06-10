//----------------------------------------------------------------------------------------------------------------------
// ExampleGame.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef EXAMPLE_EXAMPLE_GAME_EXAMPLE_GAME_H
#define EXAMPLE_EXAMPLE_GAME_EXAMPLE_GAME_H

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef EXAMPLE_GAME_EXPORTS
        #define EXAMPLE_GAME_API HELIUM_API_EXPORT
    #else
        #define EXAMPLE_GAME_API HELIUM_API_IMPORT
    #endif
#else
    #define EXAMPLE_GAME_API
#endif

#define EXAMPLE_GAME_MAX_WORLDS (1)
#define EXAMPLE_GAME_MAX_PLAYERS (4)

#endif  // EXAMPLE_EXAMPLE_GAME_EXAMPLE_GAME_H
