#include "FoundationPch.h"
#include "FoundationPch.h"
#include "Foundation/Name.h"

using namespace Helium;

NameBase< CharNameTable >::TableBucket* CharNameTable::sm_pTable = NULL;
StackMemoryHeap<>* CharNameTable::sm_pNameMemoryHeap = NULL;
char CharNameTable::sm_emptyString[ 1 ] = { '\0' };

NameBase< WideNameTable >::TableBucket* WideNameTable::sm_pTable = NULL;
StackMemoryHeap<>* WideNameTable::sm_pNameMemoryHeap = NULL;
wchar_t WideNameTable::sm_emptyString[ 1 ] = { L'\0' };
