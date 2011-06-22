//----------------------------------------------------------------------------------------------------------------------
// Framework.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_FRAMEWORK_H
#define HELIUM_FRAMEWORK_FRAMEWORK_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef HELIUM_FRAMEWORK_EXPORTS
        #define HELIUM_FRAMEWORK_API HELIUM_API_EXPORT
    #else
        #define HELIUM_FRAMEWORK_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_FRAMEWORK_API
#endif

#endif  // HELIUM_FRAMEWORK_FRAMEWORK_H
