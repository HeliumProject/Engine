#pragma once

#include "Option.h"
#include "Verb.h"

namespace Nocturnal
{
    namespace CommandLine
    {
        class Processor
        {
        private:
            M_StringToVerbDumbPtr    m_Verbs;
            M_StringToOptionDumbPtr  m_Options;

        public:

            Processor();
            virtual ~Processor();

            bool Process( const std::vector< std::string >& arguments, std::string& error );

            const M_StringToVerbDumbPtr& GetVerbs();
            const M_StringToOptionDumbPtr& GetOptions();

            bool RegisterVerb( Verb* verb );
            void UnregisterVerb( Verb* verb );
            void UnregisterVerb( const std::string& token );
            const Verb* GetVerb( const std::string& token );

            bool RegisterOption( Option* option );
            void UnregisterOption( Option* option );
            void UnregisterOption( const std::string& token );
            const Option* GetOption( const std::string& token );
        };
    }
}
