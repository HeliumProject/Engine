#include "ApplicationPch.h"
#include "CmdLineProcessor.h"

#include "Foundation/Log.h"
#include "Foundation/Insert.h"
#include "Foundation/Tokenize.h"

#include <iomanip>

using namespace Helium::CommandLine;

OptionsMap::OptionsMap()
{
}

OptionsMap::~OptionsMap()
{
	m_OptionsMap.clear();
	m_Options.clear();
}

const tstring& OptionsMap::Usage() const
{
	if ( m_Usage.empty() )
	{
		for ( V_OptionPtr::const_iterator argsBegin = m_Options.begin(), argsEnd = m_Options.end(); argsBegin != argsEnd; ++argsBegin )
		{
			const Option* option = (*argsBegin);

			m_Usage += tstring( TXT( " [-" ) ) + option->Token();

			if ( !option->Usage().empty() )
			{
				m_Usage += tstring( TXT( " " ) ) + option->Usage();
			}
			m_Usage += tstring( TXT( "]" ) );
		}
	}
	return m_Usage;
}

const tstring& OptionsMap::Help() const
{
	if ( m_Help.empty() )
	{
		m_Help += tstring( TXT( "Options:\n" ) );

		tstringstream str;
		
		for ( V_OptionPtr::const_iterator argsBegin = m_Options.begin(), argsEnd = m_Options.end(); argsBegin != argsEnd; ++argsBegin )
		{
			const Option* option = (*argsBegin);
			// m_Help += tstring( "  " ) + option->Token() + tstring( " " ) + option->Usage() + tstring( "\t" ) + option->Help() + tstring( "\n" );
			str << TXT( "  -" ) << std::setfill( TXT( ' ' ) ) << std::setw(18) << std::left << option->Token();// << " " << option->Usage();
			str << TXT( " " ) << option->Help() << std::endl;
		}

		m_Help += str.str();
	}
	return m_Help;
}

bool OptionsMap::AddOption( const OptionPtr& option, tstring& error )
{
	std::set< tstring > tokens;
	Tokenize( option->Token(), tokens, TXT( "\\|" ) );
	for ( std::set< tstring >::const_iterator tokensItr = tokens.begin(), tokensEnd = tokens.end(); tokensItr != tokensEnd; ++tokensItr )
	{
		Helium::StdInsert< M_StringToOptionPtr >::Result inserted = m_OptionsMap.insert( M_StringToOptionPtr::value_type( (*tokensItr), option ) );
		if ( !inserted.second )
		{
			error = tstring( TXT( "Failed to add option, token is not unique: " ) ) + (*tokensItr);
			return false;
		}
	}

	m_Options.push_back( option );

	m_Help.clear();
	return true;
}

bool OptionsMap::ParseOptions( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error )
{
	bool result = true;

	while ( result && ( argsBegin != argsEnd ) )
	{
		const tstring& arg = (*argsBegin);

		if ( arg.length() >= 1 && arg[ 0 ] == '-' )
		{
			M_StringToOptionPtr::const_iterator optionsItr = m_OptionsMap.find( arg.substr( 1 ) );
			if ( optionsItr != m_OptionsMap.end() )
			{
				argsBegin++;
				result &= (*optionsItr).second->Parse( argsBegin, argsEnd, error );
			}
			else
			{
				error = tstring( TXT( "Unknown option: " ) ) + arg;
				result = false;
			}
		}
		else
		{
			// end of command line options
			break;
		}
	}

	return result;
}

Command::Command( const tchar_t* token, const tchar_t* usage, const tchar_t* shortHelp )
: m_Token( token )
, m_Usage( usage )
, m_ShortHelp( shortHelp )
{
}

Command::~Command()
{
}

const tstring& Command::Help() const
{
	if ( m_Help.empty() )
	{
		// Usage
		m_Help += tstring( TXT( "\nUsage: " ) ) + m_Token + m_OptionsMap.Usage() + tstring( TXT( " " ) ) + m_Usage + tstring( TXT( "\n" ) );

		m_Help += tstring( TXT( "\n" ) ) + m_ShortHelp + tstring( TXT( "\n" ) );

		// Options
		m_Help += tstring( TXT( "\n" ) ) + m_OptionsMap.Help();
	}
	return m_Help;
}

bool Command::AddOption( const OptionPtr& option, tstring& error )
{
	return m_OptionsMap.AddOption( option, error );
}

bool Command::ParseOptions( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error )
{
	return m_OptionsMap.ParseOptions( argsBegin, argsEnd, error );
}

HelpCommand::HelpCommand( Processor* owner )
: Command( TXT( "help" ), TXT( "<COMMAND>" ), TXT( "Displays the help for the command (or application)" ) )
, m_Owner( owner )
{
}

HelpCommand::~HelpCommand()
{
	m_Owner = NULL;
}

bool HelpCommand::Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error )
{
	HELIUM_ASSERT( m_Owner );

	if ( argsBegin == argsEnd )
	{
		Log::Print( TXT( "\nPrinting help for Editor...\n" ) );
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

Processor::Processor( const tchar_t* token, const tchar_t* usage, const tchar_t* shortHelp )
: m_Token( token )
, m_Usage( usage )
, m_ShortHelp( shortHelp )
{
}

Processor::~Processor()
{
	for ( M_StringToCommandDumbPtr::iterator argsBegin = m_Commands.begin(), end = m_Commands.end(); argsBegin != end; ++argsBegin )
	{
		(*argsBegin).second = NULL;
	}
}

const tstring& Processor::Help() const
{
	if ( m_Help.empty() )
	{
		// Usage
		m_Help += tstring( TXT( "\nUsage: " ) ) + m_Token + m_OptionsMap.Usage() + tstring( TXT( " " ) ) + m_Usage + tstring( TXT( "\n" ) );

		m_Help += tstring( TXT( "\n" ) ) + m_ShortHelp + tstring( TXT( "\n" ) );

		// Options
		m_Help += tstring( TXT( "\n" ) ) + m_OptionsMap.Help();

		// Commands
		tstringstream str;
		
		m_Help += tstring( TXT( "\nCommands:\n" ) );
		for ( M_StringToCommandDumbPtr::const_iterator argsBegin = m_Commands.begin(), argsEnd = m_Commands.end(); argsBegin != argsEnd; ++argsBegin )
		{
			const Command* command = (*argsBegin).second;
			//m_Help += tstring( "  " ) + command->Token() + tstring( "\t" ) + command->ShortHelp() + tstring( "\n" );
			str << TXT( "  " ) << std::setfill( TXT( ' ' ) ) << std::setw(18) << std::left << command->Token();// << " " << option->Usage();
			str << TXT( " " ) << command->ShortHelp() << std::endl;
		}

		m_Help += str.str();
	}
	return m_Help;
}

bool Processor::AddOption( const OptionPtr& option, tstring& error )
{
	return m_OptionsMap.AddOption( option, error );
}

bool Processor::ParseOptions( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error )
{
	return m_OptionsMap.ParseOptions( argsBegin, argsEnd, error );
}

bool Processor::RegisterCommand( Command* command, tstring& error )
{
	Helium::StdInsert< M_StringToCommandDumbPtr >::Result inserted = m_Commands.insert( M_StringToCommandDumbPtr::value_type( command->Token(), command ) );
	if ( !inserted.second )
	{
		error = tstring( TXT( "Failed to add command, token is not unique: " ) ) + command->Token();
		return false;
	}

	m_Help.clear();
	return true;
}

Command* Processor::GetCommand( const tstring& token )
{
	Command* command = NULL;
	M_StringToCommandDumbPtr::iterator commandsItr = m_Commands.find( token );
	if ( commandsItr != m_Commands.end() )
	{
		command = (*commandsItr).second;
	}
	return command;
}

bool Processor::Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error )
{
	if ( !ParseOptions( argsBegin, argsEnd, error ) )
	{
		return false;
	}

	bool result = true;

	while ( result && ( argsBegin != argsEnd ) )
	{
		const tstring& arg = (*argsBegin);
		++argsBegin;

		if ( arg.length() < 1 )
			continue;

		if ( arg[ 0 ] == '-' )
		{
			error = tstring( TXT( "Unknown option, or option passed out of order: " ) ) + arg;
			result = false;
		}
		else
		{
			M_StringToCommandDumbPtr::iterator commandItr = m_Commands.find( arg );
			if ( commandItr != m_Commands.end() )
			{
				result &= (*commandItr).second->Process( argsBegin, argsEnd, error );
			}
			else
			{
				error = tstring( TXT( "Unknown commandline parameter: " ) ) + arg + TXT( "\n\n" );
				result = false;
			}
		}
	}

	return result;
}
