#include "Precompile.h"
#include "VaultSearchQuery.h"

#include "Editor/App.h"
#include "Foundation/Regex.h"
#include "Foundation/Container/Insert.h"
#include "Foundation/Checksum/MD5.h"
#include "Foundation/String/Tokenize.h"
#include "Foundation/String/Utilities.h"
#include "Foundation/Log.h"

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

#define MATCH_WORD             TXT( "[a-z0-9_\\-\\.\\\\/:\\*]+" )
#define MATCH_PHRASE           TXT( "[a-z0-9_\\-\\.\\\\/:\\s\\*]+" )
#define MATCH_COLUMN_NAME      TXT( "[a-z][a-z0-9_\\-]{1,}" )

const tchar_t* s_ParseWord             = TXT( "(" ) MATCH_WORD TXT( ")" );
const tchar_t* s_ParsePhrase           = TXT( "[\"](" ) MATCH_PHRASE TXT( ")[\"]" );
const tchar_t* s_ParseColumnName       = TXT( "(" ) MATCH_COLUMN_NAME TXT( ")\\s*[:=]\\s*" );
const tchar_t* s_TokenizeQueryString   = TXT( "(" ) MATCH_COLUMN_NAME TXT( "\\s*[:=]\\s*|[\"]" ) MATCH_PHRASE TXT( "[\"]|" ) MATCH_WORD TXT( ")" );

//const char* s_MatchAssetPathPattern = "^[a-zA-Z]\\:(/[a-zA-Z0-9]([\\w\\-\\. ]*?[a-zA-Z0-9])*){1,}[/]{0,1}$";
const tchar_t* s_MatchAssetPathPattern   = TXT( "^[a-z]\\:(?:[\\\\/]+[a-z0-9_\\-\\. ]+)*[\\\\/]*$" );
const tchar_t* s_MatchTUIDPattern        = TXT( "^((?:0[x]){0,1}[a-f0-9]{16}|(?:[\\-]){0,1}[0-9]{16,})$$" );
const tchar_t* s_MatchDecimalTUIDPattern = TXT( "^((?:[\\-]){0,1}[0-9]{16,})$" ); // this is also icky, but it might actually be a decimal TUID


///////////////////////////////////////////////////////////////////////////////
REFLECT_DEFINE_ENUMERATION( SearchType );
REFLECT_DEFINE_CLASS( VaultSearchQuery );
void VaultSearchQuery::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.AddEnumerationField( &VaultSearchQuery::m_SearchType, TXT( "m_SearchType" ) );
    comp.AddField( &VaultSearchQuery::m_QueryString, TXT( "m_QueryString" ) );
    comp.AddField( &VaultSearchQuery::m_QueryPath, TXT( "m_QueryPath" ), Reflect::FieldFlags::Force );
}

///////////////////////////////////////////////////////////////////////////////
VaultSearchQuery::VaultSearchQuery()
: m_SearchType( SearchType::CacheDB )
, m_SQLQueryString( TXT("") )
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
        tstring errors;
        if ( !ParseQueryString( m_QueryString, errors, this ) )
        {
            Log::Warning( TXT( "Errors occurred while parsing the query string: %s\n  %s\n" ), m_QueryString.c_str(), errors.c_str() );
            return;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool VaultSearchQuery::SetQueryString( const tstring& queryString, tstring& errors )
{
    // Set the QueryString
    m_QueryString = queryString;
    m_SQLQueryString.clear();

    if ( !ParseQueryString( m_QueryString, errors, this ) )
    {
        Log::Warning( TXT( "Errors occurred while parsing the query string: %s\n  %s\n" ), m_QueryString.c_str(), errors.c_str() );
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////
const tstring& VaultSearchQuery::GetSQLQueryString() const
{
    if ( m_SQLQueryString.empty() )
    {
        m_SQLQueryString = m_QueryString;
        m_SQLQueryString = TXT( '*' ) + m_SQLQueryString + TXT( '*' );

        const tregex sqlQueryString( TXT( "\\*+" ) );
        m_SQLQueryString = std::tr1::regex_replace( m_SQLQueryString, sqlQueryString, tstring( TXT( "%" ) ) ); 
    }
    return m_SQLQueryString;
}

/////////////////////////////////////////////////////////////////////////////////
bool VaultSearchQuery::operator<( const VaultSearchQuery& rhs ) const
{
    return _tcsicmp( GetQueryString().c_str(), rhs.GetQueryString().c_str() ) < 0;
}

/////////////////////////////////////////////////////////////////////////////////
bool VaultSearchQuery::operator==( const VaultSearchQuery& rhs ) const
{
    return _tcsicmp( GetQueryString().c_str(), rhs.GetQueryString().c_str() ) == 0;
}

/////////////////////////////////////////////////////////////////////////////////
bool VaultSearchQuery::operator!=( const VaultSearchQuery& rhs ) const
{
    return _tcsicmp( GetQueryString().c_str(), rhs.GetQueryString().c_str() ) != 0;
}

///////////////////////////////////////////////////////////////////////////////
// The query->m_Search is filled out when provided.
//
bool TokenizeQuery( const tstring& queryString, std::vector< tstring >& tokens )
{
    const tregex parseTokens( s_TokenizeQueryString, std::tr1::regex::icase );

    // parse once to tokenize then match again
    tsregex_iterator parseItr( queryString.begin(), queryString.end(), parseTokens );
    tsregex_iterator parseEnd;

    tstring curToken;
    for ( ; parseItr != parseEnd; ++parseItr )
    {
        const std::tr1::match_results<tstring::const_iterator>& tokenizeResults = *parseItr;
        curToken = tokenizeResults[1].matched ? Helium::MatchResultAsString( tokenizeResults, 1 ) : TXT( "" );
        if ( !curToken.empty() )
        {
            tokens.push_back( curToken );
        }
    }

    return !tokens.empty();
}

bool ParsePhrase( const tstring& token, tsmatch& matchResults, tstring& phrase, tstring& errors )
{
    const tregex parsePhrase( s_ParsePhrase, std::tr1::regex::icase );
    const tregex parseWord( s_ParseWord, std::tr1::regex::icase );

    // Phrase or Word
    if ( std::tr1::regex_search( token, matchResults, parsePhrase ) 
        || std::tr1::regex_search( token, matchResults, parseWord ) )
    {
        if ( matchResults[1].matched )
        {
            phrase = Helium::MatchResultAsString( matchResults, 1 );
            return true;
        }
    }

    errors = TXT( "Vault could not parse search query phrase: " ) + token;
    return false;
}

bool VaultSearchQuery::ParseQueryString( const tstring& queryString, tstring& errors, VaultSearchQuery* query )
{
    const tregex matchAssetPath( s_MatchAssetPathPattern, std::tr1::regex::icase ); 
    const tregex matchTUID( s_MatchTUIDPattern, std::tr1::regex::icase );


    tsmatch matchResult;
    //-------------------------------------------
    // Path
    if ( std::tr1::regex_match( queryString, matchResult, matchAssetPath ) )
    {
        // we know it's a path, clean it
        Helium::Path path( queryString );
        if ( path.IsDirectory() )
        {
            if ( query )
            {
                query->m_SearchType = SearchType::Directory;

                Helium::Path::GuaranteeSeparator( query->m_QueryString );
            }
            return true;
        }
        else if ( path.IsFile() )
        {
            if ( query )
            {
                query->m_SearchType = SearchType::File;
            }
            return true;
        }
        else
        {
            errors = TXT( "Invalid or partial path, or file/folder does not exist!" );
            return false;
        }
    }

    //-------------------------------------------
    // CacheDB.
    else
    {
        const tregex parseColumnQuery( s_ParseColumnName, std::tr1::regex::icase );

        // parse once to tokenize then match again
        std::vector< tstring > tokens;
        if ( TokenizeQuery( queryString, tokens ) )
        {
            tstring curToken;
            tstring currentValue;

            tsmatch matchResults;
            std::vector< tstring >::const_iterator tokenItr = tokens.begin(), tokenEnd = tokens.end();
            for ( ; tokenItr != tokenEnd; ++tokenItr )
            {
                curToken = *tokenItr;

                //-------------------------------------------
                // Token Query
                if ( std::tr1::regex_search( curToken, matchResults, parseColumnQuery ) && matchResults[1].matched )
                {
                    tstring columnAlias =  Helium::MatchResultAsString( matchResults, 1 );

                    ++tokenItr;
                    if ( tokenItr == tokenEnd )
                    {
                        errors = TXT( "More information needed for search query \"" ) + columnAlias + TXT( ":\", missing argument." );
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
        }

        return true;
    }

    return false;
}
