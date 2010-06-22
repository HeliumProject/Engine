#pragma once

#include "Foundation/API.h"
#include "Foundation/CommandLine/Command.h"

#include "Platform/Compiler.h"

namespace Nocturnal
{
    namespace CommandLine
    {
        class FOUNDATION_API FailTest : public Command
        {
        public:
            FailTest();
            virtual ~FailTest();

			virtual bool Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) NOC_OVERRIDE;
			virtual bool Process( std::string& error ) NOC_OVERRIDE;
        };
    }
}
