#pragma once

#include "Luna/CommandLine/Command.h"

#include "Platform/Compiler.h"

namespace Nocturnal
{
    namespace Luna
    {
        namespace CommandLine
        {
            class FailTest : public Command
            {
            private:
                const static std::string s_Token;
                const static std::string s_ShortHelp;
                const static std::string s_Help;

            public:
                FailTest();
                virtual ~FailTest();

                const std::string& Token() const
                {
                    return s_Token;
                }

                bool Process( std::vector< std::string >::const_iterator& itr, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) NOC_OVERRIDE; 
                const std::string& GetShortHelp() const
                {
                    return s_ShortHelp;
                }
                const std::string& GetHelp() const
                {
                    return s_Help;
                }
            };
        }
    }
}