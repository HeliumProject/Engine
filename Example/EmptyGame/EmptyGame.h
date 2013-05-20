//----------------------------------------------------------------------------------------------------------------------
// EmptyGame.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef EXAMPLE_EMPTY_GAME_EMPTY_GAME_H
#define EXAMPLE_EMPTY_GAME_EMPTY_GAME_H

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef EMPTY_GAME_EXPORTS
        #define EMPTY_GAME_API HELIUM_API_EXPORT
    #else
        #define EMPTY_GAME_API HELIUM_API_IMPORT
    #endif
#else
    #define EMPTY_GAME_API
#endif

#endif  // EXAMPLE_EMPTY_GAME_EMPTY_GAME_H
