#include "Windows/Windows.h"

#include "CacheDB.h"
#include "CacheDBQuery.h"

#include "Common/Boost/Regex.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"

using namespace Asset;

// max storage size for a query string
#define MAX_QUERY_LENGTH  2048
#define MAX_INSERT_LENGTH 2048

static const char* s_AttributeExpr = 
"( assets.id IN " \
"\n  ( SELECT asset_x_attribute.asset_id" \
"\n    FROM asset_x_attribute" \
"\n    WHERE asset_x_attribute.attribute_id = %u" \
"\n      AND asset_x_attribute.value LIKE '%%%s%%' ESCAPE '@' ) )";

static const char* s_LevelExpr = 
"( assets.id IN " \
"\n  ( SELECT levels_x_entities.entity_id" \
"\n    FROM levels_x_entities" \
"\n    WHERE levels_x_entities.level_id IN" \
"\n    ( SELECT assets.id" \
"\n      FROM assets" \
"\n      LEFT JOIN engine_types ON engine_types.id = assets.engine_type_id" \
"\n      WHERE ( assets.name LIKE '%%%s%%' ESCAPE '@' ) AND ( engine_types.name='Level' ) ) ) )";

static const char* s_ShaderExpr = 
"( assets.id IN " \
"\n  ( SELECT entities_x_shaders.entity_id" \
"\n    FROM entities_x_shaders" \
"\n    WHERE entities_x_shaders.shader_id IN" \
"\n    ( SELECT assets.id" \
"\n      FROM assets" \
"\n      LEFT JOIN engine_types ON engine_types.id = assets.engine_type_id" \
"\n      WHERE ( assets.name LIKE '%%%s%%' ESCAPE '@' ) AND ( engine_types.name='Shader' ) ) ) )";


/////////////////////////////////////////////////////////////////////////////
/// class CacheDBExpr
/////////////////////////////////////////////////////////////////////////////
CacheDBExpr::CacheDBExpr()
: m_NegateExpr( false )
, m_BinaryOperator( SQL::BinaryOperators::Like )
{
}

CacheDBExpr::~CacheDBExpr()
{
}


/////////////////////////////////////////////////////////////////////////////
/// class CacheDBPhraseExpr
/////////////////////////////////////////////////////////////////////////////
CacheDBPhraseExpr::CacheDBPhraseExpr()
: CacheDBExpr()
{
}

CacheDBPhraseExpr::~CacheDBPhraseExpr()
{
}

void CacheDBPhraseExpr::SetPhrase( const std::string& phrase )
{
  m_Phrase = phrase;
  FileSystem::StripPrefix( File::GlobalManager().GetManagedAssetsRoot(), m_Phrase );
  CacheDB::CleanExpressionForSQL( m_Phrase );
}

bool CacheDBPhraseExpr::GetExpression( const M_CacheDBColumns& columns, std::string& expr, S_CacheDBColumnID& tables ) const
{
  ASSETTRACKER_SCOPE_TIMER((""));

  if ( !GetPhrase().empty() )
  {
    int count = 0;
    std::string phraseExpr = "";
    M_CacheDBColumns::const_iterator colItr = columns.begin();
    M_CacheDBColumns::const_iterator colEnd = columns.end();
    for ( ; colItr != colEnd; ++colItr )
    {
      CacheDBColumn* cacheDBColumn = ( *colItr ).second;
      
      if ( cacheDBColumn->UseInGenericSearch() )
      {
        // Add this table to the list we need to join
        tables.insert( ( *colItr ).first );

        phraseExpr += ( count > 0 ) ? "\n  OR " : "";
        cacheDBColumn->GetWhere( GetPhrase(), phraseExpr, SQL::BinaryOperators::String( GetBinaryOperator() ) );
        ++count;
      }
    }

    // add this phrases where expr to the one we are building up
    if ( count > 0 )
    {
      if ( NegateExpr() )
      {
        phraseExpr = "NOT (" + phraseExpr + ")";
      }
      else if ( count > 1 )
      {
        phraseExpr = "(" + phraseExpr + ")";
      }

      expr = phraseExpr;
      return true;
    }
  }

  return false;
}


/////////////////////////////////////////////////////////////////////////////
/// class CacheDBColumnExpr
/////////////////////////////////////////////////////////////////////////////
CacheDBColumnExpr::CacheDBColumnExpr()
: CacheDBPhraseExpr()
{
}

CacheDBColumnExpr::~CacheDBColumnExpr()
{
}

void CacheDBColumnExpr::SetColumnID( const CacheDBColumnID columnID )
{
  m_ColumnID = columnID;
}

bool CacheDBColumnExpr::GetExpression( const M_CacheDBColumns& columns, std::string& expr, S_CacheDBColumnID& tables ) const
{
  ASSETTRACKER_SCOPE_TIMER((""));

  if ( !GetPhrase().empty() )
  {
    // levels and shaders are special cases
    if ( GetColumnID() == CacheDBColumnIDs::Level )
    {
      char buff[MAX_INSERT_LENGTH];
      sprintf_s( buff, sizeof( buff ), s_LevelExpr, GetPhrase().c_str() );
      expr = buff;

      if ( NegateExpr() )
      {
        expr = "NOT " + expr;
      }
    }
    else if ( GetColumnID() == CacheDBColumnIDs::Shader )
    {
      char buff[MAX_INSERT_LENGTH];
      sprintf_s( buff, sizeof( buff ), s_ShaderExpr, GetPhrase().c_str() );
      expr = buff;

      if ( NegateExpr() )
      {
        expr = "NOT " + expr;
      }
    }
    else
    {
      M_CacheDBColumns::const_iterator findColumn = columns.find( GetColumnID() );
      if ( findColumn != columns.end() )
      {
        CacheDBColumn* cacheDBColumn = ( *findColumn ).second;

        // Add this table to the list we need to join
        tables.insert( ( *findColumn ).first );

        expr = "";
        cacheDBColumn->GetWhere( GetPhrase(), expr, SQL::BinaryOperators::String( GetBinaryOperator() ) );

        if ( NegateExpr() )
        {
          expr = "NOT (" + expr + ")";
        }

        return true;
      }
    }
  }

  return false;
}


/////////////////////////////////////////////////////////////////////////////
/// class CacheDBAttributeExpr
/////////////////////////////////////////////////////////////////////////////
CacheDBAttributeExpr::CacheDBAttributeExpr()
: CacheDBPhraseExpr()
{
}

CacheDBAttributeExpr::~CacheDBAttributeExpr()
{
}


bool CacheDBAttributeExpr::GetExpression( const M_CacheDBColumns& columns, std::string& expr, S_CacheDBColumnID& tables ) const
{
  ASSETTRACKER_SCOPE_TIMER((""));

  if ( !GetPhrase().empty() && GetAttributeRowID() > 0 )
  {
    char buff[MAX_INSERT_LENGTH];
    sprintf_s( buff, sizeof( buff ), s_AttributeExpr, GetAttributeRowID(), GetPhrase().c_str() );
    expr = buff;

    if ( NegateExpr() )
    {
      expr = "NOT " + expr;
    }
    return true;
  }

  return false;
}


/////////////////////////////////////////////////////////////////////////////
/// class CacheDBQuery
/////////////////////////////////////////////////////////////////////////////
CacheDBQuery::CacheDBQuery()
: CacheDBExpr()
{
  m_BinaryOperator = SQL::BinaryOperators::And;
}

CacheDBQuery::~CacheDBQuery()
{
}

CacheDBExpr* CacheDBQuery::AddExprs( CacheDBExpr* expr )
{
  m_Exprs.push_back( expr );

  return m_Exprs.back();
}

bool CacheDBQuery::GetExpression( const M_CacheDBColumns& columns, std::string& expr, S_CacheDBColumnID& tables ) const
{
  ASSETTRACKER_SCOPE_TIMER((""));

  if ( !GetExprs().empty() )
  {
    int count = 0;
    std::string queryExpr = "";
    std::string childExpr;
    for ( V_CacheDBExpr::const_iterator itr = GetExprs().begin(), end = GetExprs().end(); itr != end; ++itr )
    {
      if ( (*itr)->GetExpression( columns, childExpr, tables ) )
      {
        queryExpr += ( count > 0 ) ? "\n  " + SQL::BinaryOperators::String( GetBinaryOperator() ) + " " : "";
        queryExpr += childExpr;
        ++count;
      }
    }

    // add this phrases where expr to the one we are building up
    if ( count > 0 )
    {
      if ( NegateExpr() )
      {
        queryExpr = "NOT (" + queryExpr + ")";
      }
      else if ( count > 1 )
      {
        queryExpr = "(" + queryExpr + ")";
      }

      expr = queryExpr;
      return true;
    }
  }

  return false;
}