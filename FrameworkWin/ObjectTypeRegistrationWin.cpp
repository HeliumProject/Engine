//----------------------------------------------------------------------------------------------------------------------
// ObjectTypeRegistrationWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkWinPch.h"
#include "FrameworkWin/ObjectTypeRegistrationWin.h"
#include "EditorSupport/FontResourceHandler.h"

/// Type registration functions.
extern void RegisterPcSupportTypes();
extern void UnregisterPcSupportTypes();

#if L_EDITOR
extern void RegisterEditorSupportTypes();
extern void UnregisterEditorSupportTypes();
#endif

using namespace Lunar;

/// @copydoc ObjectTypeRegistration::Register()
void ObjectTypeRegistrationWin::Register()
{
    ObjectTypeRegistration::Register();

    RegisterPcSupportTypes();
#if L_EDITOR
    RegisterEditorSupportTypes();

    //pmd - Other resource handlers get initialized when their types are registered, but fonts don't
    //      follow that pattern. So for now, forcing an initialize here to prevent an assert later.
    FontResourceHandler::InitializeStaticLibrary();
#endif
}

/// @copydoc ObjectTypeRegistration::Unregister()
void ObjectTypeRegistrationWin::Unregister()
{
#if L_EDITOR
    //pmd - Other resource handlers get deinitted when their types are unregistered, but fonts don't
    //      follow that pattern. So for now, forcing a deinit here.
    FontResourceHandler::DestroyStaticLibrary();

    UnregisterEditorSupportTypes();
#endif
    UnregisterPcSupportTypes();

    ObjectTypeRegistration::Unregister();
}
