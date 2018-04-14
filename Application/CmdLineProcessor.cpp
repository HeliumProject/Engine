#include "ApplicationPch.h"
#include "CmdLineProcessor.h"

#include "Foundation/Log.h"
#include "Foundation/Tokenize.h"

#include <iomanip>

using namespace Helium;
using namespace Helium::CommandLine;

Option::Option( const char* token, const char* usage, const char* help )
	: m_Token( token )
	, m_Usage( usage )
	, m_Help( help )
{
}

const std::string& Option::Token() const
{
	return m_Token;
}

const std::string& Option::Usage() const
{
	return m_Usage;
}

const std::string& Option::Help() const
{
	return m_Help;
}

template <>
bool SimpleOption<std::string>::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
	if ( argsBegin != argsEnd )
	{
		*m_Data = (*argsBegin);
		++argsBegin;

		//HELIUM_ASSERT( !(*m_Data).empty() );

		return true;
	}

	error = std::string( "Missing parameter for option: " ) + m_Token;
	return false;
}

template <>
bool SimpleOption<bool>::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
	// TODO: use in_avail
	if ( argsBegin != argsEnd )
	{
		const std::string& arg = (*argsBegin);
		++argsBegin;

		if ( CaseInsensitiveCompareString( arg.c_str(), "false" ) == 0 || CaseInsensitiveCompareString( arg.c_str(), "0" ) == 0 )
		{
			*m_Data = false;
		}
		else if ( CaseInsensitiveCompareString( arg.c_str(), "true" ) == 0 || CaseInsensitiveCompareString( arg.c_str(), "1" ) == 0 )
		{
			*m_Data = true;
		}

		return true;
	}

	error = std::string( "Missing parameter for option: " ) + m_Token;
	return false;
}

template <>
bool SimpleOption< std::vector< std::string > >::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
	// tokenize and push_back via m_Data
	bool result = false;

	while ( argsBegin != argsEnd )
	{
		// stop looking once we get to the optional params
		const std::string& arg = (*argsBegin);

		if ( arg.length() >= 1 )
		{
			if ( arg[ 0 ] == '-' )
			{
				break;
			}
			else
			{
				++argsBegin;

				(*m_Data).push_back( arg );

				result = true;
			}
		}

	}

	if ( !result || (*m_Data).empty() )
	{
		error = std::string( "Must pass one or more arguments to the option: " ) + m_Token;
		return false;
	}

	return result;
}

FlagOption::FlagOption( bool* data, const char* token, const char* help )
	: SimpleOption( data, token, "", help )
	, m_Data( data )
{
	*m_Data = false;
}

bool FlagOption::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
	*m_Data = true;
	return true;
}


OptionsMap::OptionsMap()
{
}

const std::string& OptionsMap::Usage() const
{
	if ( m_Usage.empty() )
	{
		for ( V_OptionPtr::const_iterator argsBegin = m_Options.begin(), argsEnd = m_Options.end(); argsBegin != argsEnd; ++argsBegin )
		{
			const Option* option = (*argsBegin);

			m_Usage += std::string( " [-" ) + option->Token();

			if ( !option->Usage().empty() )
			{
				m_Usage += std::string( " " ) + option->Usage();
			}
			m_Usage += std::string( "]" );
		}
	}
	return m_Usage;
}

const std::string& OptionsMap::Help() const
{
	if ( m_Help.empty() )
	{
		m_Help += std::string( "Options:\n" );

		std::stringstream str;
		
		for ( V_OptionPtr::const_iterator argsBegin = m_Options.begin(), argsEnd = m_Options.end(); argsBegin != argsEnd; ++argsBegin )
		{
			const Option* option = (*argsBegin);
			// m_Help += std::string( "  " ) + option->Token() + std::string( " " ) + option->Usage() + std::string( "\t" ) + option->Help() + std::string( "\n" );
			str << "  -" << std::setfill( ' ' ) << std::setw(18) << std::left << option->Token();// << " " << option->Usage();
			str << " " << option->Help() << std::endl;
		}

		m_Help += str.str();
	}
	return m_Help;
}

bool OptionsMap::AddOption( const OptionPtr& option, std::string& error )
{
	std::set< std::string > tokens;
	Tokenize( option->Token(), tokens, "\\|" );
	for ( std::set< std::string >::const_iterator tokensItr = tokens.begin(), tokensEnd = tokens.end(); tokensItr != tokensEnd; ++tokensItr )
	{
		std::pair< M_StringToOptionPtr::const_iterator, bool > inserted = m_OptionsMap.insert( M_StringToOptionPtr::value_type( (*tokensItr), option ) );
		if ( !inserted.second )
		{
			error = std::string( "Failed to add option, token is not unique: " ) + (*tokensItr);
			return false;
		}
	}

	m_Options.push_back( option );

	m_Help.clear();
	return true;
}

bool OptionsMap::ParseOptions( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
	bool result = true;

	while ( result && ( argsBegin != argsEnd ) )
	{
		const std::string& arg = (*argsBegin);

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
				error = std::string( "Unknown option: " ) + arg;
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

Command::Command( const char* token, const char* usage, const char* shortHelp )
: m_Token( token )
, m_Usage( usage )
, m_ShortHelp( shortHelp )
{
}

bool Command::Initialize( std::string& error )
{
	return true;
}

void Command::Cleanup()
{
}

const std::string& Command::Token() const
{
	return m_Token;
}

const std::string& Command::ShortHelp() const
{
	return m_ShortHelp;
}

const std::string& Command::Help() const
{
	if ( m_Help.empty() )
	{
		// Usage
		m_Help += std::string( "\nUsage: " ) + m_Token + m_OptionsMap.Usage() + std::string( " " ) + m_Usage + std::string( "\n" );

		m_Help += std::string( "\n" ) + m_ShortHelp + std::string( "\n" );

		// Options
		m_Help += std::string( "\n" ) + m_OptionsMap.Help();
	}
	return m_Help;
}

bool Command::AddOption( const OptionPtr& option, std::string& error )
{
	return m_OptionsMap.AddOption( option, error );
}

bool Command::ParseOptions( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
	return m_OptionsMap.ParseOptions( argsBegin, argsEnd, error );
}

HelpCommand::HelpCommand( Processor* owner )
: Command( "help", "<COMMAND>", "Displays the help for the command (or application)" )
, m_Owner( owner )
{
}

bool HelpCommand::Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
	HELIUM_ASSERT( m_Owner );

	if ( argsBegin == argsEnd )
	{
		Log::Print( "\nPrinting help for Editor...\n" );
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

Processor::Processor( const char* token, const char* usage, const char* shortHelp )
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

bool Processor::Initialize( std::string& error )
{
	return true;
}

void Processor::Cleanup()
{
}

const std::string& Processor::Token() const
{
	return m_Token;
}

const std::string& Processor::ShortHelp() const
{
	return m_ShortHelp;
}

const std::string& Processor::Help() const
{
	if ( m_Help.empty() )
	{
		// Usage
		m_Help += std::string( "\nUsage: " ) + m_Token + m_OptionsMap.Usage() + std::string( " " ) + m_Usage + std::string( "\n" );

		m_Help += std::string( "\n" ) + m_ShortHelp + std::string( "\n" );

		// Options
		m_Help += std::string( "\n" ) + m_OptionsMap.Help();

		// Commands
		std::stringstream str;
		
		m_Help += std::string( "\nCommands:\n" );
		for ( M_StringToCommandDumbPtr::const_iterator argsBegin = m_Commands.begin(), argsEnd = m_Commands.end(); argsBegin != argsEnd; ++argsBegin )
		{
			const Command* command = (*argsBegin).second;
			//m_Help += std::string( "  " ) + command->Token() + std::string( "\t" ) + command->ShortHelp() + std::string( "\n" );
			str << "  " << std::setfill( ' ' ) << std::setw(18) << std::left << command->Token();// << " " << option->Usage();
			str << " " << command->ShortHelp() << std::endl;
		}

		m_Help += str.str();
	}
	return m_Help;
}

bool Processor::AddOption( const OptionPtr& option, std::string& error )
{
	return m_OptionsMap.AddOption( option, error );
}

bool Processor::ParseOptions( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
	return m_OptionsMap.ParseOptions( argsBegin, argsEnd, error );
}

bool Processor::RegisterCommand( Command* command, std::string& error )
{
	std::pair< M_StringToCommandDumbPtr::const_iterator, bool > inserted = m_Commands.insert( M_StringToCommandDumbPtr::value_type( command->Token(), command ) );
	if ( !inserted.second )
	{
		error = std::string( "Failed to add command, token is not unique: " ) + command->Token();
		return false;
	}

	m_Help.clear();
	return true;
}

Command* Processor::GetCommand( const std::string& token )
{
	Command* command = NULL;
	M_StringToCommandDumbPtr::iterator commandsItr = m_Commands.find( token );
	if ( commandsItr != m_Commands.end() )
	{
		command = (*commandsItr).second;
	}
	return command;
}

bool Processor::Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
	if ( !ParseOptions( argsBegin, argsEnd, error ) )
	{
		return false;
	}

	bool result = true;

	while ( result && ( argsBegin != argsEnd ) )
	{
		const std::string& arg = (*argsBegin);
		++argsBegin;

		if ( arg.length() < 1 )
			continue;

		if ( arg[ 0 ] == '-' )
		{
			error = std::string( "Unknown option, or option passed out of order: " ) + arg;
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
				error = std::string( "Unknown commandline parameter: " ) + arg + "\n\n";
				result = false;
			}
		}
	}

	return result;
}
