//----------------------------------------------------------------------------------------------------------------------
// TestApp.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_TEST_APP_TEST_APP_H
#define HELIUM_TEST_APP_TEST_APP_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef HELIUM_TEST_APP_EXPORTS
        #define HELIUM_TEST_APP_API HELIUM_API_EXPORT
    #else
        #define HELIUM_TEST_APP_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_TEST_APP_API
#endif

#endif  // HELIUM_TEST_APP_TEST_APP_H
