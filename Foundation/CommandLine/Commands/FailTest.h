#pragma once

#include "Foundation/API.h"
#include "Foundation/CommandLine/Command.h"

namespace Helium
{
    namespace CommandLine
    {
        class FOUNDATION_API FailTest : public Command
        {
        public:
            FailTest();
            virtual ~FailTest();

			virtual bool Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error ) HELIUM_OVERRIDE;
        };
    }
}
