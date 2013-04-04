#include "RenderingD3D9Pch.h"

#include "Platform/MemoryHeap.h"

#if HELIUM_HEAP

// Define the memory heap for the current module and include the "new"/"delete" operator implementations.
HELIUM_DEFINE_DEFAULT_MODULE_HEAP( RenderingD3D9 );

#if HELIUM_DEBUG
#include "Platform/NewDelete.h"
#endif

#endif // HELIUM_HEAP
