#include "Command.h"

#include "Foundation/Log.h"
#include "Foundation/String/Tokenize.h"

using namespace Nocturnal::CommandLine;

Command::Command( const char* token, const char* usage, const char* shortHelp )
: m_Token( token )
, m_Usage( usage )
, m_ShortHelp( shortHelp )
{
}

Command::~Command()
{
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

