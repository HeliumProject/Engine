#pragma once

#include "Foundation/API.h"
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
			tstring m_Token;
			tstring m_Usage;
			tstring m_ShortHelp;
			mutable tstring m_Help;

			bool m_HelpFlag;
			Help m_HelpCommand;

			OptionsMap m_OptionsMap;
            M_StringToCommandDumbPtr m_Commands;

        public:
            Processor( const tchar* token, const tchar* usage = TXT( "COMMAND [ARGS]" ), const tchar* shortHelp = TXT( "" ) );
            virtual ~Processor();

			virtual bool Initialize( tstring& error );

            const tstring& Token() const
			{
				return m_Token;
			}

			const tstring& ShortHelp() const
			{
				return m_ShortHelp;
			}

            virtual const tstring& Help() const;

            bool RegisterCommand( Command* command, tstring& error );
            const Command* GetCommand( const tstring& token );

			virtual bool Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error );
        };
    }
}
