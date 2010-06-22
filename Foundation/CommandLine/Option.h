#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>

namespace Nocturnal
{
    namespace CommandLine
    {
        class Processor;

        class Option
        {
        protected:
            Processor* m_Owner;

        public:

            Option()
            {
            }
            virtual ~Option()
            {
            }

            virtual void SetOwner( Processor* owner )
            {
                m_Owner = owner;
            }

            virtual const std::string& Token() const = 0;

            virtual bool Process( std::string& error ) = 0;
            virtual const std::string& GetHelp() const = 0;
            virtual const std::string& GetShortHelp() const = 0;
        };

        typedef std::set< Option > S_Option;
        typedef std::set< Option* > S_OptionDumbPtr;
        typedef std::map< std::string, Option > M_StringToOption;
        typedef std::map< std::string, Option* > M_StringToOptionDumbPtr;
    }
}
