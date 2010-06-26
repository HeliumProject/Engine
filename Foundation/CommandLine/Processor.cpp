#include "Processor.h"

#include <iomanip>

#include "Foundation/Log.h"
#include "Foundation/CommandLine/Commands/Help.h"
#include "Foundation/Container/Insert.h"

using namespace Nocturnal::CommandLine;

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
			str << "  " << std::setfill(' ') << std::setw(18) << std::left << command->Token();// << " " << option->Usage();
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
	Nocturnal::Insert< M_StringToCommandDumbPtr >::Result inserted = m_Commands.insert( M_StringToCommandDumbPtr::value_type( command->Token(), command ) );
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