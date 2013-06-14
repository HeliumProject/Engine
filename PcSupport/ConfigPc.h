#pragma once

#include "PcSupport/PcSupport.h"

namespace Helium
{
    class XmlSerializer;
    class Asset;

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
        //static void RecursiveSerializeObject( XmlSerializer& rSerializer, Asset* pObject );
        //@}
    };
}
