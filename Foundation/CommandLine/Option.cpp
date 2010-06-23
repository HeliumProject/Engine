#include "Option.h"

#include <iomanip>

#include "Foundation/Log.h"
#include "Foundation/Container/Insert.h"
#include "Foundation/String/Tokenize.h"

using namespace Nocturnal::CommandLine;

OptionsMap::OptionsMap()
{
}

OptionsMap::~OptionsMap()
{
	m_OptionsMap.clear();
	m_Options.clear();
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
			str << "  -" << std::setfill(' ') << std::setw(18) << std::left << option->Token();// << " " << option->Usage();
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
		Nocturnal::Insert< M_StringToOptionPtr >::Result inserted = m_OptionsMap.insert( M_StringToOptionPtr::value_type( (*tokensItr), option ) );
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