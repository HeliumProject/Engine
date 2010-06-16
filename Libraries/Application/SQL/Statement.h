#pragma once

#include "Application/API.h"

#include <string>

namespace SQL
{  
  // The Statement is used to manage statement handles
  class Statement
  {
  public:
    std::string   m_SQL;         // String SQL statement
    std::string   m_BindFormat;  // Format of the statements arguments
    bool          m_IsPrepared;
    bool          m_IsBound;

  public:
    Statement()
      : m_SQL( "" )
      , m_BindFormat( "" )
      , m_IsPrepared( false )
      , m_IsBound( false )
    {
    
    }

    Statement( const std::string& sql, const std::string& bindFormat = std::string( "" ) )
      : m_SQL( sql )
      , m_BindFormat( bindFormat )
      , m_IsPrepared( false )
      , m_IsBound( bindFormat.empty() )
    {
    
    }

    virtual ~Statement()
    {

    }
    
    virtual bool IsPrepared() = 0;
  };
}