//----------------------------------------------------------------------------------------------------------------------
// PreprocessingPc.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_PREPROCESSING_PC_PREPROCESSING_PC_H
#define HELIUM_PREPROCESSING_PC_PREPROCESSING_PC_H

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_PREPROCESSING_PC_EXPORTS
        #define HELIUM_PREPROCESSING_PC_API HELIUM_API_EXPORT
    #else
        #define HELIUM_PREPROCESSING_PC_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_PREPROCESSING_PC_API
#endif

#endif  // HELIUM_PREPROCESSING_PC_PREPROCESSING_PC_H
