//----------------------------------------------------------------------------------------------------------------------
// PcSupport.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_PC_SUPPORT_PC_SUPPORT_H
#define HELIUM_PC_SUPPORT_PC_SUPPORT_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef HELIUM_PC_SUPPORT_EXPORTS
        #define HELIUM_PC_SUPPORT_API HELIUM_API_EXPORT
    #else
        #define HELIUM_PC_SUPPORT_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_PC_SUPPORT_API
#endif

#endif  // HELIUM_PC_SUPPORT_PC_SUPPORT_H
