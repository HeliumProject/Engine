//----------------------------------------------------------------------------------------------------------------------
// ObjectTypeRegistrationWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_WIN_OBJECT_TYPE_REGISTRATION_WIN_H
#define LUNAR_FRAMEWORK_WIN_OBJECT_TYPE_REGISTRATION_WIN_H

#include "FrameworkWin/FrameworkWin.h"
#include "Framework/ObjectTypeRegistration.h"

namespace Helium
{
    /// Object type registration base class for Windows.
    class LUNAR_FRAMEWORK_WIN_API ObjectTypeRegistrationWin : public ObjectTypeRegistration
    {
    public:
        /// @name Type Registration
        //@{
        virtual void Register();
        virtual void Unregister();
        //@}
    };
}

#endif  // LUNAR_FRAMEWORK_WIN_OBJECT_TYPE_REGISTRATION_WIN_H
