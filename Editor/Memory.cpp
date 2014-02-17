#include "EditorPch.h"

#include "Platform/MemoryHeap.h"

#if HELIUM_HEAP

HELIUM_DEFINE_DEFAULT_MODULE_HEAP( Editor );

#if HELIUM_SHARED
#include "Platform/NewDelete.h"
#endif

#endif // HELIUM_HEAP
