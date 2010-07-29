#include "Precompile.h"
#include "SearchQuery.h"

#include "App.h"
#include "Vault.h"

#include "Foundation/Regex.h"
#include "Foundation/Container/Insert.h"
#include "Foundation/Checksum/MD5.h"
#include "Foundation/String/Tokenize.h"
#include "Foundation/String/Utilities.h"
#include "Foundation/Log.h"

using namespace Editor;

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
//    Folder,
//    ID,
//    Phrase,
//    ColumnName,
//    ComponentName,
//  };
//}

#define MATCH_WORD             TXT( "[a-z0-9_\\-\\.\\\\/:\\*]+" )
#define MATCH_PHRASE           TXT( "[a-z0-9_\\-\\.\\\\/:\\s\\*]+" )
#define MATCH_COLUMN_NAME      TXT( "[a-z][a-z0-9_\\-]{1,}" )
#define MATCH_COLLECTION_NAME  TXT( "[a-z0-9]{1}[\\w\\-\\(\\. ]{1,24}" )

const tchar* s_ParseWord             = TXT( "(" ) MATCH_WORD TXT( ")" );
const tchar* s_ParsePhrase           = TXT( "[\"](" ) MATCH_PHRASE TXT( ")[\"]" );
const tchar* s_ParseColumnName       = TXT( "(" ) MATCH_COLUMN_NAME TXT( ")\\s*[:=]\\s*" );
const tchar* s_ParseCollectionName   = TXT( "(" ) MATCH_COLLECTION_NAME TXT( ")" );
const tchar* s_TokenizeQueryString   = TXT( "(" ) MATCH_COLUMN_NAME TXT( "\\s*[:=]\\s*|[\"]" ) MATCH_PHRASE TXT( "[\"]|" ) MATCH_WORD TXT( ")" );

//const char* s_MatchAssetPathPattern = "^[a-zA-Z]\\:(/[a-zA-Z0-9]([\\w\\-\\. ]*?[a-zA-Z0-9])*){1,}[/]{0,1}$";
const tchar* s_MatchAssetPathPattern   = TXT( "^[a-z]\\:(?:[\\\\/]+[a-z0-9_\\-\\. ]+)*[\\\\/]*$" );
const tchar* s_MatchTUIDPattern        = TXT( "^((?:0[x]){0,1}[a-f0-9]{16}|(?:[\\-]){0,1}[0-9]{16,})$$" );
const tchar* s_MatchDecimalTUIDPattern = TXT( "^((?:[\\-]){0,1}[0-9]{16,})$" ); // this is also icky, but it might actually be a decimal TUID


///////////////////////////////////////////////////////////////////////////////
REFLECT_DEFINE_CLASS( SearchQuery );
void SearchQuery::EnumerateClass( Reflect::Compositor<SearchQuery>& comp )
{
    Reflect::EnumerationField* enumSearchType = comp.AddEnumerationField( &SearchQuery::m_SearchType, "m_SearchType" );
    Reflect::Field* fieldQueryString = comp.AddField( &SearchQuery::m_QueryString, "m_QueryString" );
    Reflect::Field* fieldQueryPath = comp.AddField( &SearchQuery::m_QueryPath, "m_QueryPath", Reflect::FieldFlags::Force );
    Reflect::Field* fieldCollectionPath = comp.AddField( &SearchQuery::m_CollectionPath, "m_CollectionPath", Reflect::FieldFlags::Force );
}

///////////////////////////////////////////////////////////////////////////////
SearchQuery::SearchQuery()
: m_SearchType( SearchTypes::DBSearch )
{

}

SearchQuery::~SearchQuery()
{
}

///////////////////////////////////////////////////////////////////////////////
void SearchQuery::PostDeserialize()
{
    __super::PostDeserialize();

    tstring errors;
    if ( !ParseQueryString( m_QueryString, errors, this ) )
    {
        Log::Warning( TXT( "Errors occurred while parsing the query string: %s\n  %s\n" ), m_QueryString.c_str(), errors.c_str() );
        return;
    }
}

///////////////////////////////////////////////////////////////////////////////
void SearchQuery::SetQueryString( const tstring& queryString )
{
    // Set the QueryString
    m_QueryString = queryString;

    tstring errors;
    if ( !ParseQueryString( m_QueryString, errors, this ) )
    {
        Log::Warning( TXT( "Errors occurred while parsing the query string: %s\n  %s\n" ), m_QueryString.c_str(), errors.c_str() );
        return;
    }
}

/////////////////////////////////////////////////////////////////////////////////
void SearchQuery::SetCollection( const AssetCollection* collection )
{
    if ( !collection ) 
        return;

    m_SearchType = SearchTypes::DBSearch;

    if ( m_CollectionPath )
    {
        delete m_CollectionPath;
    }

    m_CollectionPath = collection->GetPath();

    if ( m_QueryString.empty() )
    {
        m_QueryString = TXT( "collection: \"" ) + collection->GetName() + TXT( "\"" );
    }
}

/////////////////////////////////////////////////////////////////////////////////
AssetCollection* SearchQuery::GetCollection()
{
    //return wxGetApp().GetVaultPreferences()->GetCollectionManager()->FindCollection( m_CollectionPath->GetPath() );
#pragma TODO( "collections are being replaced... ?" )
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////////
bool SearchQuery::operator<( const SearchQuery& rhs ) const
{
    return _tcsicmp( GetQueryString().c_str(), rhs.GetQueryString().c_str() ) < 0;
}

/////////////////////////////////////////////////////////////////////////////////
bool SearchQuery::operator==( const SearchQuery& rhs ) const
{
    return _tcsicmp( GetQueryString().c_str(), rhs.GetQueryString().c_str() ) == 0;
}

/////////////////////////////////////////////////////////////////////////////////
bool SearchQuery::operator!=( const SearchQuery& rhs ) const
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

bool ParseCollectionName( const tstring& token, tsmatch& matchResults, tstring& collectionName, tstring& errors )
{
    const tregex parseCollectionName( s_ParseCollectionName, std::tr1::regex::icase );

    // Phrase or Word
    if ( std::tr1::regex_search( token, matchResults, parseCollectionName ) )
    {
        if ( matchResults[1].matched )
        {
            collectionName = Helium::MatchResultAsString( matchResults, 1 );
            return true;
        }
    }

    errors = TXT( "Vault could not parse collection name: " ) + token;
    return false;
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

bool SearchQuery::ParseQueryString( const tstring& queryString, tstring& errors, SearchQuery* query )
{
#pragma TODO( "Rachel: Need more error checking in SearchQuery::ParseQueryString" )

    const tregex matchAssetPath( s_MatchAssetPathPattern, std::tr1::regex::icase ); 
    const tregex matchTUID( s_MatchTUIDPattern, std::tr1::regex::icase );


    tsmatch matchResult;
    //-------------------------------------------
    // Is an AssetFile/Folder path?
    if ( std::tr1::regex_match( queryString, matchResult, matchAssetPath ) )
    {
        // we know it's a path, clean it
        Helium::Path path( queryString );
        if ( path.IsDirectory() )
        {
            if ( query )
            {
                query->m_SearchType = SearchTypes::Folder;

                Helium::Path::Normalize( query->m_QueryString );
                Helium::Path::GuaranteeSlash( query->m_QueryString );
            }
            return true;
        }
        else if ( path.IsFile() )
        {
            // it's an AssetFile
            if ( query )
            {
                query->m_SearchType = SearchTypes::File;
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
    // Otherwise we assume it's an CacheDB Query.
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

                    //-------------------------------------------
                    // Collection Query
                    if ( _tcsicmp( columnAlias.c_str(), TXT( "collection" ) ) == 0 )
                    {
                        if ( ParseCollectionName( curToken, matchResults, currentValue, errors ) )
                        {
                            AssetCollection* collection = wxGetApp().GetPreferences()->GetVaultPreferences()->GetCollectionManager()->FindCollection( currentValue );
                            if ( !collection )
                            {
                                // TODO: error out
                                errors = "Could not find collection named: " + currentValue;
                                return false;
                            }
                            else if ( collection && query )
                            {
                                query->SetCollection( collection );
                            }
                            continue;
                        }
                        else
                        {
                            return false;
                        }
                    }
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
