#pragma once

#include "Platform/Types.h"

#include <string>

// Forwards
class wxConfigBase;

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Class the wraps up interaction with the Windows Registry or alternative.
        // You can read and write persistent string values using this class.
        // 
        class SimpleConfig
        {
        private:
            wxConfigBase* m_Config;

        private:
            SimpleConfig();

        public:
            static SimpleConfig* GetInstance();

            bool Read( const std::string& relativePath, const std::string& key, std::string& value );
            bool Write( const std::string& relativePath, const std::string& key, const std::string& value );
        };
    }
}
