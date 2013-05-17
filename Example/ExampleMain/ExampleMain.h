//----------------------------------------------------------------------------------------------------------------------
// ExampleMain.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef EXAMPLE_EXAMPLE_MAIN_EXAMPLE_MAIN_H
#define EXAMPLE_EXAMPLE_MAIN_EXAMPLE_MAIN_H

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef EXAMPLE_EXAMPLE_MAIN_EXPORTS
        #define EXAMPLE_EXAMPLE_MAIN_API HELIUM_API_EXPORT
    #else
        #define EXAMPLE_EXAMPLE_MAIN_API HELIUM_API_IMPORT
    #endif
#else
    #define EXAMPLE_EXAMPLE_MAIN_API
#endif

#endif  // EXAMPLE_EXAMPLE_MAIN_EXAMPLE_MAIN_H
