#include "Processor.h"

#include <iomanip>

#include "Foundation/Log.h"
#include "Foundation/CommandLine/Commands/Help.h"
#include "Foundation/Container/Insert.h"

using namespace Helium::CommandLine;

Processor::Processor( const tchar* token, const tchar* usage, const tchar* shortHelp )
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
	Helium::Insert< M_StringToCommandDumbPtr >::Result inserted = m_Commands.insert( M_StringToCommandDumbPtr::value_type( command->Token(), command ) );
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
