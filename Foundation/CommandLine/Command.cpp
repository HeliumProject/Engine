#include "Command.h"

#include "Foundation/Log.h"

using namespace Nocturnal::CommandLine;

Command::Command( const char* token, const char* usage, const char* shortHelp )
: m_Token( token )
, m_Usage( usage )
, m_ShortHelp( shortHelp )
, m_HelpFlag( "help", "print command usage" )
{
}

Command::~Command()
{
	for ( M_StringToOptionDumbPtr::iterator argsBegin = m_Options.begin(), argsEnd = m_Options.end(); argsBegin != argsEnd; ++argsBegin )
	{
		delete (*argsBegin).second;
	}
}

const std::string& Command::Help() const
{
	if ( m_Help.empty() )
	{		
		m_Help += std::string( "\nUsage: " ) + m_Token;
		//size_t indent = m_Help.length() + 1;
		//size_t lineLen = indent;

		for ( M_StringToOptionDumbPtr::const_iterator argsBegin = m_Options.begin(), argsEnd = m_Options.end(); argsBegin != argsEnd; ++argsBegin )
		{
			const Option* option = (*argsBegin).second;

			//if ( ( lineLen + 5 + option->Token().length() + option->Usage().length() ) > 80 )
			//{
			//	char indentString[80] = "";
			//	if(indent >= sizeof(indentString))
			//	{
			//		indent = sizeof(indentString)-1;
			//	}

			//	for (int i=0; i<indent; i++)
			//	{
			//		indentString[i] = ' ';
			//	}
			//	indentString[indent] = '\0';

			//	m_Help += std::string( "\n" ) + std::string( indentString );

			//	lineLen = 0;
			//}

			m_Help += std::string( " [-" ) + option->Token();
	        //lineLen += 3;
			if ( !option->Usage().empty() )
			{
				m_Help += std::string( " " ) + option->Usage();
				//lineLen += option->Usage().length() + 1;
			}
			m_Help += std::string( "]" );
			//lineLen += 1;
		}
		m_Help += std::string( " " ) + m_Usage + std::string( "\n" );

		m_Help += std::string( "\nOptions:\n" );
		for ( M_StringToOptionDumbPtr::const_iterator argsBegin = m_Options.begin(), argsEnd = m_Options.end(); argsBegin != argsEnd; ++argsBegin )
		{
			const Option* option = (*argsBegin).second;
			m_Help += std::string( "  " ) + option->Token() + std::string( " " ) + option->Usage() + std::string( "\t" ) + option->Help() + std::string( "\n" );
		}
	}
	return m_Help;
}

bool Command::RegisterOption( Option* option )
{
	m_Options[ option->Token() ] = option;
	return true;
}

void Command::UnregisterOption( Option* option )
{
	m_Options.erase( option->Token() );
	m_Help.clear();
}

void Command::UnregisterOption( const std::string& token )
{
	m_Options.erase( token );
	m_Help.clear();
}

const Option* Command::GetOption( const std::string& token )
{
	Option* option = NULL;
	M_StringToOptionDumbPtr::iterator optionsItr = m_Options.find( token );
	if ( optionsItr != m_Options.end() )
	{
		option = (*optionsItr).second;
	}
	return option;
}

void Command::RegisterOptions()
{
	RegisterOption( &m_HelpFlag );
}

bool Command::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
	bool result = true;

	while ( result && ( argsBegin != argsEnd ) )
	{
		const std::string& arg = (*argsBegin);
		argsBegin++;

		if ( arg.length() >= 1 && arg[ 0 ] == '-' )
		{
			M_StringToOptionDumbPtr::const_iterator optionsItr = m_Options.find( arg.substr( 1 ) );
			if ( optionsItr != m_Options.end() )
			{
				result &= (*optionsItr).second->Parse( argsBegin, argsEnd, error );
			}
			else
			{
				error = std::string( "Unknown option: " ) + arg;
				result = false;
			}
		}
	}

	return result;
}

bool Command::Process( std::string& error )
{
	if ( m_HelpFlag.IsSet() )
	{
		Log::Print( Help().c_str() );
		return true;
	}

	return false;
}