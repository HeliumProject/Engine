#include "Help.h"

#include "Foundation/CommandLine/Processor.h"
#include "Foundation/Log.h"

using namespace Nocturnal;
using namespace Nocturnal::CommandLine;

Help::Help( Processor* owner )
: Command( "help", "<COMMAND>", "Displays the help for the command (or application if no command specified)" )
, m_Owner( owner )
{
}

Help::~Help()
{
	m_Owner = NULL;
}

bool Help::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
	if ( argsBegin == argsEnd )
	{
		//error = std::string( "Please specify a command." );
		m_CommandName.clear();

		return false;
	}
	else
	{
		m_CommandName = (*argsBegin);
		++argsBegin;

		return true;
	}

    return false;
}

bool Help::Process( std::string& error )
{
	if ( m_Owner )
	{
		if ( m_CommandName.empty() )
		{
			Log::Print( m_Owner->Help().c_str() );
			return true;
		}
		else
		{
			const Command* command = m_Owner->GetCommand( m_CommandName );
			if ( command )
			{
				Log::Print( command->Help().c_str() );
				return true;
			}
			else
			{
				error = std::string( "No help for command: " ) + m_CommandName;
				return false;
			}
		}
	}

	return false;
}