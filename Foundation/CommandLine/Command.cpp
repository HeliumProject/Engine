#include "Command.h"

#include "Foundation/Log.h"
#include "Foundation/String/Tokenize.h"

using namespace Nocturnal::CommandLine;

Command::Command( const tchar* token, const tchar* usage, const tchar* shortHelp )
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
		m_Help += m_ShortHelp + tstring( TXT( "\n" ) );

		// Usage
		m_Help += tstring( TXT( "\nUsage: " ) ) + m_Token + m_OptionsMap.Usage() + tstring( TXT( " " ) ) + m_Usage + tstring( TXT( "\n" ) );

		// Options
		m_Help += tstring( TXT( "\n" ) ) + m_OptionsMap.Help();
	}
	return m_Help;
}

