//----------------------------------------------------------------------------------------------------------------------
// ExampleGame.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef EXAMPLE_EXAMPLE_GAME_EXAMPLE_GAME_H
#define EXAMPLE_EXAMPLE_GAME_EXAMPLE_GAME_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef EXAMPLE_EXAMPLE_GAME_EXPORTS
        #define EXAMPLE_EXAMPLE_GAME_API HELIUM_API_EXPORT
    #else
        #define EXAMPLE_EXAMPLE_GAME_API HELIUM_API_IMPORT
    #endif
#else
    #define EXAMPLE_EXAMPLE_GAME_API
#endif

#endif  // EXAMPLE_EXAMPLE_GAME_EXAMPLE_GAME_H
