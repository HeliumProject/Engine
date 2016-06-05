#pragma once

#include "Application/CmdLineProcessor.h"

namespace Helium
{
    namespace Editor
    {
        class ProfileDumpCommand : public Helium::CommandLine::Command
        {
        public:
            ProfileDumpCommand();

            virtual bool Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) override;
        };
    }
}