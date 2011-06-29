//----------------------------------------------------------------------------------------------------------------------
// System.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_SYSTEM_H
#define HELIUM_FRAMEWORK_SYSTEM_H

#include "Framework/Framework.h"
#include "Platform/Utility.h"

#include "Foundation/String.h"

namespace Helium
{
    /// Base interface for management of application and component initialization and the main application loop.
    class HELIUM_FRAMEWORK_API System : NonCopyable
    {
    public:
        /// @name Initialization
        //@{
        virtual void Shutdown();
        //@}

        /// @name Static Access
        //@{
        static System* GetStaticInstance();
        static void DestroyStaticInstance();
        //@}

    protected:
        /// Module file name.
        String m_moduleName;
        /// Command-line arguments (not including the module name).
        DynArray< String > m_arguments;

        /// Singleton instance.
        static System* sm_pInstance;

        /// @name Construction/Destruction
        //@{
        System();
        virtual ~System() = 0;
        //@}
    };
}

#endif  // HELIUM_FRAMEWORK_SYSTEM_H
