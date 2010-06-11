#pragma once

#include "Option.h"
#include "Command.h"

namespace Nocturnal
{
    namespace Luna
    {
        namespace CommandLine
        {
            class Processor
            {
            private:
                M_StringToCommandDumbPtr m_Commands;
                M_StringToOptionDumbPtr  m_Options;

            public:

                Processor();
                virtual ~Processor();

                bool Process( const std::vector< std::string >& arguments, std::string& error );

                const M_StringToCommandDumbPtr& GetCommands();
                const M_StringToOptionDumbPtr& GetOptions();

                bool RegisterCommand( Command* command );
                void UnregisterCommand( Command* command );
                void UnregisterCommand( const std::string& token );
                const Command* GetCommand( const std::string& token );

                bool RegisterOption( Option* option );
                void UnregisterOption( Option* option );
                void UnregisterOption( const std::string& token );
                const Option* GetOption( const std::string& token );
            };
        }
    }
}