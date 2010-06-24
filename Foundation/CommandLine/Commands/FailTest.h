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

			virtual bool Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error ) NOC_OVERRIDE;
        };
    }
}
