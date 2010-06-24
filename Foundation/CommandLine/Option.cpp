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
		Nocturnal::Insert< M_StringToOptionPtr >::Result inserted = m_OptionsMap.insert( M_StringToOptionPtr::value_type( (*tokensItr), option ) );
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