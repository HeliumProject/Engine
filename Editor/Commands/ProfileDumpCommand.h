#pragma once

#include "Foundation/CmdLineProcessor.h"

namespace Helium
{
    namespace Editor
    {
        class ProfileDumpCommand : public Helium::CommandLine::Command
        {
        public:
            ProfileDumpCommand();

            virtual bool Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error ) HELIUM_OVERRIDE;
        };
    }
}