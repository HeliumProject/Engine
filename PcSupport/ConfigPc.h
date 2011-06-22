//----------------------------------------------------------------------------------------------------------------------
// ConfigPc.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_PC_SUPPORT_CONFIG_PC_H
#define HELIUM_PC_SUPPORT_CONFIG_PC_H

#include "PcSupport/PcSupport.h"

namespace Helium
{
    class XmlSerializer;
    class GameObject;

    /// PC configuration support.
    class HELIUM_PC_SUPPORT_API ConfigPc
    {
    public:
        /// @name Configuration Saving
        //@{
        static bool SaveUserConfig();
        //@}

    private:
        /// @name Private Utility Functions
        //@{
        static void RecursiveSerializeObject( XmlSerializer& rSerializer, GameObject* pObject );
        //@}
    };
}

#endif  // HELIUM_PC_SUPPORT_CONFIG_PC_H
