//----------------------------------------------------------------------------------------------------------------------
// ObjectTypeRegistration.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_OBJECT_TYPE_REGISTRATION_H
#define LUNAR_FRAMEWORK_OBJECT_TYPE_REGISTRATION_H

#include "Framework/Framework.h"

namespace Helium
{
    /// Base class for GameObject type registration support.
    ///
    /// This handles registration and unregistration for the following modules:
    /// - Engine
    /// - Graphics
    /// - Framework
    class LUNAR_FRAMEWORK_API ObjectTypeRegistration
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~ObjectTypeRegistration();
        //@}

        /// @name Type Registration
        //@{
        virtual void Register();
        virtual void Unregister();
        //@}
    };
}

#endif  // LUNAR_FRAMEWORK_OBJECT_TYPE_REGISTRATION_H
