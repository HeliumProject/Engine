//----------------------------------------------------------------------------------------------------------------------
// ExampleMain.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef EXAMPLE_EMPTY_MAIN_EMPTY_MAIN_H
#define EXAMPLE_EMPTY_MAIN_EMPTY_MAIN_H

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef EXAMPLE_EMPTY_MAIN_EXPORTS
        #define EMPTY_MAIN_API HELIUM_API_EXPORT
    #else
        #define EMPTY_MAIN_API HELIUM_API_IMPORT
    #endif
#else
    #define EMPTY_MAIN_API
#endif

#endif  // EXAMPLE_EMPTY_MAIN_EMPTY_MAIN_H
