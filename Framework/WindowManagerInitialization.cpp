//----------------------------------------------------------------------------------------------------------------------
// WindowManagerInitialization.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/WindowManagerInitialization.h"

using namespace Helium;

/// Destructor.
WindowManagerInitialization::~WindowManagerInitialization()
{
}

/// @fn bool WindowManagerInitialization::Initialize()
/// Create and initialize a new WindowManager instance.
///
/// Note that a window manager is optional.  If no window manager is intended to be created, this will still return
/// true (success) even though the window manager instance is null.
///
/// @return  True if initialization was successful or no window manager was created intentionally, false if window
///          manager creation failed.
