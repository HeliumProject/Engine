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
		m_Help += m_ShortHelp + std::string( "\n" );

		// Usage
		m_Help += std::string( "\nUsage: " ) + m_Token + m_OptionsMap.Usage() + std::string( " " ) + m_Usage + std::string( "\n" );

		// Options
		m_Help += std::string( "\n" ) + m_OptionsMap.Help();
	}
	return m_Help;
}

