//----------------------------------------------------------------------------------------------------------------------
// Engine.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_ENGINE_ENGINE_H
#define HELIUM_ENGINE_ENGINE_H

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_ENGINE_EXPORTS
        #define HELIUM_ENGINE_API HELIUM_API_EXPORT
    #else
        #define HELIUM_ENGINE_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_ENGINE_API
#endif

#endif  // HELIUM_ENGINE_ENGINE_H
