#include "Platform/Memory.h"

// Define the memory heap for the current module and include the "new"/"delete" operator implementations.
#pragma TODO( "Rename EditorApp module heap to Editor once naming conflicts with Lunar/Editor are resolved." )
HELIUM_DEFINE_DEFAULT_MODULE_HEAP( EditorApp );

#if HELIUM_SHARED
#include "Platform/NewDelete.h"
#endif
