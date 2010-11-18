//----------------------------------------------------------------------------------------------------------------------
// PreprocessingPc.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_PREPROCESSING_PC_PREPROCESSING_PC_H
#define LUNAR_PREPROCESSING_PC_PREPROCESSING_PC_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef LUNAR_PREPROCESSING_PC_EXPORTS
        #define LUNAR_PREPROCESSING_PC_API HELIUM_API_EXPORT
    #else
        #define LUNAR_PREPROCESSING_PC_API HELIUM_API_IMPORT
    #endif
#else
    #define LUNAR_PREPROCESSING_PC_API
#endif

#endif  // LUNAR_PREPROCESSING_PC_PREPROCESSING_PC_H
