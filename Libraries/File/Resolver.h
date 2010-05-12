#pragma once

#include "API.h"
#include "CacheDB.h" 
#include "ManagedFile.h"
#include "Exceptions.h"

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"
#include "EventSystem/EventSystem.h"

#include "TUID/TUID.h"

namespace File
{
  /////////////////////////////////////////////////////////////////////////////
  // Resolves a TUID with a file path.
  //  This class is used to interface and maintain a local SQLite DB
  //  containing file path and TUID pairs. 
  //

  class Resolver;
  typedef Nocturnal::SmartPtr< Resolver > ResolverPtr;

  class FILE_API Resolver : public Nocturnal::RefCountBase< Resolver >
  {

  public:
    Resolver( );
    void Initialize( const std::string& projectRoot = std::string( "" ), const std::string& projectAssets = std::string( "" ) );

    virtual ~Resolver( );

    void Update();
    void Recreate();

    void SetWorkingChangeset( const RCS::Changeset& changeset );

    // 
    // Transactions
    //

    void BeginTrans( bool manageEventsFile = true );
    void CommitTrans();
    void RollbackTrans();
    bool IsTransOpen() const { return m_IsTransOpen; }

    //
    // API
    //

    tuid AddEntry( const std::string &filePath, const tuid tryId = TUID::Null, bool createEvent = true );
    tuid AddEntry( ManagedFilePtr& file, bool createEvent = true );
    tuid UpdateEntry( const ManagedFilePtr& file, const std::string &newFilePath );
    void DeleteEntry( const ManagedFilePtr& file );
    void DeleteEntry( const tuid id );

    ManagedFilePtr GetFile( const std::string& filePath );
    ManagedFilePtr GetFile( const tuid id );
    tuid GetFileID( const std::string &filePath );
    void GetFilePath( const tuid id, std::string &filePath );
    void GetFileHistory( const ManagedFilePtr& file, S_FileHistory& history );

    bool Contains( const tuid id, const bool getDeletedFiles = false );

    void FindFilesByPath( const std::string& searchQuery, V_ManagedFilePtr& listOfFiles, bool getOneRow = false );
    void FindFilesByHistory( const std::string& searchQuery, const std::string& modifiedBy, V_ManagedFilePtr& listOfFiles, const std::string& operation = std::string( "%" ), bool searchHistoryData = false );

    void DumpEventsToTextFile( const std::string& eventsFile = std::string( "" ), const std::string& txtFile = std::string( "" ), const std::string& filter = std::string( "" ) );
    void LoadEventsFromTextFile( const std::string& txtFile = std::string( "" ), const std::string& eventsFile = std::string( "" ) );

    const std::string& GetComputerName() { return m_ComputerName; }
    const std::string& GetUsername() { return m_Username; }
    std::string        GetUserClientName( bool isFileName = false ); // isFileName makes the path contain '-' rather than '@'

    void GetUsernameByID( const u32 userID, std::string& username );

  protected:
    //
    // Members
    //

    std::string           m_RootDir;
    bool                  m_IsInitialized;

    CacheDBPtr            m_CacheDB;

    ES::EventSystemPtr    m_EventSystem;
    std::string           m_UsersEventsFile;

    std::string           m_UserClientName;
    std::string           m_ComputerName;
    std::string           m_Username;

    RCS::Changeset        m_Changeset;

    // 
    // Transactions
    //

    bool              m_IsTransOpen;

    ES::V_EventPtr    m_TransUserEvents; // Events that will be writen to the user's event file when the Trans is commited

    bool              m_TransManageEventsFile;
    std::string       m_TransChangelistDescription;

    inline void ThrowIfMaxOpenTrans() { if ( IsTransOpen() ) throw MaxOpenTransException(); }
    inline void ThrowIfNoTransOpen() { if ( !IsTransOpen() ) throw NoTransOpenException(); }

    inline void AppendChangelistDescription( const char* patchOperation, const std::string& filePath, const tuid id, const std::string& newFilePath = std::string("") );

    //
    // Patching
    //

    // this structure is currently only used for temporary conversion purposes
    // we store PatchRecords as strings in our cache, see below
    struct PatchRecord
    {
      PatchOperation m_Operation; 
      u64            m_FileCreated; 
      u64            m_FileModified; 
      tuid           m_FileId; 
      std::string    m_FilePath; 
      std::string    m_FileUsername;
      //std::string    m_FileComputer;
    }; 

    void PatchCache();
    bool ApplyPatchRecord( ES::Event* event );
    void CreatePatchRecord( const int operation, const tuid id, const u64 creationTime, const u64 modifiedTime, const std::string& filePath );

    static void ParseEvent( const ES::Event* event, PatchRecord& output ); 
    static void ParseTextFileData( const std::string& patchRecord,  PatchRecord& output ); 
    static void ParseEventData( const std::string& eventData, PatchRecord& output ); 
  };
}
