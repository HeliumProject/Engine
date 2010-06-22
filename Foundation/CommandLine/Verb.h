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

        class Verb
        {
        protected:
            Processor* m_Owner;

        public:

            Verb()
            {
            }
            virtual ~Verb()
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

        typedef std::set< Verb > S_Verb;
        typedef std::set< Verb* > S_VerbDumbPtr;
        typedef std::map< std::string, Verb > M_StringToVerb;
        typedef std::map< std::string, Verb* > M_StringToVerbDumbPtr;
    }
}
