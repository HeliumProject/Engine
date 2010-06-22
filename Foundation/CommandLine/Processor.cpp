#include "Processor.h"

#include "Foundation/Log.h"

using namespace Nocturnal::CommandLine;

Processor::Processor( const char* token, const char* usage, const char* shortHelp )
: m_Token( token )
, m_Usage( usage )
, m_ShortHelp( shortHelp )
, m_HelpFlag( "help", "print command usage" )
{
	m_HelpCommand.SetOwner( this );
}

Processor::~Processor()
{
	for ( M_StringToOptionDumbPtr::iterator argsBegin = m_Options.begin(), argsEnd = m_Options.end(); argsBegin != argsEnd; ++argsBegin )
	{
		delete (*argsBegin).second;
	}

    for ( M_StringToCommandDumbPtr::iterator argsBegin = m_Commands.begin(), end = m_Commands.end(); argsBegin != end; ++argsBegin )
    {
        delete (*argsBegin).second;
    }
}

const std::string& Processor::Help() const
{
	if ( m_Help.empty() )
	{
		m_Help += m_ShortHelp + std::string( "\n" );
		
		m_Help += std::string( "\nUsage: " ) + m_Token + std::string( " " ) + m_Usage + std::string( "\n" );

		m_Help += std::string( "\nCommands:\n" );
		for ( M_StringToCommandDumbPtr::const_iterator argsBegin = m_Commands.begin(), argsEnd = m_Commands.end(); argsBegin != argsEnd; ++argsBegin )
		{
			const Command* command = (*argsBegin).second;
			m_Help += std::string( "  " ) + command->Token() + std::string( "\t" ) + command->ShortHelp() + std::string( "\n" );
		}
	}
	return m_Help;
}

bool Processor::RegisterOption( Option* option )
{
	m_Options[ option->Token() ] = option;
	return true;
}

void Processor::UnregisterOption( Option* option )
{
	m_Options.erase( option->Token() );
	m_Help.clear();
}

void Processor::UnregisterOption( const std::string& token )
{
	m_Options.erase( token );
	m_Help.clear();
}

const Option* Processor::GetOption( const std::string& token )
{
	Option* option = NULL;
	M_StringToOptionDumbPtr::iterator optionsItr = m_Options.find( token );
	if ( optionsItr != m_Options.end() )
	{
		option = (*optionsItr).second;
	}
	return option;
}

void Processor::RegisterOptions()
{
	RegisterOption( &m_HelpFlag );
}

bool Processor::RegisterCommand( Command* command )
{
	m_Commands[ command->Token() ] = command;
	return true;
}

void Processor::UnregisterCommand( Command* command )
{
	m_Commands.erase( command->Token() );
	m_Help.clear();
}

void Processor::UnregisterCommand( const std::string& token )
{
	m_Commands.erase( token );
	m_Help.clear();
}

const Command* Processor::GetCommand( const std::string& token )
{
	Command* command = NULL;
	M_StringToCommandDumbPtr::iterator commandsItr = m_Commands.find( token );
	if ( commandsItr != m_Commands.end() )
	{
		command = (*commandsItr).second;
	}
	return command;
}

void Processor::RegisterCommands()
{
	RegisterCommand( &m_HelpCommand );
}

bool Processor::Parse( const std::vector< std::string >& options, std::string& error )
{
	return Parse( options.begin(), options.end(), error );
}

bool Processor::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
	bool result = true;

	while ( result && ( argsBegin != argsEnd ) )
	{
        const std::string& arg = (*argsBegin);
        argsBegin++;

        if ( arg.length() >= 1 && arg[ 0 ] == '-' )
        {
            M_StringToOptionDumbPtr::const_iterator optionItr = m_Options.find( arg.substr( 1 ) );
            if ( optionItr != m_Options.end() )
            {
                result &= (*optionItr).second->Parse( argsBegin, argsEnd, error );
            }
            else
            {
                error = std::string( "Unknown option: " ) + arg;
                return false;
            }
        }
        else
        {
            M_StringToCommandDumbPtr::iterator commandItr = m_Commands.find( arg );
            if ( commandItr != m_Commands.end() )
            {
                result &= (*commandItr).second->Parse( argsBegin, argsEnd, error );
            }
            else
            {
                error = std::string( "Unknown command: " ) + arg + "\n\n";
				return false;
            }
        }
    }

    return result;
}