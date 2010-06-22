#pragma once

#include "Foundation/API.h"
#include "Foundation/CommandLine/Flag.h"
#include "Foundation/CommandLine/Option.h"
#include "Foundation/CommandLine/Command.h"
#include "Foundation/CommandLine/Commands/Help.h"

#include "Platform/Compiler.h"

/******************************************************************************

Option
- Token
- Usage
- Help
- Value (<class T>)

Command
- Token
- Usage
- ShortHelp
- [Dynamic]Help
- Options
* Func: Un/Register Options
* Func: Parse - Parse command line
* Func: Process - Do the command with parsed options
  
Processor
- Token
- Usage
- ShortHelp
- [Dynamic]Help
- Options
- Commands
* Func: Un/Register Options
* Func: Un/Register Commands
* Func: Parse - Parse command line
* Func: Process - Do the command with parsed options

******************************************************************************/

namespace Nocturnal
{
    namespace CommandLine
    {
		class FOUNDATION_API Processor
        {
        protected:
			std::string m_Token;
			std::string m_Usage;
			std::string m_ShortHelp;
			mutable std::string m_Help;

			Nocturnal::CommandLine::Flag m_HelpFlag;
			Nocturnal::CommandLine::Help m_HelpCommand;

			M_StringToOptionDumbPtr m_Options;
            M_StringToCommandDumbPtr m_Commands;

        public:
            Processor( const char* token, const char* usage = "COMMAND [ARGS]", const char* shortHelp = "" );
            virtual ~Processor();

            const std::string& Token() const
			{
				return m_Token;
			}

			const std::string& ShortHelp() const
			{
				return m_ShortHelp;
			}

            virtual const std::string& Help() const;

            bool RegisterOption( Option* option );
            void UnregisterOption( Option* option );
            void UnregisterOption( const std::string& token );
            const Option* GetOption( const std::string& token );

			virtual void RegisterOptions();

            bool RegisterCommand( Command* command );
            void UnregisterCommand( Command* command );
            void UnregisterCommand( const std::string& token );
            const Command* GetCommand( const std::string& token );

			virtual void RegisterCommands();

			virtual bool Parse( const std::vector< std::string >& options, std::string& error );
			virtual bool Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error );
			virtual bool Process( std::string& error );
        };
    }
}
