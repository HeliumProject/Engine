#pragma once

#include "API.h"
#include "Exceptions.h"
#include "History.h"
#include "ManagedFile.h"
#include "PatchOperations.h"

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"

#include "SQL/SQLiteDB.h"

namespace File
{ 

  /////////////////////////////////////////////////////////////////////////////
  // This class manages teh DB for teh file resolver only
  class FILE_API CacheDB : public SQL::SQLiteDB
  {
  public:
    CacheDB( const char* friendlyName = "Global-FileResolverCacheDB" );
    virtual ~CacheDB();

    tuid InsertFile( const tuid id, const u64 creationTime, const u64 modifiedTime, const std::string& filePath, const u32 userID );
    tuid UpdateFile( const tuid id, const u64 modifiedTime, const std::string& filePath, const u32 userID );
    void MarkFileDeleted( const tuid id, const u64 modifiedTime, const u32 userID );
    bool RestoreFile( const tuid id, const u64 modifiedTime, const u32 userID );

    ManagedFilePtr SelectFileByID( const tuid id, bool getDeletedFiles = false );
    ManagedFilePtr SelectFileByPath( const std::string& filePath, bool getDeletedFiles = false );
    bool SelectListOfFilesByPath( const std::string& filePath, V_ManagedFilePtr &listOfFiles, bool getOneRow = false, bool getDeletedFiles = false );

    bool Contains( const tuid id, const bool getDeletedFiles = false );

    void InsertFileHistory( const tuid fileID, const PatchOperation operation, const u64 modifiedTime, const u32 userID, const std::string& data = std::string( "" ) );
    void SelectFileHistory( const ManagedFilePtr& file, S_FileHistory& history );
    void SelectFilesByHistory( const std::string& searchQuery, const std::string& modifiedBy, V_ManagedFilePtr &listOfFiles, const std::string& operation = "%", bool getDeletedFiles = false, bool searchHistoryData = false );

    u32  SelectUsernameID( const std::string& username );
    void SelectUsernameByID( const u32 userID, std::string& username );

    void InsertHandledEvent( const tuid id, const u64 created, const std::string& username, const std::string& data );
    void SelectHandledEvents( S_tuid& eventIDs );
    void DeleteHandledEvents();

    static void CleanExpressionForSQL( std::string& argument );

    friend class Resolver;

  protected:

    //
    // SQLiteDB overrides functions
    //

    virtual void PrepareStatements() NOC_OVERRIDE;
    virtual void Delete();

  public:
    static const char* s_FileResolverDBVersion;

  private:

    //
    // Members
    //

    bool              m_NeedsToFlushEvents;

    SQL::StmtHandle   m_SqlSelectIdStatementHandle;
    SQL::StmtHandle   m_SqlSelectPathStatementHandle;
    SQL::StmtHandle   m_SqlSelectOnePathStatementHandle;
    SQL::StmtHandle   m_SqlContainsIdStatementHandle;
    SQL::StmtHandle   m_SqlObliterateStatementHandle;
    SQL::StmtHandle   m_SqlMarkDeletedStatementHandle;
    SQL::StmtHandle   m_SqlRestoreStatementHandle;

    SQL::StmtHandle   m_SqlLookupFileByIDStatementHandle;

    SQL::StmtHandle   m_SqlSelectFileHistoryHandle;
    SQL::StmtHandle   m_SqlSelectFileByHistoryHandle;
    SQL::StmtHandle   m_SqlSelectFileByHistoryDataHandle;

    SQL::StmtHandle   m_SqlSelectHandledEventsHandle;

    SQL::StmtHandle   m_SqlSelectUsernameIDHandle;
    SQL::StmtHandle   m_SqlSelectUsernameByIDHandle;
    
    ManagedFilePtr StepSelectFile( int sqlResult, const SQL::StmtHandle stmt, bool resetStmt = true );

    u64 SelectIDByName( SQL::StmtHandle select, const char* value, const char* insert = NULL );

    void ObliterateFile( const tuid id, const u64 modifiedTime, const u32 userID );

  public:
    void DumpDBData( const std::string& outputFile );

  };

  typedef FILE_API Nocturnal::SmartPtr< CacheDB > CacheDBPtr;

} // namespace File