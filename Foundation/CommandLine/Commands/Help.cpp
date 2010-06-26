#include "Help.h"

#include "Foundation/CommandLine/Processor.h"
#include "Foundation/Log.h"

using namespace Nocturnal;
using namespace Nocturnal::CommandLine;

Help::Help( Processor* owner )
: Command( "help", "<COMMAND>", "displays the help for the command (or application)" )
, m_Owner( owner )
{
}

Help::~Help()
{
	m_Owner = NULL;
}

bool Help::Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
	NOC_ASSERT( m_Owner );

	if ( argsBegin == argsEnd )
	{
        Log::Print( "\nPrinting help for Luna...\n" );
        Log::Print( m_Owner->Help().c_str() );
        Log::Print( "\n" );
		return true;
	}
	else
	{
		m_CommandName = (*argsBegin);
		++argsBegin;

		const Command* command = m_Owner->GetCommand( m_CommandName );
		if ( command )
		{
            Log::Print( "\nGetting help for command: %s...\n", m_CommandName.c_str() );
            Log::Print( command->Help().c_str() );
            Log::Print( "\n" );
			return true;
		}
		else
		{
			error = std::string( "No help for unknown command: " ) + m_CommandName;
			return false;
		}
	}

    return false;
}
