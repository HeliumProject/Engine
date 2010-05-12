#include "Windows/Windows.h"

#include "Manager.h"

#include "Exceptions.h"
#include "TempResolver.h"
#include "Resolver.h"
#include "Reference.h"
#include "Visitor.h"

#include "Common/Assert.h"
#include "Common/CommandLine.h"
#include "Common/InitializerStack.h"
#include "Common/Memory/SmartPtr.h"

#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/ProjectSpecs.h"
#include "Console/Console.h"
#include "Windows/Error.h"
#include "Windows/Thread.h"
#include "Profile/Profile.h"
#include "RCS/RCS.h"

#include "Reflect/Registry.h"
#include "Reflect/Serializers.h"
#include "Common/InitializerStack.h"

DWORD g_DefaultManagerKey = TLS_OUT_OF_INDEXES; 
typedef std::set< File::Manager* > S_Manager;
S_Manager g_Managers;
Windows::CriticalSection g_GlobalManagerSection;

BOOL __stdcall DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{ 
  using namespace File;

  BOOL result = TRUE;

  switch (fdwReason) 
  { 
  case DLL_PROCESS_ATTACH: 
    {
      g_DefaultManagerKey = TlsAlloc();
      result = g_DefaultManagerKey != TLS_OUT_OF_INDEXES;
      break;
    }

    // Skip DLL_THREAD_ATTACH, we will just create a manager when a thread requests one

  case DLL_THREAD_DETACH: 
    {
      Manager* manager = (File::Manager*)TlsGetValue(g_DefaultManagerKey);
      if ( manager )
      {
        g_Managers.erase( manager );
        TlsSetValue(g_DefaultManagerKey, NULL); 
        delete manager;
      }
      break; 
    }

  case DLL_PROCESS_DETACH: 
    {
      Manager* manager = (File::Manager*)TlsGetValue(g_DefaultManagerKey);
      if ( manager )
      {
        g_Managers.erase( manager );
        delete manager;
      }

      TlsFree( g_DefaultManagerKey );
      g_DefaultManagerKey = TLS_OUT_OF_INDEXES;

      // Catch all for any memory leaks caused by improper thread termination
      if ( !g_Managers.empty() )
      {
        Console::Warning( "Detected %d orphan File::Manager(s), manually deleting them.\n", g_Managers.size() );
        for ( S_Manager::const_iterator managerItr = g_Managers.begin(), managerEnd = g_Managers.end();
          managerItr != managerEnd; ++managerItr )
        {
          delete *managerItr;
        }
        g_Managers.clear();
      }
      break;
    }
  } 

  return result; 
}

namespace File
{
  const char* File::Args::NoAutoSubmit = "no_auto_submit";
  const char* File::Args::FixupTUIDs = "fixup_tuids";

  /////////////////////////////////////////////////////////////////////////////
  static inline void PrependFilePath( const std::string& projectAssets, std::string& path )
  {
    if ( !FileSystem::HasPrefix( projectAssets, path ) )
    {
      path = projectAssets + path;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  //
  //  GLOBAL MANAGER FUNCTIONS
  //
  /////////////////////////////////////////////////////////////////////////////

  int g_InitCount = 0;
  Nocturnal::InitializerStack g_InitializerStack;

  void Initialize()
  {
    if ( ++g_InitCount == 1 ) 
    {
      Console::Print( Console::Levels::Verbose, "File::Initialize...\n" );

      g_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
      g_InitializerStack.Push( Reflect::RegisterClass< Reference >( "FileReference" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<ManagedFile>( "ManagedFile" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<ManagedFileStash>( "ManagedFileStash" ) );

      Reflect::Archive::AddSerializeListener( Reflect::SerializeSignature::Delegate ( &File::Serialize ) );
      Reflect::Archive::AddDeserializeListener( Reflect::DeserializeSignature::Delegate ( &File::Deserialize ) );
    }
  }

  void Cleanup()
  {
    if ( --g_InitCount == 0 )
    {
      Console::Print( Console::Levels::Verbose, "File::Cleanup...\n" );

      Reflect::Archive::RemoveSerializeListener( Reflect::SerializeSignature::Delegate ( &File::Serialize ) );
      Reflect::Archive::RemoveDeserializeListener( Reflect::DeserializeSignature::Delegate ( &File::Deserialize ) );

      g_InitializerStack.Cleanup();
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns a reference to the static global Manager theGlobalManager.
  //
  Manager& GlobalManager()
  {
    Manager* manager = (Manager*)TlsGetValue(g_DefaultManagerKey);
    if ( manager == NULL )
    {
      Windows::TakeSection section( g_GlobalManagerSection );
      manager = new File::Manager();
      g_Managers.insert( manager );
      manager->Initialize( Finder::ProjectRoot(), Finder::ProjectAssets() );
      TlsSetValue(g_DefaultManagerKey, manager); 
    }
    return *manager;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Swap out the global manager with the supplied manager
  //
  void SwapGlobalManager( Manager* fileManager )
  {
    throw File::Exception( "Swapping Global File Managers is not currently supported" );
  }

  /////////////////////////////////////////////////////////////////////////////
  // ManagerEvent class
  class ManagerEvent : public Nocturnal::RefCountBase< ManagerEvent >
  {
  public:
    bool          m_OpCompleted;

    OperationFlag m_OperationFlag;
    ManagerConfig m_ManagerConfig;
    std::string   m_FilePath;
    std::string   m_TargetFilePath; // is empty for Add and Delete commands      

    ManagerEvent( OperationFlag opFlag, ManagerConfig managerConfig, const std::string& filePath )
      : m_OpCompleted( false )
      , m_OperationFlag( opFlag )
      , m_ManagerConfig( managerConfig )
      , m_FilePath( filePath )
    {
      NOC_ASSERT( opFlag != OperationFlags::Copy || opFlag != OperationFlags::Move );
    }

    ManagerEvent( OperationFlag opFlag, ManagerConfig managerConfig, const std::string& filePath, const std::string& targetFilePath )
      : m_OpCompleted( false )
      , m_OperationFlag( opFlag )
      , m_ManagerConfig( managerConfig )
      , m_FilePath( filePath )
      , m_TargetFilePath( targetFilePath )
    {
    }
  };



  /////////////////////////////////////////////////////////////////////////////
  //
  //  CTOR/DTOR
  //
  /////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////
  // Default constructor
  //
  Manager::Manager( )
    : m_IsInitialized( false )
    , m_IsTransOpen( false )
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  // Dtor calls Cleanup()
  Manager::~Manager()
  {
    Cleanup();
  }

  /////////////////////////////////////////////////////////////////////////////
  // Initializes the Manager
  void Manager::Initialize( const std::string& projectRoot, const std::string& projectAssets )
  {
    // Only call this function once
    NOC_ASSERT( !m_IsInitialized );

    if ( m_IsInitialized )
      return;

    m_ManagedAssetsRoot = projectAssets;

    m_Resolver = new Resolver();
    m_Resolver->Initialize( projectRoot, projectAssets );   

    m_TempResolver = new TempResolver();

    m_IsInitialized = true;
  }


  /////////////////////////////////////////////////////////////////////////////
  void Manager::Cleanup()
  {
    // Only call this function once
    if ( !m_IsInitialized )
      return;

    m_TransManagerEvents.clear();

    m_Resolver = NULL;
    m_TempResolver = NULL;

    m_IsInitialized = false;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Update the file resolver after new events have been received.
  // 
  void Manager::Update()
  {
    m_Resolver->Update();
  }

  /////////////////////////////////////////////////////////////////////////////
  void Manager::ClearTempResolver()
  {
    m_TempResolver->ClearCache();
  }

  /////////////////////////////////////////////////////////////////////////////
  void Manager::SetWorkingChangeset( const RCS::Changeset& changeset )
  {
    m_WorkingChangeset = changeset;
    m_Resolver->SetWorkingChangeset( changeset );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  TRANSACTION FUNCTIONS
  //
  /////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////
  // Begins a new transaction; this wrapper ensures that there is only one
  // open transaction at all times.
  //
  void Manager::BeginTrans( const ManagerConfig managerConfig, bool manageEventsFile )
  {
    ThrowIfMaxOpenTrans();

    m_TransManagerConfig = managerConfig;
    m_TransManagerEvents.clear();
    m_Resolver->BeginTrans( manageEventsFile && !Nocturnal::GetCmdLineFlag( File::Args::NoAutoSubmit ) );

    m_IsTransOpen = true;
  }



  /////////////////////////////////////////////////////////////////////////////
  // Commits the existing transaction; this wrapper ensures that there is
  // only one open transaction at all times.
  //
  void Manager::CommitTrans( bool partial )
  {
    ThrowIfNoTransOpen();

    try
    {
      for each ( const ManagerEventPtr& managerEvent in m_TransManagerEvents )
      {
        if ( managerEvent->m_OpCompleted )
          continue;

        switch( managerEvent->m_OperationFlag )
        {
        case OperationFlags::Add :
          RCSAdd( managerEvent, true );
          break;

        case OperationFlags::Move :
        case OperationFlags::Copy :
          RCSMove( managerEvent, true );
          break;

        case OperationFlags::Delete :
          RCSDelete( managerEvent, true );
          break;

        case OperationFlags::Migrate :
          //RCSMigrate( managerEvent, true );
          RCSMove( managerEvent, true );
          break;

        default:
          throw Exception( "Unknown RCS operation." );
          break;
        }
      }
    }
    catch( ... )
    {
      // try to rollback as much as we can :(
      RollbackTrans();

      throw;
    }

    // Only commit the perforce file transactions
    if ( !partial )
    {
      m_Resolver->CommitTrans();

      m_TransManagerConfig = ManagerConfigs::Default;
      m_TransManagerEvents.clear();
      m_IsTransOpen = false;
    }
  }



  /////////////////////////////////////////////////////////////////////////////
  // RollBack the all transactions; this wrapper ensures that there is
  // only one open transaction at all times.
  //
  void Manager::RollbackTrans()
  {
    ThrowIfNoTransOpen();

    m_Resolver->RollbackTrans();

    for each ( const ManagerEventPtr& managerEvent in m_TransManagerEvents )
    {
      if ( !managerEvent->m_OpCompleted )
        continue;

      RCS::File rcsFile( managerEvent->m_FilePath );

      switch( managerEvent->m_OperationFlag )
      {
      case OperationFlags::Add :
        rcsFile.Revert(); 
        break;

      case OperationFlags::Move :
      case OperationFlags::Copy :
        RCSRevertMove( managerEvent );
        break;

      case OperationFlags::Delete :
        rcsFile.Revert();
        break;

      case OperationFlags::Migrate :
        //RCSRevertMigrate( managerEvent );
        RCSRevertMove( managerEvent );
        break;

      default:
        throw Exception( "Unknown RCS operation." );
        break;
      }
    }

    m_TransManagerConfig = ManagerConfigs::Default;
    m_TransManagerEvents.clear();
    m_IsTransOpen = false;
  }


  /////////////////////////////////////////////////////////////////////////////
  // Function to determine what to do with errors
  //
  void inline HandleValidationError( const std::string& error, const ManagerConfig managerConfig, bool isFatal = false )
  {
    if ( HasPreview( managerConfig ) )
    {
      Console::Error( "%s\n", error.c_str() );
    }
    else if ( !HasFailIfExists( managerConfig ) && !isFatal )
    {
      Console::Warning( "%s\n", error.c_str() );
    }
    else if ( !isFatal )
    {
      Console::Error( "%s\n", error.c_str() );
    }
    else
    {
      throw Exception( error.c_str() );
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // 
  ManagerConfig Manager::GetOperationManagerConfig( ManagerConfig managerConfig )
  {
    ManagerConfig opManagerConfig = managerConfig;
    if ( IsTransOpen() )
    {
      if ( HasPreview( m_TransManagerConfig ) )
      {
        opManagerConfig |= ManagerConfigs::Preview;
      }

      if ( !HasFailIfExists( m_TransManagerConfig ) )
      {
        opManagerConfig &= ~ManagerConfigs::FailIfExists;
      }
    }

    return opManagerConfig;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  ADD FUNCTIONS
  //
  /////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////
  // Add the file to revision control
  //
  void Manager::RCSAdd( ManagerEvent* managerEvent, bool doIt )
  {
    if ( !doIt )
    {
      m_TransManagerEvents.push_back( managerEvent );
      return;
    }

    RCS::File file( managerEvent->m_FilePath );
    file.GetInfo();   

    // Note: if the file is open for add/edit it will not be reverted is the transaction is rolledback
    if ( !file.ExistsInDepot() || file.HeadDeleted() )
    {
      m_WorkingChangeset.Open( file );

      managerEvent->m_OpCompleted = true;
    }    
  }

  //////////////////////////////////////////
  // Error check/validation adding a file
  //
  bool Manager::ValidateCanAddFile( const std::string& filePath, const ManagerConfig managerConfig, const std::string& assetBranch, std::string& error )
  {
    bool errorOccurred = false;
    std::stringstream errorStream;
    errorStream << "Cannot add the file: " << filePath << std::endl;

    if ( !errorOccurred && HasResolver( managerConfig ) ) 
    {
      if ( !FileSystem::HasPrefix( assetBranch, filePath ) )
      {
        errorStream << "Outside of the asset branch: " << assetBranch << std::endl;
        errorOccurred = true;
      }
    } 

    if ( !errorOccurred && HasRCS( managerConfig ) )
    {
      RCS::File rcsFile( filePath );
      rcsFile.GetInfo();

      if ( rcsFile.ExistsInDepot() && !rcsFile.IsUpToDate() && !rcsFile.HeadDeleted() )
      {
        errorStream << "You do not have the latest revision. Please getassets and try again." << std::endl;
        errorOccurred = true;
      }
    }  

    if ( errorOccurred )
    {
      error = errorStream.str();
    }

    return !errorOccurred;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Add a file that exists on disk to the FileResolver, returns the
  // tuid id of the newly created File in FileResolver
  //
  tuid Manager::Add( const std::string &name, const tuid tryId, const ManagerConfig managerConfig )
  { 
    FILE_SCOPE_TIMER((""));

    ManagerConfig opManagerConfig = GetOperationManagerConfig( managerConfig );   

    std::string filePath;
    FileSystem::MakeAbsolute( name, filePath );

    std::string relativePath = filePath;
    FileSystem::StripPrefix( m_ManagedAssetsRoot, relativePath );

    // if Preview, output the text then return
    if ( HasPreview( opManagerConfig ) )
    {
      std::string operation = "Adding";
      std::string configString = ManagerConfigs::GetManagerConfigString( managerConfig );

      Console::Bullet add ( "%s file %s...\n", operation.c_str(), configString.c_str() );
      {
        Console::Print( "%s\n", filePath.c_str() );
      }
    }


    ////////////////////////////////////////////
    // Error check/validation
    std::string error;
    bool canAddFile = ValidateCanAddFile( filePath, opManagerConfig, m_ManagedAssetsRoot, error );
    if ( !canAddFile )
    {
      HandleValidationError( error, opManagerConfig, true );
    }

    // early out if in Preview mode
    if ( HasPreview( opManagerConfig ) )
    {
      return TUID::Null; 
    }


    bool isTransOpen = IsTransOpen();
    if ( !isTransOpen )
    {    
      BeginTrans( managerConfig );
    }

    tuid assetId = tryId;

    try
    {
      //////////////////////////////////////////
      // File::Resolver
      if ( HasResolver( managerConfig ) )
      {
        try
        {
          tuid existingId = m_Resolver->GetFileID( relativePath );
          if ( existingId != TUID::Null )
          {
            assetId = existingId;
          }
          else
          {   
            assetId = ResolverAddEntry( relativePath, tryId );
          }
        }
        catch ( const DuplicateEntryException &ex )
        {
          assetId = ex.GetExistingID();
        }
      }

      //////////////////////////////////////////
      // RCS
      if ( HasRCS( managerConfig ) )
      {
        RCSAdd( new ManagerEvent( OperationFlags::Add, managerConfig, filePath ) );
      }
    }
    catch ( ... )
    {
      if ( !isTransOpen )
      {    
        RollbackTrans();
      }

      throw;
    }

    if ( !isTransOpen )
    {    
      CommitTrans();
    }

    return assetId;
  }


  /////////////////////////////////////////////////////////////////////////////
  tuid Manager::ResolverAddEntry( const std::string &relativePath, const tuid tryId  )
  {
    tuid assetId = m_Resolver->AddEntry( relativePath, tryId );

    // Remove the file from the TempResolver
    m_TempResolver->DeleteEntry( tryId );

    return assetId;
  }


  /////////////////////////////////////////////////////////////////////////////
  // Adds the asset to the local resolver only
  tuid Manager::TempResolverAddEntry( ManagedFilePtr& file )
  {
    if ( !m_Resolver->Contains( file->m_Id ) )
    {
      if ( !Nocturnal::GetCmdLineFlag( File::Args::FixupTUIDs ) )
      {
        // Add the file to the TempResolver without generating an Event
        Console::Print( Console::Levels::Verbose, "Adding local file resolver entry for %s\n", file->m_Path.c_str() );

        m_TempResolver->AddEntry( file );
      }
      else
      {
        Console::Warning( "Generating event for missing file reference: %s ("TUID_HEX_FORMAT")", file->m_Path.c_str(), file->m_Id );

        bool isTransOpen = IsTransOpen();
        if ( !isTransOpen )
        {    
          BeginTrans( ManagerConfigs::Default );
        }

        try
        {
#pragma TODO( "This hack doesn't work with non-uniform IG_ROOT" )
#pragma TODO( "Remove this hack to fix up files with cached resolver entries that are absolute paths -Geoff" )
          Finder::StripAnyProjectAssets( file->m_Path );

          ResolverAddEntry( file->m_Path, file->m_Id );
        }
        catch ( ... )
        {
          if ( !isTransOpen )
          {    
            RollbackTrans();
          }

          throw;
        }

        if ( !isTransOpen )
        {    
          CommitTrans();
        }
      }
    }

    return file->m_Id;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  OPEN FUNCTIONS
  //
  /////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////
  // Try to find the file and return the Id if it exists, otherwise
  // add a file that exists on disk to the FileResolver, returns the
  // tuid id of the newly created File in FileResolver
  //
  tuid Manager::Open( const std::string &name, const ManagerConfig managerConfig )
  {
    return Open( name, TUID::Null, managerConfig );
  }

  tuid Manager::Open( const std::string &name, const tuid tryId, const ManagerConfig managerConfig )
  { 
    FILE_SCOPE_TIMER((""));

    tuid fileId = GetID( name );

    if ( fileId != TUID::Null )
    {
      // File that is being set is already in the resolver.  Were they trying to fix
      // a broken file reference?
      if ( tryId != TUID::Null && tryId != fileId )
      {
        Console::Warning( "Attempted to set path '%s' to ["TUID_HEX_FORMAT"], but there was already a file ID for this path. Using existing file ID ["TUID_HEX_FORMAT"].\n", name.c_str(), tryId, fileId );
      }
    }
    else if ( FileSystem::Exists( name ) &&
      FileSystem::HasExtension( name, FinderSpecs::Extension::REFLECT_BINARY.GetExtension() ) )
    {
      // It turns out that we do inadvertently store information about the file we are serializing,
      //  but we do it in such a way that the file is still not self-referential. Example: AssetClass has a
      //  non-serialized (but reflection-aware) member variable that is the tuid of the file resovler id.
      //  When reflect serializes an AssetClass it stashes out the id and file path (as metadata,
      //  not as the actual member data), but the API that stashes the extra meta data doesn't know 
      //  its the file currently being processed. Since we don't actually serialize the tuid,
      //  the file still doesn't know it was located at that path. -Geoff
      //
      // Loading the file here should restore the metadata that *may* know what the file id is in the local resolver
      //  This is somewhat expensive and kind of a long-shot.  How frequently does this really happen? -Geoff
      try
      {
        Reflect::ElementPtr tempElement = Reflect::Archive::FromFile< Reflect::Element >( name );

        fileId = File::GlobalManager().GetID( name );
      }
      catch ( Nocturnal::Exception& )
      { 
        // do nothing
      }
    }

    // Otherwise, the file was not in the resolver.  Try to add it.
    if ( fileId == TUID::Null )
    {
      fileId = File::GlobalManager().Add( name, tryId );
    }

    return fileId;
  }




  /////////////////////////////////////////////////////////////////////////////
  //
  //  MOVE/COPY FUNCTIONS
  //
  /////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////
  // Using CopyFile/RCS::Copy and DeleteFile/RCS::Delete (because MoveFile is scary)
  //
  // Note: if the file is open for add/edit it will not be reverted if the move fails
  //
  void Manager::RCSMove( ManagerEvent* managerEvent, bool doIt )
  {
    // early out if a transaction is open
    if ( !doIt )
    {
      m_TransManagerEvents.push_back( managerEvent );
      return;
    }

    RCS::File source( managerEvent->m_FilePath );
    source.GetInfo();

    RCS::File target( managerEvent->m_TargetFilePath );   
    target.GetInfo();

    if ( source.ExistsInDepot() && !source.HeadDeleted() )
    {
      // RCS::Copy will either integrate or it may potentially
      // do nothing if all versions have been integrated
      m_WorkingChangeset.Copy( source, target );

      // reopen the file so the user to continue modifing it after the branch
      target.GetInfo();
      if ( target.IsCheckedOutByMe() )
      {
        m_WorkingChangeset.Edit( target );
      }

      // if, some how, we didn't check it out - open it explicitly
      // Note: This can happen all revisions had been integrated
      //       and perforce thought it had nothing to do
      else if ( !target.IsCheckedOutByMe() )
      {
        m_WorkingChangeset.Open( target );
      }
    }
    else
    {
      m_WorkingChangeset.Open( target );
    }

    // if the file is open for edit, force copy on disk also
    // FIXME: later checkin THEN reopen and copy new edits
    if ( ( !source.ExistsInDepot()
      || source.IsCheckedOutByMe() )
      && FileSystem::Exists( managerEvent->m_FilePath ) )
    {
      target.GetInfo();
      if ( target.IsCheckedOutByMe() )
      {
        std::string win32FilePath = managerEvent->m_FilePath;
        FileSystem::Win32Name( win32FilePath );

        std::string win32TargetFilePath = managerEvent->m_TargetFilePath;
        FileSystem::Win32Name( win32TargetFilePath );

        FileSystem::MakePath( managerEvent->m_TargetFilePath, true );
        if ( !CopyFile( win32FilePath.c_str(), win32TargetFilePath.c_str(), HasFailIfExists( managerEvent->m_ManagerConfig ) ) )
        {
          throw Windows::Exception( "Could not copy file %s to %s, Error: %s",
            managerEvent->m_FilePath.c_str(),
            managerEvent->m_TargetFilePath.c_str(),
            Windows::GetErrorString().c_str() );
        }
      }
    }

    // If we are moving the file, Delete the source file
    if ( ( managerEvent->m_OperationFlag & OperationFlags::Move ) == OperationFlags::Move )
    {
      if ( source.IsCheckedOutByMe() )
      {
        source.Revert();
      }

      source.GetInfo();
      if ( source.ExistsInDepot() && !source.HeadDeleted() )
      {
        m_WorkingChangeset.Delete( source );
      }
    }

    managerEvent->m_OpCompleted = true;
  }


  /////////////////////////////////////////////////////////////////////////////
  // Called from RollbackTrans to revert a Move/Copy operation
  //
  void Manager::RCSRevertMove( ManagerEvent* managerEvent )
  {
    // revert the copy before rethrow
    RCS::File target( managerEvent->m_TargetFilePath );
    target.GetInfo();
    if ( target.IsCheckedOutByMe() )
    {
      target.Revert();
    }

    target.GetInfo();
    if ( !target.ExistsInDepot() )
    {
      std::string win32TargetFilePath = managerEvent->m_TargetFilePath;
      FileSystem::Win32Name( win32TargetFilePath );

      DeleteFile( win32TargetFilePath.c_str() );
    }

    // If we were moving the file, revert the delete of the source file
    if ( ( managerEvent->m_OperationFlag & OperationFlags::Move ) == OperationFlags::Move )
    {
      RCS::File source( managerEvent->m_FilePath );
      source.GetInfo();
      if ( source.IsCheckedOutByMe() )
      {
        source.Revert();
      }
    }

    managerEvent->m_OpCompleted = false;
  }


  /////////////////////////////////////////////////////////////////////////////
  // Error check/validation
  //
  bool Manager::ValidateCanMoveFile( const std::string& sourcePath, const std::string& targetPath, const ManagerConfig managerConfig, const std::string& assetBranch, std::string& error, const bool move )
  {
    bool errorOccurred = false;
    std::stringstream errorStream;
    errorStream << "Cannot move or copy the file " << sourcePath << " to " << targetPath << std::endl;

    bool targetExistedOnDisk = FileSystem::Exists( targetPath );

    if ( HasDisc( managerConfig ) )
    {
      // get info on the target and make sure the file does not exist
      if ( !FileSystem::Exists( sourcePath ) )
      {
        errorStream << "Source file does not exist on disk " << sourcePath << std::endl;
        errorOccurred = true;
      }

      if ( targetExistedOnDisk )
      {
        errorStream << "Target file already exists on disc " << targetPath << std::endl;
        errorOccurred = true;
      }
    }

    if ( HasRCS( managerConfig ) )
    {
      bool gotInfo = false;
      RCS::File target( targetPath );
      try
      {
        target.GetInfo();
        gotInfo = true;
      }
      catch ( const Nocturnal::Exception& e )
      {
        errorStream << "RCS::GetInfo failed: " << e.Get() << std::endl;
        errorOccurred = true;
      }

      if ( gotInfo )
      {
        if ( target.ExistsInDepot() && !target.HeadDeleted() )
        {
          errorStream << "Target file already exists in Perforce " << targetPath << std::endl;
          errorOccurred = true;
        }
        else if ( target.ExistsInDepot() && !target.IsUpToDate() )
        {
          errorStream << "You do not have the latest revision of the target file. Please getassets and try again." << std::endl;
          errorOccurred = true;
        }
      }
    }  

    if ( HasResolver( managerConfig ) ) 
    {
      // strip the project root
      std::string sourceRelativePath = sourcePath;
      FileSystem::StripPrefix( m_ManagedAssetsRoot, sourceRelativePath );

      std::string targetRelativePath = targetPath;
      FileSystem::StripPrefix( m_ManagedAssetsRoot, targetRelativePath );

      if ( move && GetID( targetRelativePath ) != TUID::Null )
      {
        errorStream << "Target file already exists in file manager system " << targetPath << std::endl;
        errorOccurred = true;
      }

      if ( move && GetID( sourceRelativePath ) == TUID::Null )
      {
        errorStream << "Source file does not exist in file manager system " << sourcePath << std::endl;
        errorOccurred = true;
      }

      // if the file isn't in the project root
      if ( !FileSystem::HasPrefix( assetBranch, targetPath ) )
      {
        errorStream << "The target path is outside of the asset branch: " << assetBranch << std::endl;
        errorOccurred = true;
      }
    } 

    if ( errorOccurred )
    {
      error = errorStream.str();
    }
    return !errorOccurred;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Move the file on disk and in the FileResolver, and branch it in perforce
  // Move an existing file (that exists on disk and in the File::Resolver), 
  // returns the tuid id of the File in File::Resolver
  //
  tuid Manager::Move( const std::string &sourceName, const std::string &targetName, const ManagerConfig managerConfig, const bool move )
  {
    FILE_SCOPE_TIMER((""));

    ManagerConfig opManagerConfig = GetOperationManagerConfig( managerConfig );

    std::string sourcePath;
    FileSystem::MakeAbsolute( sourceName, sourcePath );

    std::string targetPath;
    FileSystem::MakeAbsolute( targetName, targetPath );

    // strip the project root
    std::string sourceRelativePath = sourcePath;
    FileSystem::StripPrefix( m_ManagedAssetsRoot, sourceRelativePath );

    std::string targetRelativePath = targetPath;
    FileSystem::StripPrefix( m_ManagedAssetsRoot, targetRelativePath );


    // if Preview, output the text then return
    if ( HasPreview( opManagerConfig ) )
    {
      std::string operation = ( move ? "Moving" : "Copying" );
      std::string configString = ManagerConfigs::GetManagerConfigString( managerConfig );

      Console::Bullet move ( "%s file %s...\n", operation.c_str(), configString.c_str() );
      {
        Console::Print( "From:  %s\n", sourceRelativePath.c_str() );
        Console::Print( "To:    %s\n", targetRelativePath.c_str());
      }
    }

    ////////////////////////////////////////////
    // Error check/validation
    std::string error;
    bool canMoveFile = ValidateCanMoveFile( sourcePath, targetPath, opManagerConfig, m_ManagedAssetsRoot, error, move);
    if ( !canMoveFile )
    {
      HandleValidationError( error, opManagerConfig, true );
    }

    // early out if in Preview mode
    if ( HasPreview( opManagerConfig ) )
    {
      return TUID::Null; 
    }

    //////////////////////////////////////////
    // Perform the Move
    tuid targetId = TUID::Null;

    bool isTransOpen = IsTransOpen();
    if ( !isTransOpen )
    {    
      BeginTrans( managerConfig );
    }

    try
    {
      //////////////////////////////////////////
      // File::Resolver
      if ( HasResolver( managerConfig ) )
      {
        if ( move )
        {
          // get the source file's ID and set the file path
          ManagedFilePtr sourceFile = m_Resolver->GetFile( sourceRelativePath );
          if ( !sourceFile )
          {
            //sourceFile = m_TempResolver->GetFile( sourceRelativePath );
            //if ( !sourceFile )
            //{
            throw File::Exception( "Cannot move file that doesn't exist in File::Resolver database." );
            //}
            //else
            //{
            //  // Moving file that didn't exist in the global File::Resolver
            //  // TODO: targetId = AddGlobal( sourceFile );
            //  targetId = m_TempResolver->UpdateEntry( sourceFile, targetRelativePath );
            //}
          }
          else
          {
            targetId = m_Resolver->UpdateEntry( sourceFile, targetRelativePath );
            PrependFilePath( m_ManagedAssetsRoot, sourceFile->m_Path  );

            // Remove the file from the TempResolver
            m_TempResolver->DeleteEntry( targetId );

            // Make sure we remove the non-auth tuid if it exists in the TempResolver
            if ( targetId != sourceFile->m_Id )
            {
              m_TempResolver->DeleteEntry( sourceFile->m_Id );
            }
          }
        }
        else
        {
          // add the new file id if the path doesn't already exist
          targetId = m_Resolver->GetFileID( targetRelativePath );

          if ( targetId == TUID::Null )
          {
            targetId = ResolverAddEntry( targetRelativePath );
          }
        }

        if ( targetId != TUID::Null
          && !HasRCS( managerConfig )
          && !HasDisc( managerConfig ) )
        {
          if ( !isTransOpen )
          {    
            CommitTrans();
          }

          return targetId;
        }
      }


      //////////////////////////////////////////
      // RCS
      if ( HasRCS( managerConfig ) )
      {
        OperationFlag opFlag = ( move ? OperationFlags::Move : OperationFlags::Copy );
        RCSMove( new ManagerEvent( opFlag, managerConfig, sourcePath, targetPath ) );
      }
      // Disk
      else if ( HasDisc( managerConfig ) 
        && FileSystem::Exists( sourcePath ) )
      {
        // FIXME: this needs to happen in the transactions :(
        // Using CopyFile and DeleteFile because MoveFile is scary
        FileSystem::MakePath( targetPath, true );

        if ( !CopyFile( sourcePath.c_str(),
          targetPath.c_str(),
          HasFailIfExists( managerConfig ) ) )
        {
          std::string error = Windows::GetErrorString();
          throw Windows::Exception( "Could not copy file %s to %s, Error: %s", sourcePath.c_str(), targetPath.c_str(), error.c_str() );
        }
      }
    }
    catch( ... )
    {
      if ( !isTransOpen )
      {    
        RollbackTrans();
      }

      ////////////////////////////////////////
      // Disk
      // FIXME: this should happen in the transaction
      if ( !HasRCS( managerConfig ) && HasDisc( managerConfig ) )
      {
        DeleteFile( targetPath.c_str() );
      }

      throw;
    }

    if ( !isTransOpen )
    {    
      CommitTrans();
    }

    // Delete the source file
    if ( move && HasDisc( managerConfig ) )
    {
      DeleteFile( sourcePath.c_str() );
    }

    // always make sure the target file is writable
    SetFileAttributes( targetPath.c_str(), FILE_ATTRIBUTE_NORMAL );

    return targetId;
  }


  /////////////////////////////////////////////////////////////////////////////
  // Rename a list of files
  void Manager::Move( M_string& renameFileList, const ManagerConfig managerConfig, const bool move )
  {
    bool isTransOpen = IsTransOpen();
    if ( !isTransOpen )
    {    
      BeginTrans( managerConfig );
    }

    try
    {
      for each ( const M_string::value_type& renameFile in renameFileList )
      {
        Move( renameFile.first, renameFile.second, managerConfig, move );
      }
    }
    catch( ... )
    {
      if ( !isTransOpen )
      {    
        RollbackTrans();
      }
      throw;
    }

    if ( !isTransOpen )
    {    
      CommitTrans();
    }
  }


  bool Manager::ValidateCanCopyFile( const std::string& sourcePath, const std::string& targetPath, const ManagerConfig managerConfig, const std::string& assetBranch, std::string& error )
  {
    return ValidateCanMoveFile( sourcePath, targetPath, managerConfig, assetBranch, error, false );
  }

  /////////////////////////////////////////////////////////////////////////////
  // Copy an existing file (that exists on disk and in the File::Resolver), 
  // returns the tuid id of the newly created File in File::Resolver
  //
  tuid Manager::Copy( const std::string &sourceName, const std::string &targetName, const ManagerConfig managerConfig )
  {
    return Move( sourceName, targetName, managerConfig, false );
  }

  void Manager::Copy( M_string& renameFileList, const ManagerConfig managerConfig )
  {
    Move( renameFileList, managerConfig, false ); 
  }










  /////////////////////////////////////////////////////////////////////////////
  //
  //  DELETE FUNCTIONS
  //
  /////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////
  // 
  //
  void Manager::RCSDelete( ManagerEvent* managerEvent, bool doIt )
  {
    if ( !doIt )
    {
      m_TransManagerEvents.push_back( managerEvent );
      return;
    }

    RCS::File rcsFile( managerEvent->m_FilePath );
    rcsFile.GetInfo();      

    if ( rcsFile.IsCheckedOutByMe() )
    {
      rcsFile.Revert();

      // if we reverted an add P4 operation then we don't need to delete
      rcsFile.GetInfo();
      if ( rcsFile.ExistsInDepot() && !rcsFile.HeadDeleted() )
      {
        m_WorkingChangeset.Delete( rcsFile );
      }
    }
    else if ( rcsFile.ExistsInDepot() && !rcsFile.HeadDeleted() )
    {
      m_WorkingChangeset.Delete( rcsFile );
    }

    managerEvent->m_OpCompleted = true;
  }

  //////////////////////////////////////////
  // Error check/validation adding a file
  //
  bool Manager::ValidateCanDeleteFile( const std::string& filePath, const ManagerConfig managerConfig, const std::string& assetBranch, std::string& error )
  {
    bool errorOccurred = false;
    std::stringstream errorStream;
    errorStream << "Cannot delete the file: " << filePath << std::endl;

    if ( HasDisc( managerConfig ) )
    {
      if ( !FileSystem::Exists( filePath ) )
      {
        errorStream << "File does not exist on disk " << filePath ;
        errorOccurred = true;
      }
    }

    if ( HasRCS( managerConfig ) )
    {
      bool gotInfo = false;
      RCS::File rcsFile( filePath );
      try
      {
        rcsFile.GetInfo();
        gotInfo = true;
      }
      catch ( const Nocturnal::Exception& e )
      {
        errorStream << "RCS::GetInfo failed: " << e.Get() << std::endl;
        errorOccurred = true;
      }

      if ( gotInfo )
      {
        if ( !rcsFile.ExistsInDepot() || rcsFile.HeadDeleted() )
        {
          errorStream << "File does not exist in Perforce " << filePath << std::endl;
          errorOccurred = true;
        }  
        if ( rcsFile.IsCheckedOutByMe() )
        {
          errorStream << "File is currently open for edit in Perforce: " << filePath << std::endl;
          errorOccurred = true;
        }
        else if ( rcsFile.IsCheckedOut() )
        {
          errorStream << "File is currently open for edit in Perforce by someone else: " << filePath << std::endl;
          errorOccurred = true;
        }
      }
    }  

    if ( HasResolver( managerConfig ) ) 
    {
      // strip the project root
      std::string relativePath = filePath;
      FileSystem::StripPrefix( m_ManagedAssetsRoot, relativePath );

      if ( GetID( relativePath ) == TUID::Null )
      {
        errorStream << "File does not exist in the file manager system " << filePath << std::endl;
        errorOccurred = true;
      }
    }

    if ( errorOccurred )
    {
      error = errorStream.str();
    }
    return !errorOccurred;
  }


  /////////////////////////////////////////////////////////////////////////////
  // Delete the given file from RCS, File::Resolver and disk
  //
  void Manager::Delete( const std::string &name, const ManagerConfig managerConfig )
  {
    FILE_SCOPE_TIMER((""));

    ManagerConfig opManagerConfig = GetOperationManagerConfig( managerConfig );

    std::string filePath;
    FileSystem::MakeAbsolute( name, filePath );

    // strip the project root
    std::string relativePath = filePath;
    FileSystem::StripPrefix( m_ManagedAssetsRoot, relativePath );


    // if Preview, output the text then return
    if ( HasPreview( opManagerConfig ) )
    {
      std::string operation = "Deleting";
      std::string configString = ManagerConfigs::GetManagerConfigString( managerConfig );

      Console::Bullet add ( "%s file %s...\n", operation.c_str(), configString.c_str() );
      {
        Console::Print( "%s\n", filePath.c_str() );
      }
    }


    ////////////////////////////////////////////
    // Error check/validation
    std::string error;
    bool canDeleteFile = ValidateCanDeleteFile( filePath, opManagerConfig, m_ManagedAssetsRoot, error );
    if ( !canDeleteFile )
    {
      HandleValidationError( error, opManagerConfig, true );
    }


    // early out if in Preview mode
    if ( HasPreview( opManagerConfig ) )
    {
      return; 
    }


    //////////////////////////////////////////
    // Perform the delete operation
    //

    bool isTransOpen = IsTransOpen();
    if ( !isTransOpen )
    {    
      BeginTrans( managerConfig );
    }

    try
    {
      // Delete from File::Resolver
      if ( HasResolver( managerConfig ) )
      {
        // get the file's ID and delete it
        tuid id = TUID::Null;
        id = m_Resolver->GetFileID( relativePath );
        if ( id != TUID::Null )
        {
          m_Resolver->DeleteEntry( id );
        }

        m_TempResolver->DeleteEntry( id );
      }

      // Delete from revision control
      if ( HasRCS( managerConfig ) )
      {
        RCSDelete( new ManagerEvent( OperationFlags::Delete, managerConfig, filePath ) );
      }
      // Delete it from disk
      else if ( HasDisc( managerConfig ) && FileSystem::Exists( filePath )  )
      {
        // FIXME:this needs to happen in the transaction
        DeleteFile( filePath.c_str() );
      }
    }
    catch( ... )
    {
      if ( !isTransOpen )
      {    
        RollbackTrans();
      }

      throw;
    }

    if ( !isTransOpen )
    {    
      CommitTrans();
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  // Delete a list of files
  //
  void Manager::Delete( const S_string &deleteFileList, const ManagerConfig managerConfig  )
  {
    bool isTransOpen = IsTransOpen();
    if ( !isTransOpen )
    {    
      BeginTrans( managerConfig );
    }

    try
    {
      for each ( const std::string& deleteFile in deleteFileList )
      {
        Delete( deleteFile, managerConfig );
      }
    }
    catch( ... )
    {
      if ( !isTransOpen )
      {    
        RollbackTrans();
      }
      throw;
    }
    if ( !isTransOpen )
    {    
      CommitTrans();
    }
  }










  /////////////////////////////////////////////////////////////////////////////
  //
  //  SEARCH API FUNCTIONS
  //
  /////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////
  //
  void Manager::Find( const std::string& searchQuery, V_ManagedFilePtr& listOfFiles )
  {
    FILE_SCOPE_TIMER((""));

    std::string filePath = searchQuery;
    FileSystem::StripPrefix( m_ManagedAssetsRoot, filePath );

    m_Resolver->FindFilesByPath( filePath, listOfFiles, false );


    V_ManagedFilePtr::iterator itr = listOfFiles.begin();
    V_ManagedFilePtr::iterator end = listOfFiles.end();
    for( ; itr != end; ++itr )
    {
      PrependFilePath( m_ManagedAssetsRoot, (*itr)->m_Path  );
    }
  }


  void Manager::Find( const std::string &searchQuery, V_string &listOfFilesPaths )
  {
    FILE_SCOPE_TIMER((""));

    V_ManagedFilePtr listOfFiles;
    Find( searchQuery, listOfFiles );

    V_ManagedFilePtr::const_iterator itr = listOfFiles.begin();
    V_ManagedFilePtr::const_iterator end = listOfFiles.end();
    for( ; itr != end; ++itr )
    {
      listOfFilesPaths.push_back( (*itr)->m_Path );
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  ManagedFilePtr Manager::GetManagedFile( const tuid &id )
  {
    FILE_SCOPE_TIMER((""));

    ManagedFilePtr file = m_Resolver->GetFile( id );
    if ( file )
    {
      PrependFilePath( m_ManagedAssetsRoot, file->m_Path  );
    }
    else
    {
      // Try to find the file in the TempResolver
      Console::Print( Console::Levels::Verbose, "Searching temporary lookup table for file ID: "TUID_HEX_FORMAT"... ", id );

      file = m_TempResolver->GetFile( id );

      if ( file )
      {
        if ( file->m_Path.find( ":" ) == std::string::npos )
        {
          PrependFilePath( m_ManagedAssetsRoot, file->m_Path  );
        }

        Console::Print( Console::Levels::Verbose, "FOUND\n" );
        Console::Print( Console::Levels::Verbose, "File: %s\n", file->m_Path.c_str() );
      }
      else
      {
        Console::Print( Console::Levels::Verbose, "NOT FOUND\n" );
      }
    }

    return file;
  }

  /////////////////////////////////////////////////////////////////////////////
  ManagedFilePtr Manager::GetManagedFile( const std::string& path )
  {
    FILE_SCOPE_TIMER((""));

    std::string filePath = path;
    FileSystem::StripPrefix( m_ManagedAssetsRoot, filePath );

    ManagedFilePtr file = m_Resolver->GetFile( filePath );
    if ( file )
    {
      PrependFilePath( m_ManagedAssetsRoot, file->m_Path  );
    }
    else
    {
      // Try to find the file in the TempResolver
      Console::Print( Console::Levels::Verbose, "Searching temporary lookup table for file path: %s...", filePath.c_str() );

      file = m_TempResolver->GetFile( filePath );

      if ( file )
      {
        if ( file->m_Path.find( ":" ) == std::string::npos )
        {
          PrependFilePath( m_ManagedAssetsRoot, file->m_Path  );
        }

        Console::Print( Console::Levels::Verbose, "FOUND\n" );
        Console::Print( Console::Levels::Verbose, "File: "TUID_HEX_FORMAT"\n", file->m_Id );
      }
      else
      {
        Console::Print( Console::Levels::Verbose, "NOT FOUND\n" );
      }

    }

    return file;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Get the managed assets root
  std::string Manager::GetManagedAssetsRoot()
  {
    return m_ManagedAssetsRoot;
  }

  /////////////////////////////////////////////////////////////////////////////
  bool Manager::GetPath( const tuid &id, std::string &filePath )
  {
    FILE_SCOPE_TIMER((""));

    ManagedFilePtr file = GetManagedFile( id );

    if ( !file )
    {
      return false;
    }

    filePath = file->m_Path;
    return true;
  }

  /////////////////////////////////////////////////////////////////////////////
  std::string Manager::GetPath( const tuid &id )
  {
    std::string path;
    GetPath( id, path );
    return path;
  }

  /////////////////////////////////////////////////////////////////////////////
  tuid Manager::GetID( const std::string& path )
  {
    FILE_SCOPE_TIMER((""));

    ManagedFilePtr file = GetManagedFile( path );
    return file ? file->m_Id : TUID::Null;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns true if a file ID exits in the Resolver
  bool Manager::Contains( const tuid id, const bool getDeletedFiles )
  {
    FILE_SCOPE_TIMER((""));

    return m_Resolver->Contains( id, getDeletedFiles );
  }

  /////////////////////////////////////////////////////////////////////////////
  // Get's teh file's history from the CacheDB
  void Manager::GetFileHistory( const ManagedFilePtr& file, S_FileHistory& history )
  {
    FILE_SCOPE_TIMER((""));

    m_Resolver->GetFileHistory( file, history );
  }

  /////////////////////////////////////////////////////////////////////////////
  // Finds files by who modifed them
  void Manager::FindFilesByHistory( const std::string& searchQuery, const std::string& modifiedBy, V_ManagedFilePtr& listOfFiles, const std::string& operation, bool searchHistoryData )
  {
    FILE_SCOPE_TIMER((""));

    std::string filename = searchQuery;
    FileSystem::MakeAbsolute( filename );
    FileSystem::StripPrefix( m_ManagedAssetsRoot, filename );

    m_Resolver->FindFilesByHistory( filename, modifiedBy, listOfFiles, operation, searchHistoryData );

    V_ManagedFilePtr::iterator itr = listOfFiles.begin();
    V_ManagedFilePtr::iterator end = listOfFiles.end();
    for( ; itr != end; ++itr )
    {
      (*itr)->m_Path = m_ManagedAssetsRoot + (*itr)->m_Path;
    };
  }

  /////////////////////////////////////////////////////////////////////////////
  void Manager::GetUsernameByID( const u32 userID, std::string& username )
  {
    m_Resolver->GetUsernameByID( userID, username );
  }

  /////////////////////////////////////////////////////////////////////////////
  // 
  void Manager::LoadEventsFromTextFile( const std::string& txtFile, const std::string& eventsFile )
  {
    m_Resolver->LoadEventsFromTextFile( txtFile, eventsFile ); 
  }

  void Manager::DumpEventsToTextFile( const std::string& eventsFile, const std::string& txtFile, const std::string& filter )
  {
    m_Resolver->DumpEventsToTextFile( eventsFile, txtFile, filter ); 
  }





  /////////////////////////////////////////////////////////////////////////////
  //
  //  COPY ASSET ACROSS PROJECTS API FUNCTIONS
  //
  /////////////////////////////////////////////////////////////////////////////




  /////////////////////////////////////////////////////////////////////////////
  // Using CopyFile/RCS::Copy and DeleteFile/RCS::Delete (because MoveFile is scary)
  //
  // Note: if the file is open for add/edit it will not be reverted if the move fails
  //
  void Manager::RCSMigrate( ManagerEvent* managerEvent, bool doIt )
  {
    // early out if a transaction is open
    if ( !doIt )
    {
      m_TransManagerEvents.push_back( managerEvent );
      return;
    }

    RCS::File source( managerEvent->m_FilePath );
    source.GetInfo();

    RCS::File target( managerEvent->m_TargetFilePath );   
    target.GetInfo();

    if ( source.ExistsInDepot() && !source.HeadDeleted() )
    {
      // RCS::Copy will either integrate or it may potentially
      // do nothing if all versions have been integrated
      m_WorkingChangeset.Copy( source, target );

      // reopen the file so the user to continue modifing it after the branch
      if ( target.IsCheckedOutByMe() )
      {
        m_WorkingChangeset.Edit( target );
      }

      // if, some how, we didn't check it out - open it explicitly
      // Note: This can happen all revisions had been integrated
      //       and perforce thought it had nothing to do
      target.GetInfo();
      if ( !target.IsCheckedOutByMe() )
      {
        m_WorkingChangeset.Open( target );
      }
    }
    else
    {
      m_WorkingChangeset.Open( target );
    }

    // if the file is open for edit, force copy on disk also
    // FIXME: later checkin THEN reopen and copy new edits
    if ( ( !source.ExistsInDepot()
      || source.IsCheckedOutByMe() )
      && FileSystem::Exists( managerEvent->m_FilePath ) )
    {
      target.GetInfo();
      if ( target.IsCheckedOutByMe() )
      {
        std::string win32FilePath = managerEvent->m_FilePath;
        FileSystem::Win32Name( win32FilePath );

        std::string win32TargetFilePath = managerEvent->m_TargetFilePath;
        FileSystem::Win32Name( win32TargetFilePath );

        FileSystem::MakePath( managerEvent->m_TargetFilePath, true );
        if ( !CopyFile( win32FilePath.c_str(), win32TargetFilePath.c_str(), HasFailIfExists( managerEvent->m_ManagerConfig ) ) )
        {
          throw Windows::Exception( "Could not copy file %s to %s, Error: %s",
            managerEvent->m_FilePath.c_str(),
            managerEvent->m_TargetFilePath.c_str(),
            Windows::GetErrorString().c_str() );
        }
      }
    }

    managerEvent->m_OpCompleted = true;
  }


  /////////////////////////////////////////////////////////////////////////////
  // Called from RollbackTrans to revert a Move/Copy operation
  //
  void Manager::RCSRevertMigrate( ManagerEvent* managerEvent )
  {
    // revert the copy before rethrow
    RCS::File target( managerEvent->m_TargetFilePath );
    target.GetInfo();
    if ( target.IsCheckedOutByMe() )
    {
      target.Revert();
    }

    target.GetInfo();
    if ( !target.ExistsInDepot() )
    {
      std::string win32TargetFilePath = managerEvent->m_TargetFilePath;
      FileSystem::Win32Name( win32TargetFilePath );

      DeleteFile( win32TargetFilePath.c_str() );
    }

    managerEvent->m_OpCompleted = false;
  }


  /////////////////////////////////////////////////////////////////////////////
  // Static function for moving a ManagedFile asset from one project's File::Manager to another
  //
  // Move the file on disk and in the FileResolver, and branch it in perforce
  // Move an existing file (that exists on disk and in the File::Resolver), 
  // returns the tuid id of the File in the target Manager's Resolver
  //
  tuid Manager::Migrate( const ManagedFilePtr& sourceFile, Manager* sourceManager, const ManagerConfig managerConfig )
  {
    FILE_SCOPE_TIMER((""));

    ManagerConfig opManagerConfig = GetOperationManagerConfig( managerConfig );

    std::string sourceName = sourceFile->m_Path;

    std::string sourcePath;
    FileSystem::MakeAbsolute( sourceName, sourcePath );

    std::string sourceRelativePath = sourcePath;
    FileSystem::StripPrefix( sourceManager->GetManagedAssetsRoot(), sourceRelativePath );

    const std::string& targetRelativePath = sourceRelativePath;
    std::string targetPath = m_ManagedAssetsRoot + targetRelativePath;

    ////////////////////////////////////////////////////////////////
    // see if the file has already been migrated

    if ( HasResolver( managerConfig ) ) 
    {
      // search by ID
      ManagedFilePtr targetFile = GetManagedFile( sourceFile->m_Id );
      if ( targetFile )
      {
        // ID already exits
        if ( targetFile->m_Path != targetPath )
        {
          bool matchingExt = FileSystem::HasExtension( targetFile->m_Path, FileSystem::GetExtension( targetPath ) );
          std::string error = "Target file already exists with the given ID, but a different path.";

          // fatal if the file extensions do not match
          HandleValidationError( error, opManagerConfig, !matchingExt );
        }

        return targetFile->m_Id;
      }

      // search by path 
      targetFile = GetManagedFile( targetRelativePath );
      if ( targetFile )
      {
        // path already exits with a different ID
        std::string error = "Target file path already exists with a different ID.";
        HandleValidationError( error, opManagerConfig, true );
      }
    } 

    ////////////////////////////////////////////
    // Error check/validation
    std::string error;
    bool canMoveFile = ValidateCanMoveFile( sourcePath, targetPath, opManagerConfig, GetManagedAssetsRoot(), error, false);
    if ( !canMoveFile )
    {
      HandleValidationError( error, opManagerConfig, true );
    }

    // early out if in Preview mode
    if ( HasPreview( opManagerConfig ) )
    {
      return TUID::Null; 
    }

    //////////////////////////////////////////
    // Perform the Move

    tuid targetId = TUID::Null;

    bool isTransOpen = IsTransOpen();
    if ( !isTransOpen )
    {    
      BeginTrans( managerConfig );
    }
    try
    {
      //////////////////////////////////////////
      // File::Resolver
      if ( HasResolver( managerConfig ) )
      {
        // see if the path already exits in the target manager
        targetId = m_Resolver->GetFileID( targetRelativePath );

        if ( targetId == TUID::Null )
        {
          // add the new file and id pair to the target manager (if the path doesn't already exist)
          targetId = ResolverAddEntry( targetRelativePath, sourceFile->m_Id );
        }
      }

      //////////////////////////////////////////
      // RCS
      if ( HasRCS( managerConfig ) )
      {
        //RCSMigrate( new ManagerEvent( OperationFlags::Migrate, managerConfig, sourcePath, targetPath ) );
        RCSMove( new ManagerEvent( OperationFlags::Migrate, managerConfig, sourcePath, targetPath ) );
      }
      //////////////////////////////////////////
      // Disk
      else if ( HasDisc( managerConfig ) 
        && FileSystem::Exists( sourcePath ) )
      {
        // FIXME: this needs to happen in the transactions :(
        // Using CopyFile and DeleteFile because MoveFile is scary
        FileSystem::MakePath( targetPath, true );

        if ( !CopyFile( sourcePath.c_str(),
          targetPath.c_str(),
          HasFailIfExists( managerConfig ) ) )
        {
          std::string error = Windows::GetErrorString();
          throw Windows::Exception( "Could not copy file %s to %s, Error: %s", sourcePath.c_str(), targetPath.c_str(), error.c_str() );
        }

        // always make sure the target file is writable
        SetFileAttributes( targetPath.c_str(), FILE_ATTRIBUTE_NORMAL );
      }
    }
    catch( ... )
    {
      if ( !isTransOpen )
      {    
        RollbackTrans();
      }

      ////////////////////////////////////////
      // Disk
      // FIXME: this should happen in the transaction
      if ( !HasRCS( managerConfig ) && HasDisc( managerConfig ) )
      {
        DeleteFile( targetPath.c_str() );
      }

      throw;
    }

    if ( !isTransOpen )
    {    
      CommitTrans();
    }

    return targetId;
  }


  /////////////////////////////////////////////////////////////////////////////
  std::string ManagerConfigs::GetManagerConfigString( const u32 managerConfig )
  {
    std::string configString = "";
    if ( HasDisc( managerConfig ) )
    {
      if ( !configString.empty() )
        configString += ", ";

      configString += "Disc";
    }
    if ( HasRCS( managerConfig ) )
    {
      if ( !configString.empty() )
        configString += ", ";

      configString += "RCS";
    }
    if ( HasResolver( managerConfig ) )
    {
      if ( !configString.empty() )
        configString += ", ";

      configString += "Resolver";
    }
    if ( HasFailIfExists( managerConfig ) )
    {
      if ( !configString.empty() )
        configString += ", ";

      configString += "FailIfExists";
    }

    if ( !configString.empty() )
    {
      configString = std::string( "(" ) + configString + std::string( ")" );
    }

    return configString;
  }
}
