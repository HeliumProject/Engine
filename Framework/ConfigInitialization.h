//----------------------------------------------------------------------------------------------------------------------
// ConfigInitialization.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_CONFIG_INITIALIZATION_H
#define HELIUM_FRAMEWORK_CONFIG_INITIALIZATION_H

#include "Framework/Framework.h"

namespace Helium
{
    /// Base class for initializing application configuration settings.
    class HELIUM_FRAMEWORK_API ConfigInitialization
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~ConfigInitialization();
        //@}

        /// @name Config Initialization
        //@{
        virtual bool Initialize();
        //@}
    };
}

#endif  // HELIUM_FRAMEWORK_CONFIG_INITIALIZATION_H
