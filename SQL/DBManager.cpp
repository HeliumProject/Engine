#include "stdafx.h"
#include "DBManager.h"
#include "Exceptions.h"

using namespace SQL;

//////////////////////////////////////////////////////////////////////////////

//void DBManager::ConvertAsteriskToPercent( std::string& argument )
//{
//  // Converts all wildcard asterisks '*' into percent symbols '%' for SQL LIKE expression
//  const boost::regex asterisk("[*]+"); 
//  argument = boost::regex_replace(argument, asterisk, "%"); 
//}
//
//void DBManager::CleanExpressionForSQL( std::string& argument )
//{ 
//  // convert asterisks to percent 
//  const boost::regex asterisk("[*]+"); 
//  argument = boost::regex_replace(argument, asterisk, "%"); 
//
//  // escape underscores
//  const boost::regex underscore("[_]"); 
//  argument = boost::regex_replace(argument, underscore, "@_"); 
//}

void DBManager::ThrowIfDBConnected( const char* function )
{
  if( IsConnected() ) throw SQL::DBManagerException( this, function, "A DB connection is already open!" );
}

void DBManager::ThrowIfDBNotConnected( const char* function )
{
  if( !IsConnected() ) throw SQL::DBManagerException( this, function, "There is no open DB connection!" );
}

void DBManager::ThrowIfMaxOpenTrans( const char* function )
{
  if ( m_IsTransOpen ) throw SQL::DBManagerException( this, function, "Maximum number of transactions exceeded." );
}

void DBManager::ThrowIfNoTransOpen( const char* function )
{
  if ( !m_IsTransOpen ) throw SQL::DBManagerException( this, function, "There is no open transaction!" );
}

