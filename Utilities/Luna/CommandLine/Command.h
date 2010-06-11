#pragma once

#include <vector>
#include <set>

namespace Nocturnal
{
    namespace Luna
    {
        namespace CommandLine
        {
            class Processor;

            class Command
            {
            protected:
                Processor* m_Owner;

            public:

                Command()
                {
                }
                virtual ~Command()
                {
                }

                virtual void SetOwner( Processor* owner )
                {
                    m_Owner = owner;
                }

                virtual const std::string& Token() const = 0;

                virtual bool Process( std::vector< std::string >::const_iterator& itr, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) = 0;
                virtual const std::string& GetShortHelp() const = 0;
                virtual const std::string& GetHelp() const = 0;
            };

            typedef std::set< Command > S_Command;
            typedef std::set< Command* > S_CommandDumbPtr;
            typedef std::map< std::string, Command > M_StringToCommand;
            typedef std::map< std::string, Command* > M_StringToCommandDumbPtr;
        }
    }
}