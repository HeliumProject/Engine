//----------------------------------------------------------------------------------------------------------------------
// ObjectTypeRegistrationWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkWinPch.h"
#include "FrameworkWin/ObjectTypeRegistrationWin.h"
#include "EditorSupport/FontResourceHandler.h"

using namespace Helium;

/// @copydoc ObjectTypeRegistration::Register()
void ObjectTypeRegistrationWin::Register()
{
    ObjectTypeRegistration::Register();

// #if HELIUM_TOOLS
// 
//     //pmd - Other resource handlers get initialized when their types are registered, but fonts don't
//     //      follow that pattern. So for now, forcing an initialize here to prevent an assert later.
//     FontResourceHandler::InitializeStaticLibrary();
// #endif
}

/// @copydoc ObjectTypeRegistration::Unregister()
void ObjectTypeRegistrationWin::Unregister()
{
// #if HELIUM_TOOLS
//     //pmd - Other resource handlers get deinitted when their types are unregistered, but fonts don't
//     //      follow that pattern. So for now, forcing a deinit here.
//     FontResourceHandler::DestroyStaticLibrary();
// 
// #endif

    ObjectTypeRegistration::Unregister();
}
