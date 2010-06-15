#include "Precompile.h"
#include "SearchQuery.h"

#include "Browser.h"

#include "Asset/CacheDB.h"
#include "Foundation/Boost/Regex.h"
#include "Foundation/Container/Insert.h"
#include "Foundation/Checksum/MD5.h"
#include "Foundation/String/Tokenize.h"
#include "Foundation/String/Utilities.h"
#include "Foundation/Log.h"

using namespace Luna;

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
//    AttributeName,
//  };
//}

#define MATCH_WORD         "[a-z0-9_\\-\\.\\\\/:\\*]+"
#define MATCH_PHRASE       "[a-z0-9_\\-\\.\\\\/:\\s\\*]+"
#define MATCH_COLUMN_NAME  "[a-z][a-z0-9_\\-]{1,}"
#define MATCH_COLLECTION_NAME  "[a-z0-9]{1}[\\w\\-\\(\\. ]{1,24}"

const char* s_ParseWord             = "("MATCH_WORD")";
const char* s_ParsePhrase           = "[\"]("MATCH_PHRASE")[\"]";
const char* s_ParseColumnName       = "("MATCH_COLUMN_NAME")\\s*[:=]\\s*";
const char* s_ParseCollectionName       = "("MATCH_COLLECTION_NAME")";
const char* s_TokenizeQueryString   = "("MATCH_COLUMN_NAME"\\s*[:=]\\s*|[\"]"MATCH_PHRASE"[\"]|"MATCH_WORD")";

//const char* s_MatchAssetPathPattern = "^[a-zA-Z]\\:(/[a-zA-Z0-9]([\\w\\-\\. ]*?[a-zA-Z0-9])*){1,}[/]{0,1}$";
const char* s_MatchAssetPathPattern = "^[a-z]\\:(?:[\\\\/]+[a-z0-9_\\-\\. ]+)*[\\\\/]*$";
const char* s_MatchTUIDPattern = "^((?:0[x]){0,1}[a-f0-9]{16}|(?:[\\-]){0,1}[0-9]{16,})$$";
const char* s_MatchDecimalTUIDPattern = "^((?:[\\-]){0,1}[0-9]{16,})$"; // this is also icky, but it might actually be a decimal TUID


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
, m_Search( NULL )
{

}

SearchQuery::~SearchQuery()
{
    m_Search = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void SearchQuery::PostDeserialize()
{
    __super::PostDeserialize();

    std::string errors;
    if ( !ParseQueryString( m_QueryString, errors, this ) )
    {
        Log::Warning( "Errors occurred while parsing the query string: %s\n  %s\n", m_QueryString.c_str(), errors.c_str() );
        return;
    }
}

///////////////////////////////////////////////////////////////////////////////
void SearchQuery::SetQueryString( const std::string& queryString )
{
    // Set the QueryString
    m_QueryString = queryString;

    m_Search = NULL;

    std::string errors;
    if ( !ParseQueryString( m_QueryString, errors, this ) )
    {
        Log::Warning( "Errors occurred while parsing the query string: %s\n  %s\n", m_QueryString.c_str(), errors.c_str() );
        return;
    }
}

///////////////////////////////////////////////////////////////////////////////
void SearchQuery::SetSelectPath( const std::string& selectPath )
{
    m_SelectPath = selectPath;
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
        m_QueryString = "collection: \"" + collection->GetName() + "\"";
    }
}

/////////////////////////////////////////////////////////////////////////////////
AssetCollection* SearchQuery::GetCollection()
{
    //return GlobalBrowser().GetBrowserPreferences()->GetCollectionManager()->FindCollection( m_CollectionPath->GetPath() );
#pragma TODO( "collections are being replaced... ?" )
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////////
bool SearchQuery::operator<( const SearchQuery& rhs ) const
{
    return stricmp( GetQueryString().c_str(), rhs.GetQueryString().c_str() ) < 0;
}

/////////////////////////////////////////////////////////////////////////////////
bool SearchQuery::operator==( const SearchQuery& rhs ) const
{
    return stricmp( GetQueryString().c_str(), rhs.GetQueryString().c_str() ) == 0;
}

/////////////////////////////////////////////////////////////////////////////////
bool SearchQuery::operator!=( const SearchQuery& rhs ) const
{
    return stricmp( GetQueryString().c_str(), rhs.GetQueryString().c_str() ) != 0;
}

///////////////////////////////////////////////////////////////////////////////
// The query->m_Search is filled out when provided.
//
bool TokenizeQuery( const std::string& queryString, V_string& tokens )
{
    const boost::regex parseTokens( s_TokenizeQueryString, boost::regex::icase );

    // parse once to tokenize then match again
    boost::sregex_iterator parseItr( queryString.begin(), queryString.end(), parseTokens );
    boost::sregex_iterator parseEnd;

    std::string curToken;
    for ( ; parseItr != parseEnd; ++parseItr )
    {
        const boost::match_results<std::string::const_iterator>& tokenizeResults = *parseItr;
        curToken = tokenizeResults[1].matched ? ResultAsString( tokenizeResults, 1 ) : "";
        if ( !curToken.empty() )
        {
            tokens.push_back( curToken );
        }
    }

    return !tokens.empty();
}

bool ParseColumnID( const std::string& token, Asset::S_CacheDBColumnID& columnIDs )
{
    bool result = false;

    std::string cleanToken( token );
    toLower( cleanToken );

    // levels and shaders are special cases
    if ( cleanToken == "level" || cleanToken == "levels" )
    {
        columnIDs.insert( Asset::CacheDBColumnIDs::Level );
        result = true;
    }
    else if ( cleanToken == "shader" || cleanToken == "shaders" )
    {
        columnIDs.insert( Asset::CacheDBColumnIDs::Shader );
        result = true;
    }
    else
    {
        std::string phraseWhere;
        int count = 0;
        //Asset::M_CacheDBColumns::const_iterator colItr = GlobalBrowser().GetCacheDB()->GetDBColumns().begin();
        //Asset::M_CacheDBColumns::const_iterator colEnd = GlobalBrowser().GetCacheDB()->GetDBColumns().end();
        //for ( ; colItr != colEnd; ++colItr )
        //{
        //    Asset::CacheDBColumn* cacheDBColumn = ( *colItr ).second;

        //    if ( cacheDBColumn->m_ColumnAliases.find( cleanToken ) != cacheDBColumn->m_ColumnAliases.end() )
        //    {
        //        columnIDs.insert( ( *colItr ).first );
        //        result = true;
        //    }
        //}
    }

    return result;
}

bool ParseAttributeRowID( const std::string& token, u64& rowID )
{
    bool result = false;

#pragma TODO( "reimplemnent without GlobalBrowser" )
    //rowID = GlobalBrowser().GetCacheDB()->FindAttributeRowID( token );
    //if ( rowID > 0 )
    //{
    //    result = true;
    //}

    return result;
}

bool ParseCollectionName( const std::string& token, boost::smatch& matchResults, std::string& collectionName, std::string& errors )
{
    const boost::regex parseCollectionName( s_ParseCollectionName, boost::regex::icase );

    // Phrase or Word
    if ( boost::regex_search( token, matchResults, parseCollectionName ) )
    {
        if ( matchResults[1].matched )
        {
            collectionName = ResultAsString( matchResults, 1 );
            return true;
        }
    }

    errors = "Browser could not parse collection name: " + token;
    return false;
}

bool ParsePhrase( const std::string& token, boost::smatch& matchResults, std::string& phrase, std::string& errors )
{
    const boost::regex parsePhrase( s_ParsePhrase, boost::regex::icase );
    const boost::regex parseWord( s_ParseWord, boost::regex::icase );

    // Phrase or Word
    if ( boost::regex_search( token, matchResults, parsePhrase ) 
        || boost::regex_search( token, matchResults, parseWord ) )
    {
        if ( matchResults[1].matched )
        {
            phrase = ResultAsString( matchResults, 1 );
            return true;
        }
    }

    errors = "Browser could not parse search query phrase: " + token;
    return false;
}

bool SearchQuery::ParseQueryString( const std::string& queryString, std::string& errors, SearchQuery* query )
{
#pragma TODO( "Rachel: Need more error checking in SearchQuery::ParseQueryString" )

    const boost::regex matchAssetPath( s_MatchAssetPathPattern, boost::regex::icase ); 
    const boost::regex matchTUID( s_MatchTUIDPattern, boost::regex::icase );


    boost::smatch matchResult;
    //-------------------------------------------
    // Is an AssetFile/Folder path?
    if ( boost::regex_match( queryString, matchResult, matchAssetPath ) )
    {
        // we know it's a path, clean it
        Nocturnal::Path path( queryString );
        if ( path.IsDirectory() )
        {
            if ( query )
            {
                query->m_SearchType = SearchTypes::Folder;

                Nocturnal::Path::Normalize( query->m_QueryString );
                Nocturnal::Path::GuaranteeSlash( query->m_QueryString );
            }
            return true;
        }
        else if ( path.IsFile() )
        {
            // it's an AssetFile
            if ( query )
            {
                query->m_SearchType = SearchTypes::File;

                Nocturnal::Path::Normalize( query->m_QueryString );
                query->m_SelectPath = query->m_QueryString;
            }
            return true;
        }
        else
        {
            errors = "Invalid or partial path, or file/folder does not exist!";
            return false;
        }
    }

    //-------------------------------------------
    // Otherwise we assume it's an CacheDB Query.
    else
    {
        const boost::regex parseColumnQuery( s_ParseColumnName, boost::regex::icase );

        // parse once to tokenize then match again
        V_string tokens;
        if ( TokenizeQuery( queryString, tokens ) )
        {
            std::string curToken;
            std::string currentValue;

            boost::smatch matchResults;
            V_string::const_iterator tokenItr = tokens.begin(), tokenEnd = tokens.end();
            for ( ; tokenItr != tokenEnd; ++tokenItr )
            {
                curToken = *tokenItr;

                //-------------------------------------------
                // Token Query
                if ( boost::regex_search( curToken, matchResults, parseColumnQuery ) && matchResults[1].matched )
                {
                    std::string columnAlias =  ResultAsString( matchResults, 1 );

                    ++tokenItr;
                    if ( tokenItr == tokenEnd )
                    {
                        errors = "More information needed for search query \"" + columnAlias + ":\", missing argument.";
                        return false;
                    }
                    curToken = *tokenItr;

                    //-------------------------------------------
                    // Collection Query
                    if ( stricmp( columnAlias.c_str(), "collection" ) == 0 )
                    {
                        if ( ParseCollectionName( curToken, matchResults, currentValue, errors ) )
                        {
#pragma TODO( "reimplemnent without GlobalBrowser" )
                            //AssetCollection* collection = GlobalBrowser().GetBrowserPreferences()->GetCollectionManager()->FindCollection( currentValue );
                            //if ( !collection )
                            //{
                            //    // TODO: error out
                            //    errors = "Could not find collection named: " + currentValue;
                            //    return false;
                            //}
                            //else if ( collection && query )
                            //{
                            //    query->SetCollection( collection );
                            //}
                            //continue;
                            return false;
                        }
                        else
                        {
                            return false;
                        }
                    }

                    //-------------------------------------------
                    // Column Query
                    Asset::S_CacheDBColumnID columnIDs;
                    if ( ParseColumnID( columnAlias, columnIDs ) )
                    {
                        if ( !ParsePhrase( curToken, matchResults, currentValue, errors )  )
                        {
                            // a phrase is expected
                            return false;
                        }

                        if ( query )
                        {
                            if ( !query->m_Search )
                            {
                                query->m_Search = new Asset::CacheDBQuery();
                            }

                            if ( columnIDs.size() == 1 )
                            {
                                Asset::CacheDBColumnExpr* columnExpr = dynamic_cast<Asset::CacheDBColumnExpr*>( query->m_Search->AddExprs( new Asset::CacheDBColumnExpr() ) );
                                columnExpr->SetColumnID( *columnIDs.begin() );
                                columnExpr->SetPhrase( currentValue );
                            }
                            else if ( columnIDs.size() > 1 )
                            {
                                Asset::CacheDBQueryPtr subQuery = new Asset::CacheDBQuery();
                                subQuery->SetBinaryOperator( SQL::BinaryOperators::Or );

                                Asset::S_CacheDBColumnID::const_iterator itr = columnIDs.begin(), end = columnIDs.end(); 
                                for ( ; itr != end; ++itr )
                                {
                                    Asset::CacheDBColumnExpr* columnExpr = dynamic_cast<Asset::CacheDBColumnExpr*>( subQuery->AddExprs( new Asset::CacheDBColumnExpr() ) );
                                    columnExpr->SetColumnID( *itr );
                                    columnExpr->SetPhrase( currentValue );
                                }

                                query->m_Search->AddExprs( subQuery );
                            }
                        }

                        continue;
                    }

                    //-------------------------------------------
                    // Attribute Query
                    u64 attrRowID;
                    if ( ParseAttributeRowID( columnAlias, attrRowID ) )
                    {
                        if ( !ParsePhrase( curToken, matchResults, currentValue, errors )  )
                        {
                            // a phrase is expected
                            return false;
                        }

                        if ( query )
                        {
                            if ( !query->m_Search )
                            {
                                query->m_Search = new Asset::CacheDBQuery();
                            }

                            Asset::CacheDBAttributeExpr* attrExpr = dynamic_cast<Asset::CacheDBAttributeExpr*>( query->m_Search->AddExprs( new Asset::CacheDBAttributeExpr() ) );
                            attrExpr->SetAttributeRowID( attrRowID );
                            attrExpr->SetPhrase( currentValue );
                        }

                        continue;
                    }
                }

                //-------------------------------------------
                // Phrase or Word
                if ( ParsePhrase( curToken, matchResults, currentValue, errors )  )
                {
                    NOC_ASSERT( !currentValue.empty() );

                    if ( query )
                    {
                        if ( !query->m_Search )
                        {
                            query->m_Search = new Asset::CacheDBQuery();
                        }

                        Asset::CacheDBPhraseExpr* attrExpr = dynamic_cast<Asset::CacheDBPhraseExpr*>( query->m_Search->AddExprs( new Asset::CacheDBPhraseExpr() ) );
                        attrExpr->SetPhrase( currentValue );
                    }

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
