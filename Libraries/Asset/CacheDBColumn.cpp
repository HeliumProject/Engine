#include "Windows/Windows.h"

#include "CacheDBColumn.h"
#include "CacheDB.h"

#include "Common/String/Tokenize.h"

using namespace Asset;

CacheDBColumn::CacheDBColumn
(
 Asset::CacheDB* assetCacheDB,
 const std::string& name,
 const std::string& table,
 const std::string& column,
 const std::string& columnAliases
 )
 : m_AssetCacheDB( assetCacheDB )
 , m_Name( name )
 , m_Table( table )
 , m_Column( column )
 , m_PopulateStmtHandle( SQL::NullStatement )
 , m_UseInGenericSearch( true )
{
  NOC_ASSERT( m_AssetCacheDB );

  m_TableColumn = m_Table+"."+m_Column;

  m_SelectExpr = m_TableColumn+" AS "+m_Name;

  if ( !columnAliases.empty() )
  {
    Tokenize( columnAliases, m_ColumnAliases, "," );
  }
}

void CacheDBColumn::SetJoin( const std::string& primaryKey, const std::string& foreignTable, const std::string& foreignKey )
{
  NOC_ASSERT( !primaryKey.empty() && !foreignTable.empty() && !foreignKey.empty() );

  m_PrimaryKey = primaryKey;
  m_ForeignTable = foreignTable;
  m_ForeignKey = foreignKey;

  m_JoinExpr = "LEFT JOIN "+m_Table+" ON "+m_Table+"."+m_PrimaryKey+" = "+m_ForeignTable+"."+m_ForeignKey+"\n";
}

void CacheDBColumn::GetWhere( const std::string& phrase, std::string& whereExpr, const std::string& op, bool escape ) const
{
  if ( escape )
  {
    whereExpr += "( " + m_TableColumn + " " + op + " '" + phrase + "' ESCAPE '@' )";
  }
  else
  {
    whereExpr += "( " + m_TableColumn + " " + op + " '" + phrase + "' )";
  }
}

void CacheDBColumn::SetPopulateSQL( const std::string& sql )
{
  m_PopulateSQL = sql;
}

void CacheDBColumn::SetPopulateStmtHandle( SQL::StmtHandle stmtHandle )
{
  m_PopulateStmtHandle = stmtHandle;
}
