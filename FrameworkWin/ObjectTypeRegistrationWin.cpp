//----------------------------------------------------------------------------------------------------------------------
// ObjectTypeRegistrationWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkWinPch.h"
#include "FrameworkWin/ObjectTypeRegistrationWin.h"

/// Type registration functions.
extern void RegisterPcSupportTypes();
extern void UnregisterPcSupportTypes();

#if L_EDITOR
extern void RegisterEditorSupportTypes();
extern void UnregisterEditorSupportTypes();
#endif

namespace Lunar
{
    /// @copydoc ObjectTypeRegistration::Register()
    void ObjectTypeRegistrationWin::Register()
    {
        ObjectTypeRegistration::Register();

        RegisterPcSupportTypes();
#if L_EDITOR
        RegisterEditorSupportTypes();
#endif
    }

    /// @copydoc ObjectTypeRegistration::Unregister()
    void ObjectTypeRegistrationWin::Unregister()
    {
#if L_EDITOR
        UnregisterEditorSupportTypes();
#endif
        UnregisterPcSupportTypes();

        ObjectTypeRegistration::Unregister();
    }
}
