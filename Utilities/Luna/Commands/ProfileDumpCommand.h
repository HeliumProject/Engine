#pragma once

#include "Foundation/CommandLine/Command.h"

namespace Luna
{
    class ProfileDumpCommand : public Nocturnal::CommandLine::Command
    {
    public:
        ProfileDumpCommand();

        virtual bool Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) NOC_OVERRIDE;
    };
}