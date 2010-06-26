#pragma once

#include "Foundation/API.h"
#include "Foundation/CommandLine/Option.h"
#include "Foundation/CommandLine/Command.h"

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

			OptionsMap m_OptionsMap;
            M_StringToCommandDumbPtr m_Commands;

        public:
            Processor( const char* token, const char* usage = "COMMAND [ARGS]", const char* shortHelp = "" );
            virtual ~Processor();

			virtual bool Initialize( std::string& error )
            {
                return true;
            }

            const std::string& Token() const
			{
				return m_Token;
			}

			const std::string& ShortHelp() const
			{
				return m_ShortHelp;
			}

            virtual const std::string& Help() const;

            bool AddOption( const OptionPtr& option, std::string& error );
            bool ParseOptions( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error );

            bool RegisterCommand( Command* command, std::string& error );
            Command* GetCommand( const std::string& token );

			virtual bool Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error );
        };
    }
}
