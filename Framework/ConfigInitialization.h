//----------------------------------------------------------------------------------------------------------------------
// ConfigInitialization.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_CONFIG_INITIALIZATION_H
#define LUNAR_FRAMEWORK_CONFIG_INITIALIZATION_H

#include "Framework/Framework.h"

namespace Helium
{
    /// Base class for initializing application configuration settings.
    class LUNAR_FRAMEWORK_API ConfigInitialization
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

#endif  // LUNAR_FRAMEWORK_CONFIG_INITIALIZATION_H
