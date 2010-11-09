//----------------------------------------------------------------------------------------------------------------------
// Threading.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_THREADING_H
#define LUNAR_CORE_THREADING_H

#include "Core/Core.h"

#include "Platform/Platform.h"
#include "Platform/Utility.h"

#if HELIUM_OS_WIN
#include "ThreadingWin.h"
#include "ThreadingWin.inl"
#endif

#endif  // LUNAR_CORE_THREADING_H
