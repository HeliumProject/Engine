#pragma once

#include "API.h"
#include "Exceptions.h"
#include "History.h"
#include "ManagedFile.h"

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"


namespace File
{
  //
  // Top level pervasive command line args
  //
  struct FILE_API Args
  {
    static const char* NoAutoSubmit;
    static const char* FixupTUIDs;
  };

  // FIXME: since the File::Manager will now need to support multiple Managers,
  // it would be wise to move the GlobalManager somewhere project specific

  // Returns a reference to the static global Manager theGlobalManager.
  FILE_API class Manager& GlobalManager();

  // Swap out the global manger with this file Manager
  FILE_API void SwapGlobalManager( Manager * fileManager = NULL);

  void FILE_API Initialize();
  void FILE_API Cleanup();

  /////////////////////////////////////////////////////////////////////////////


  FILE_API typedef u32 ManagerConfig;
  namespace ManagerConfigs
  {
    enum ManagerConfig
    {
      Resolver        =   1 << 0,  // manage the tuid backed file in File::Resolver
      RCS             =   1 << 1,  // version control the file in RCS
      Disc            =   1 << 2,  // manipulate the file on disc

      FailIfExists    =   1 << 3,  // fail if the target file already exists
      Preview         =   1 << 4,  // print out the operations but do nothing
    };

    static const u32 Default = ( Resolver | RCS | Disc | FailIfExists );

    FILE_API std::string GetManagerConfigString( const u32 managerConfig );
  }

  FILE_API inline bool HasResolver( ManagerConfig managerConfig ) { return ( ( managerConfig & ManagerConfigs::Resolver ) == ManagerConfigs::Resolver ); }
  FILE_API inline bool HasRCS( ManagerConfig managerConfig ) { return ( ( managerConfig & ManagerConfigs::RCS ) == ManagerConfigs::RCS ); }
  FILE_API inline bool HasDisc( ManagerConfig managerConfig ) { return ( ( managerConfig & ManagerConfigs::Disc ) == ManagerConfigs::Disc ); }
  FILE_API inline bool HasFailIfExists( ManagerConfig managerConfig ) { return ( ( managerConfig & ManagerConfigs::FailIfExists ) == ManagerConfigs::FailIfExists ); }
  FILE_API inline bool HasPreview( ManagerConfig managerConfig ) { return ( ( managerConfig & ManagerConfigs::Preview ) == ManagerConfigs::Preview ); }


  /////////////////////////////////////////////////////////////////////////////

  namespace OperationFlags
  {
    enum OperationFlag
    {
      Add,
      Copy,
      Move,
      Delete,
      Migrate,
    };
  }
  typedef OperationFlags::OperationFlag OperationFlag;


  /////////////////////////////////////////////////////////////////////////////

  //
  // Forwards
  // 

  class Resolver;
  typedef FILE_API Nocturnal::SmartPtr< Resolver > ResolverPtr;

  class TempResolver;
  typedef FILE_API Nocturnal::SmartPtr< TempResolver > TempResolverPtr;

  class Manager;
  typedef FILE_API Nocturnal::SmartPtr< Manager > ManagerPtr;

  class ManagerEvent;
  typedef Nocturnal::SmartPtr< ManagerEvent > ManagerEventPtr;
  typedef std::vector< ManagerEventPtr > V_ManagerEventPtr;

  class FileVisitor;

  class FILE_API Manager : public Nocturnal::RefCountBase< Manager >
  {
  public:

    //
    //  CTOR/DTOR
    //

    Manager( );
    virtual ~Manager();
    
    void Initialize( const std::string& projectRoot, const std::string& projectAssets = std::string( "" ) );
    void Cleanup();

    // 
    // Transactions
    //
    
    void BeginTrans( const ManagerConfig managerConfig = ManagerConfigs::Default, bool manageEventsFile = true );
    void CommitTrans( bool partial = false );
    void RollbackTrans();
    bool IsTransOpen() const { return m_IsTransOpen; }


    //
    //  BASIC MANAGE API FUNCTIONS
    //
    
    void Update();
    void ClearTempResolver();

    void SetWorkingChangeset( const RCS::Changeset& changeset );

    bool ValidateCanAddFile( const std::string& filePath, const ManagerConfig managerConfig, const std::string& assetBranch, std::string& error );
    tuid Add( const std::string &name, const tuid tryId = TUID::Null, const ManagerConfig managerConfig = ManagerConfigs::Default );
    
    tuid Open( const std::string &name, const ManagerConfig managerConfig = ManagerConfigs::Default );
    tuid Open( const std::string &name, const tuid tryId, const ManagerConfig managerConfig = ManagerConfigs::Default );

    bool ValidateCanMoveFile( const std::string& sourcePath, const std::string& targetPath, const ManagerConfig managerConfig, const std::string& assetBranch, std::string& error, const bool move = true );
    tuid Move( const std::string &sourceName, const std::string &targetName, const ManagerConfig managerConfig = ManagerConfigs::Default, const bool move = true );
    void Move( M_string& renameFileList, const ManagerConfig moveConfig = ManagerConfigs::Default, const bool move = true );

    bool ValidateCanCopyFile( const std::string& sourcePath, const std::string& targetPath, const ManagerConfig managerConfig, const std::string& assetBranch, std::string& error );
    tuid Copy( const std::string &sourceName, const std::string &targetName, const ManagerConfig managerConfig = ManagerConfigs::Default );
    void Copy( M_string& renameFileList, const ManagerConfig managerConfig = ManagerConfigs::Default );

    bool ValidateCanDeleteFile( const std::string& filePath, const ManagerConfig managerConfig, const std::string& assetBranch, std::string& error );
    void Delete( const std::string &name, const ManagerConfig managerConfig = ManagerConfigs::Default );
    void Delete( const S_string &deleteFileList, const ManagerConfig managerConfig = ManagerConfigs::Default );
    
    //
    //  SEARCH API FUNCTIONS
    //

    void Find( const std::string& searchQuery, V_string &listOfFilesPaths );
    void Find( const std::string& searchQuery, V_ManagedFilePtr& listOfFiles );

    std::string GetPath( const tuid &id );
    bool GetPath( const tuid &id, std::string &filePath );
    tuid GetID( const std::string &path );
    
    ManagedFilePtr GetManagedFile( const tuid &id );
    ManagedFilePtr GetManagedFile( const std::string& path );

    std::string GetManagedAssetsRoot();

    bool Contains( const tuid id, const bool getDeletedFiles = false );
    
    void GetFileHistory( const ManagedFilePtr& file, S_FileHistory& history );
    void FindFilesByHistory( const std::string& searchQuery, const std::string& modifiedBy, V_ManagedFilePtr& listOfFiles, const std::string& operation = "%", bool searchHistoryData = false );

    void GetUsernameByID( const u32 userID, std::string& username );

    void DumpEventsToTextFile( const std::string& eventsFile = std::string( "" ), const std::string& txtFile = std::string( "" ), const std::string& filter = std::string( "" ) );
    void LoadEventsFromTextFile( const std::string& txtFile = std::string( "" ), const std::string& eventsFile = std::string( "" ) );

    // Static function for moving a ManagedFile asset from one project's File::Manager to another
    tuid Migrate( const ManagedFilePtr& sourceFile, Manager* sourceManager, const ManagerConfig managerConfig = ManagerConfigs::Default );

    friend Manager& ::File::GlobalManager();
    friend class ::File::FileVisitor;
  
  protected:
    // Hide the copy ctor and assignment operator
    Manager( const Manager& rhs );
    Manager& operator=( const Manager& rhs );

    // Used by the File::Visitor class to only add the file to the user's local DB, w/o creating an event
    tuid ResolverAddEntry( const std::string &relativePath, const tuid tryId = TUID::Null );
    tuid TempResolverAddEntry( ManagedFilePtr& file );
    
  private:
    
    bool              m_IsInitialized;
    
    ResolverPtr       m_Resolver;
    TempResolverPtr   m_TempResolver;
 
    std::string       m_ManagedAssetsRoot;

    RCS::Changeset    m_WorkingChangeset;

    // 
    // Transactions
    //

    bool              m_IsTransOpen;
    ManagerConfig     m_TransManagerConfig;  
    V_ManagerEventPtr m_TransManagerEvents;
    
    inline void ThrowIfMaxOpenTrans() { if ( IsTransOpen() ) throw MaxOpenTransException(); }
    inline void ThrowIfNoTransOpen() { if ( !IsTransOpen() ) throw NoTransOpenException(); }

    void RCSAdd( ManagerEvent* managerEvent, bool doIt = false );
    void RCSMove( ManagerEvent* managerEvent, bool doIt = false );
    void RCSRevertMove( ManagerEvent* managerEvent );
    void RCSDelete( ManagerEvent* managerEvent, bool doIt = false );
    void RCSMigrate( ManagerEvent* managerEvent, bool doIt = false );
    void RCSRevertMigrate( ManagerEvent* managerEvent );
    
    ManagerConfig GetOperationManagerConfig( ManagerConfig managerConfig );
  };

} // napespace File