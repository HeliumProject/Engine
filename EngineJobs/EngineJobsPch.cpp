//----------------------------------------------------------------------------------------------------------------------
// EngineJobsPch.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EngineJobsPch.h"

// Define the memory heap for the current module and include the "new"/"delete" operator implementations.
HELIUM_DEFINE_DEFAULT_MODULE_HEAP( EngineJobs );

#if L_DEBUG
#include "Platform/NewDelete.h"
#endif
