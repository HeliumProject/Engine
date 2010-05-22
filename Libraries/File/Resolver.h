#pragma once

#include "API.h"
#include "Exceptions.h"
#include "Reference.h"

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"

#include "SQL/SQLiteDB.h"

namespace File
{ 

  /////////////////////////////////////////////////////////////////////////////
  // This class manages teh DB for teh file resolver only
  class FILE_API Resolver : public SQL::SQLiteDB
  {
  public:

    Resolver( const std::string& databaseFile, const std::string& configPath );
    virtual ~Resolver();

    bool Find  ( File::Reference& file );
    bool Find  ( const std::string& query, File::S_Reference& files );
    bool Find  ( const std::string& query, File::V_Reference& files );
    void Insert( const File::Reference& file );
    void Update( const File::Reference& file );
    void Delete( const File::Reference& file );

    static const char* GetVersion()
    {
      return "1.0";
    }

  protected:

    //
    // SQLiteDB overrides functions
    //

    virtual void PrepareStatements() NOC_OVERRIDE;
    static  void CleanExpressionForSQL( std::string& argument );

  private:

    //
    // Members
    //

    SQL::StmtHandle   m_SqlInsertStatementHandle;
    SQL::StmtHandle   m_SqlUpdateStatementHandle;
    SQL::StmtHandle   m_SqlSelectIdStatementHandle;
    SQL::StmtHandle   m_SqlSelectPathStatementHandle;
    SQL::StmtHandle   m_SqlDeleteStatementHandle;
  };

  typedef FILE_API Nocturnal::SmartPtr< Resolver > ResolverPtr;

} // namespace File