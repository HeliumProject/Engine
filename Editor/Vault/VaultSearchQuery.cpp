#include "Precompile.h"
#include "VaultSearchQuery.h"

#include "Editor/App.h"
#include "Foundation/Regex.h"
#include "Foundation/MD5.h"
#include "Foundation/Tokenize.h"
#include "Foundation/Log.h"

HELIUM_DEFINE_CLASS( Helium::Editor::VaultSearchQuery );

using namespace Helium;
using namespace Helium::Editor;

//
// Searching
//

/////////////////////////////////////////////////////////////////////////////
// Phrase:
//  - literal-word
//  - double quoted group of literal-words
//  
// ColumnAlias:
//  - alias name of a searchable table column (must be 2 or more characters)
//
// ColumnQuery:
//  - ColumnAlias : Phrase
//  
// Query:
//  - Phrase
//  - ColumnQuery
//namespace TokenTypes
//{
//  enum TokenType
//  {
//    File,
//    Directory,
//    Phrase,
//    ColumnName,
//    ComponentName,
//  };
//}

#define MATCH_WORD             "[a-z0-9_\\-\\.\\\\/:\\*]+"
#define MATCH_PHRASE           "[a-z0-9_\\-\\.\\\\/:\\s\\*]+"
#define MATCH_COLUMN_NAME      "[a-z][a-z0-9_\\-]{1,}"

const char* s_ParseWord             = "(" MATCH_WORD ")";
const char* s_ParsePhrase           = "[\"](" MATCH_PHRASE ")[\"]";
const char* s_ParseColumnName       = "(" MATCH_COLUMN_NAME ")\\s*[:=]\\s*";
const char* s_TokenizeQueryString   = "(" MATCH_COLUMN_NAME "\\s*[:=]\\s*|[\"]" MATCH_PHRASE "[\"]|" MATCH_WORD ")";

///////////////////////////////////////////////////////////////////////////////
void VaultSearchQuery::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &VaultSearchQuery::m_QueryString, "m_QueryString" );
}

///////////////////////////////////////////////////////////////////////////////
VaultSearchQuery::VaultSearchQuery()
: m_SQLQueryString( "" )
{

}

VaultSearchQuery::~VaultSearchQuery()
{
}

///////////////////////////////////////////////////////////////////////////////
void VaultSearchQuery::PostDeserialize( const Reflect::Field* field )
{
    Base::PostDeserialize( field );

    if ( field == NULL )
    {
        std::string errors;
        if ( !ParseQueryString( m_QueryString, errors, this ) )
        {
            Log::Warning( "Errors occurred while parsing the query string: %s\n  %s\n", m_QueryString.c_str(), errors.c_str() );
            return;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool VaultSearchQuery::SetQueryString( const std::string& queryString, std::string& errors )
{
    // Set the QueryString
    m_QueryString = queryString;
    m_SQLQueryString.clear();

    if ( !ParseQueryString( m_QueryString, errors, this ) )
    {
        Log::Warning( "Errors occurred while parsing the query string: %s\n  %s\n", m_QueryString.c_str(), errors.c_str() );
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////
const std::string& VaultSearchQuery::GetSQLQueryString() const
{
    if ( m_SQLQueryString.empty() )
    {
        m_SQLQueryString = m_QueryString;
        m_SQLQueryString = '*' + m_SQLQueryString + '*';

        const std::regex sqlQueryString( "\\*+" );
        m_SQLQueryString = std::regex_replace( m_SQLQueryString, sqlQueryString, std::string( "%" ) ); 
    }
    return m_SQLQueryString;
}

/////////////////////////////////////////////////////////////////////////////////
bool VaultSearchQuery::operator<( const VaultSearchQuery& rhs ) const
{
    return CaseInsensitiveCompareString( GetQueryString().c_str(), rhs.GetQueryString().c_str() ) < 0;
}

/////////////////////////////////////////////////////////////////////////////////
bool VaultSearchQuery::operator==( const VaultSearchQuery& rhs ) const
{
    return CaseInsensitiveCompareString( GetQueryString().c_str(), rhs.GetQueryString().c_str() ) == 0;
}

/////////////////////////////////////////////////////////////////////////////////
bool VaultSearchQuery::operator!=( const VaultSearchQuery& rhs ) const
{
    return CaseInsensitiveCompareString( GetQueryString().c_str(), rhs.GetQueryString().c_str() ) != 0;
}

///////////////////////////////////////////////////////////////////////////////
// The query->m_Search is filled out when provided.
//
bool TokenizeQuery( const std::string& queryString, std::vector< std::string >& tokens )
{
    const std::regex parseTokens( s_TokenizeQueryString, std::regex::icase );

    // parse once to tokenize then match again
    std::sregex_iterator parseItr( queryString.begin(), queryString.end(), parseTokens );
    std::sregex_iterator parseEnd;

    std::string curToken;
    for ( ; parseItr != parseEnd; ++parseItr )
    {
        const std::match_results<std::string::const_iterator>& tokenizeResults = *parseItr;
        curToken = tokenizeResults[1].matched ? Helium::MatchResultAsString( tokenizeResults, 1 ) : "";
        if ( !curToken.empty() )
        {
            tokens.push_back( curToken );
        }
    }

    return !tokens.empty();
}

bool ParsePhrase( const std::string& token, std::smatch& matchResults, std::string& phrase, std::string& errors )
{
    const std::regex parsePhrase( s_ParsePhrase, std::regex::icase );
    const std::regex parseWord( s_ParseWord, std::regex::icase );

    // Phrase or Word
    if ( std::regex_search( token, matchResults, parsePhrase ) 
        || std::regex_search( token, matchResults, parseWord ) )
    {
        if ( matchResults[1].matched )
        {
            phrase = Helium::MatchResultAsString( matchResults, 1 );
            return true;
        }
    }

    errors = "Vault could not parse search query phrase: " + token;
    return false;
}

bool VaultSearchQuery::ParseQueryString( const std::string& queryString, std::string& errors, VaultSearchQuery* query )
{
    std::smatch matchResult;
    const std::regex parseColumnQuery( s_ParseColumnName, std::regex::icase );

    // parse once to tokenize then match again
    std::vector< std::string > tokens;
    if ( TokenizeQuery( queryString, tokens ) )
    {
        std::string curToken;
        std::string currentValue;

        std::smatch matchResults;
        std::vector< std::string >::const_iterator tokenItr = tokens.begin(), tokenEnd = tokens.end();
        for ( ; tokenItr != tokenEnd; ++tokenItr )
        {
            curToken = *tokenItr;

            //-------------------------------------------
            // Token Query
            if ( std::regex_search( curToken, matchResults, parseColumnQuery ) && matchResults[1].matched )
            {
                std::string columnAlias =  Helium::MatchResultAsString( matchResults, 1 );

                ++tokenItr;
                if ( tokenItr == tokenEnd )
                {
                    errors = "More information needed for search query \"" + columnAlias + ":\", missing argument.";
                    return false;
                }
                curToken = *tokenItr;
            }

            //-------------------------------------------
            // Phrase or Word
            if ( ParsePhrase( curToken, matchResults, currentValue, errors )  )
            {
                HELIUM_ASSERT( !currentValue.empty() );

                continue;
            }
            else
            {
                return false;
            }

        }

        return true;
    }

    return false;
}
