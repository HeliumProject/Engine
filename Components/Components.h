//----------------------------------------------------------------------------------------------------------------------
// Engine.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_COMPONENTS_COMPONENTS_H
#define HELIUM_COMPONENTS_COMPONENTS_H

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_COMPONENTS_EXPORTS
        #define HELIUM_COMPONENTS_API HELIUM_API_EXPORT
    #else
        #define HELIUM_COMPONENTS_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_COMPONENTS_API
#endif

#endif  // HELIUM_COMPONENTS_COMPONENTS_H
