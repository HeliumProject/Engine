#include "Help.h"

#include "Foundation/CommandLine/Processor.h"
#include "Foundation/Log.h"

using namespace Nocturnal;
using namespace Nocturnal::CommandLine;

Help::Help( Processor* owner )
: Command( TXT( "help" ), TXT( "<COMMAND>" ), TXT( "Displays the help for the command (or application)" ) )
, m_Owner( owner )
{
}

Help::~Help()
{
	m_Owner = NULL;
}

bool Help::Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error )
{
	NOC_ASSERT( m_Owner );

	if ( argsBegin == argsEnd )
	{
        Log::Print( TXT( "\nPrinting help for Luna...\n" ) );
        Log::Print( m_Owner->Help().c_str() );
        Log::Print( TXT( "\n" ) );
		return true;
	}
	else
	{
		m_CommandName = (*argsBegin);
		++argsBegin;

		const Command* command = m_Owner->GetCommand( m_CommandName );
		if ( command )
		{
            Log::Print( TXT( "\nGetting help for command: %s...\n" ), m_CommandName.c_str() );
            Log::Print( command->Help().c_str() );
            Log::Print( TXT( "\n" ) );
			return true;
		}
		else
		{
			error = tstring( TXT( "No help for unknown command: " ) ) + m_CommandName;
			return false;
		}
	}

    return false;
}
