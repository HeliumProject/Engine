// stdafx.cpp : source file that includes just the standard includes
// TestApp.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// Define the memory heap for the current module and include the "new"/"delete" operator implementations.
HELIUM_DEFINE_DEFAULT_MODULE_HEAP( TestApp );

#if L_DEBUG
#include "Platform/NewDelete.h"
#endif
