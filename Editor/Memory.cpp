#include "EditorPch.h"

#include "Platform/MemoryHeap.h"

// Define the memory heap for the current module and include the "new"/"delete" operator implementations.
HELIUM_DEFINE_DEFAULT_MODULE_HEAP( Editor );

#if HELIUM_HEAP

#if HELIUM_SHARED
#include "Platform/NewDelete.h"
#endif

#endif // HELIUM_HEAP
