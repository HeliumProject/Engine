#include "Windows/Windows.h"
#undef CreateEvent

#include "Resolver.h" 
#include "CacheDB.h" 

#include "AppUtils/AppUtils.h"
#include "Common/Types.h"
#include "Common/Boost/Regex.h" 
#include "Common/Memory/SmartPtr.h"
#include "Common/String/Utilities.h"
#include "Common/Environment.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/ProjectSpecs.h"
#include "Profile/Profile.h"
#include "Perforce/Perforce.h"

#define TIME_SIZE 32 // FIXME?

using namespace File;

/////////////////////////////////////////////////////////////////////////////
// Default constructor
Resolver::Resolver( )
: m_IsInitialized( false )
, m_IsTransOpen( false )
, m_TransManageEventsFile( true )
, m_EventSystem(NULL)
{
}


// Initializes the Resolver.
// Sets all of the file and directory paths, ensures trailing slash is
// place, that files are readable/writable, and that (if it's a directory)
// the directory exists.
void Resolver::Initialize( const std::string& projectRoot, const std::string& projectAssets )
{
  // Only call this function once
  NOC_ASSERT( !m_IsInitialized );

  FILE_SCOPE_TIMER((""));

  AppUtils::InitializeRevisionControl();

  DWORD nameLength = MAX_COMPUTERNAME_LENGTH + 1;
  char nameString[MAX_COMPUTERNAME_LENGTH + 1];

  // ComputerName
  if ( ::GetComputerName( nameString, &nameLength ) )
  {
    m_ComputerName = nameString;
  }
  if( m_ComputerName.empty() )
  {
    if( !Nocturnal::GetEnvVar( "COMPUTERNAME", m_ComputerName ) )
    {
      m_ComputerName = "unknown";
    }
  }
  toLower( m_ComputerName );

  // Username
  if ( ::GetUserName( nameString, &nameLength ) )
  {
    m_Username = nameString;
  }
  if ( m_Username.empty() )
  {
    if( !Nocturnal::GetEnvVar( "USERNAME", m_Username ) )
    {
      m_Username = "unknown";
    }
  }
  toLower( m_Username );

  // UserClientName
  m_UserClientName = GetUserClientName( false );

  // Set m_RootDir
  m_RootDir = projectAssets + FinderSpecs::Project::FILE_RESOLVER_FOLDER.GetRelativeFolder();
  FileSystem::GuaranteeSlash( m_RootDir );
  FileSystem::MakePath( m_RootDir );

  // Initialize the EventSystem
  std::string eventsDir = m_RootDir;
  FileSystem::AppendPath( eventsDir, FinderSpecs::Project::EVENT_SYSTEM_FOLDER.GetRelativeFolder() );
  m_EventSystem = new ES::EventSystem( eventsDir );
  m_EventSystem->CreateEventsFilePath( m_UsersEventsFile );

  std::string fileResolverConfig = FinderSpecs::Project::FILE_RESOLVER_CONFIGS.GetFolder();
  FileSystem::StripPrefix( Finder::ProjectRoot(), fileResolverConfig );

  // Initialize the CacheDB
  m_CacheDB = new CacheDB();
  m_CacheDB->Open( FinderSpecs::Project::FILE_RESOLVER_DB.GetFile( m_RootDir ),
    projectRoot + fileResolverConfig, 
    m_CacheDB->s_FileResolverDBVersion );


  if ( m_CacheDB->m_NeedsToFlushEvents )
  {
    // clean out the handled events
    m_CacheDB->DeleteHandledEvents();
    m_CacheDB->m_NeedsToFlushEvents = false;
  }

  m_IsInitialized = true;
}


/////////////////////////////////////////////////////////////////////////////
// Dtor calls Cleanup()
Resolver::~Resolver()
{
  if ( !m_IsInitialized )
    return;

  FILE_SCOPE_TIMER((""));

  m_TransUserEvents.clear();

  m_CacheDB = NULL;
  m_EventSystem = NULL;

  AppUtils::CleanupRevisionControl();

  m_IsInitialized = false;
}


/////////////////////////////////////////////////////////////////////////////
// Update the Resolver's Cache DB from the outstanding patch files
void Resolver::Update()
{
  FILE_SCOPE_TIMER((""));

  PatchCache();
}


/////////////////////////////////////////////////////////////////////////////
// Forcefully recreates the Resolver's Cache DB
void Resolver::Recreate()
{
  FILE_SCOPE_TIMER((""));

  m_CacheDB->Recreate();

  // clean out the handled events
  m_CacheDB->DeleteHandledEvents();
  m_CacheDB->m_NeedsToFlushEvents = false;

  PatchCache();
}

void Resolver::SetWorkingChangeset( const RCS::Changeset& changeset )
{
  m_Changeset = changeset;
}

//*************************************************************************//
//
//  TRANSACTION FUNCTIONS
//
//*************************************************************************//

/////////////////////////////////////////////////////////////////////////////
// Begins a new transaction; this wrapper ensures that there is only one
// open transaction at all times.
//
void Resolver::BeginTrans( bool manageEventsFile )
{
  ThrowIfMaxOpenTrans();

  m_TransUserEvents.clear();

  m_TransManageEventsFile = manageEventsFile;

  m_TransChangelistDescription.clear();

  m_CacheDB->BeginTrans();

  m_IsTransOpen = true;
}



/////////////////////////////////////////////////////////////////////////////
// Commits the existing transaction; this wrapper ensures that there is
// only one open transaction at all times.
//
void Resolver::CommitTrans()
{
  ThrowIfNoTransOpen();

  if ( !m_TransUserEvents.empty() )
  {
    // sync and open the user's events file in RCS
    RCS::File userEventsFile( m_UsersEventsFile );
    userEventsFile.GetInfo();

    if ( userEventsFile.ExistsInDepot() && !userEventsFile.IsCheckedOutByMe() && !userEventsFile.IsUpToDate())
    {
      userEventsFile.Sync();
    }

    m_Changeset.Open( userEventsFile  );

    m_EventSystem->WriteEventsFile( m_UsersEventsFile, m_TransUserEvents ); 

    if ( m_TransManageEventsFile )
    {
      userEventsFile.Commit( m_TransChangelistDescription );
      m_TransChangelistDescription.clear();
    }

    // mark the events as handled
    for each ( const ES::EventPtr& event in m_TransUserEvents )
    {
      m_CacheDB->InsertHandledEvent( event->m_Id, event->m_Created, event->m_Username, event->m_Data );
    }

    m_TransUserEvents.clear();
  }

  m_CacheDB->CommitTrans();

  m_IsTransOpen = false;
}



/////////////////////////////////////////////////////////////////////////////
// RollBack the all transactions; this wrapper ensures that there is
// only one open transaction at all times.
//
void Resolver::RollbackTrans()
{
  ThrowIfNoTransOpen();

  m_TransUserEvents.clear();

  m_TransChangelistDescription.clear();

  m_CacheDB->RollbackTrans();

  m_IsTransOpen = false;
}


//*************************************************************************//
//
//  HELPER FUNCTIONS
//
//*************************************************************************//


/////////////////////////////////////////////////////////////////////////////
// Get's this users username and computer name and caches it in 
// m_Username in the format: username@computer
// OR username-computer if isFileName is true
//
std::string Resolver::GetUserClientName( bool isFileName )
{
  std::string userClientName;

  userClientName += m_Username;
  userClientName += ( isFileName ) ? "-" : "@";
  userClientName += m_ComputerName;

  return userClientName;
}


void Resolver::GetUsernameByID( const u32 userID, std::string& username )
{
  m_CacheDB->SelectUsernameByID( userID, username );
}

//*************************************************************************//
//
//  API FUNCTIONS
//
//*************************************************************************//



/////////////////////////////////////////////////////////////////////////////
// Adds unique file paths to the Resolver DBs. 
// If the path already exists in the Resolver, AddEntry should return the
// TUID of the existing entry.
tuid Resolver::AddEntry( const std::string &filePath, const tuid tryId, bool createEvent )
{
  FILE_SCOPE_TIMER((""));

  tuid id = tryId;
  if ( id == TUID::Null )
  {
    id = TUID::Generate();
  }

  __timeb64 now;
  _ftime64_s( &now );
  u64 currentTime = ( now.time * 1000 ) + now.millitm;

  ManagedFilePtr file = new ManagedFile();
  file->m_Id       = id;
  file->m_Created  = currentTime;
  file->m_Modified = currentTime;
  file->m_Path     = filePath;
  file->m_UserId   = m_CacheDB->SelectUsernameID( m_Username );
  file->m_Username = m_Username;

  return AddEntry( file, createEvent );
}


/////////////////////////////////////////////////////////////////////////////
// Adds the patch notes to the current changelist description
inline void Resolver::AppendChangelistDescription
( 
 const char* patchOperation, 
 const std::string& filePath, 
 const tuid id, 
 const std::string& newFilePath 
 )
{
  if ( !m_TransManageEventsFile )
    return;

  std::stringstream changelistDescription;
  changelistDescription << "Adding resolver event: ";
  changelistDescription << patchOperation;
  changelistDescription << " \"" << filePath << "\" (";
  changelistDescription << TUID::HexFormat << id;
  changelistDescription << ")";

  if ( !newFilePath.empty() )
  {
    changelistDescription << " to \"" << newFilePath << "\"";
  }

  if ( !m_TransChangelistDescription.empty() )
    m_TransChangelistDescription += "; ";

  m_TransChangelistDescription += changelistDescription.str();
}


/////////////////////////////////////////////////////////////////////////////
// Adds unique file paths to the Resolver DBs. 
// If the path already exists in the Resolver, AddEntry should return the
// TUID of the existing entry.
tuid Resolver::AddEntry( ManagedFilePtr& file, bool createEvent )
{
  FILE_SCOPE_TIMER((""));

  // insert into the DB and create an event
  bool isTransOpen = IsTransOpen();
  if ( !isTransOpen )
  {
    BeginTrans();
  }

  try
  {
    if ( file->m_UserId == 0 )
    {
      file->m_UserId = m_CacheDB->SelectUsernameID( file->m_Username );
    }

    file->m_Id = m_CacheDB->InsertFile( file->m_Id, file->m_Created, file->m_Modified, file->m_Path, file->m_UserId );

    if ( createEvent )
    {
      CreatePatchRecord( (int) PatchOperations::Insert, file->m_Id, file->m_Created, file->m_Created, file->m_Path );
      AppendChangelistDescription( "Inserting", file->m_Path, file->m_Id );
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

  return file->m_Id;
}


/////////////////////////////////////////////////////////////////////////////
// Updates the entry in the Resolver DBs as a single transaction;
// such that if any of the updates fails, the process can be rolled back.
tuid Resolver::UpdateEntry( const ManagedFilePtr& file, const std::string &newFilePath )
{
  FILE_SCOPE_TIMER((""));

  tuid id = file->m_Id;
  if ( id == TUID::Null )
  {
    throw Exception( "Cannot update file with null TUID, file path: %s", newFilePath.c_str() );
  }

  __timeb64 now;
  _ftime64_s( &now );
  u64 modifiedTime = ( now.time * 1000 ) + now.millitm;


  bool isTransOpen = IsTransOpen();
  if ( !isTransOpen )
  {
    BeginTrans();
  }

  try
  {
    id = m_CacheDB->UpdateFile( file->m_Id, modifiedTime, newFilePath, m_CacheDB->SelectUsernameID( m_Username )  );
    CreatePatchRecord( (int) PatchOperations::Update, file->m_Id, file->m_Created, modifiedTime, newFilePath );

    AppendChangelistDescription( "Updating", file->m_Path, file->m_Id, newFilePath );

    file->m_Modified = modifiedTime;
    file->m_Path = newFilePath;
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



  return id;
}


/////////////////////////////////////////////////////////////////////////////
// Removes an entry from the Resolver DBs
void Resolver::DeleteEntry( const ManagedFilePtr& file )
{
  FILE_SCOPE_TIMER((""));

  __timeb64 now;
  _ftime64_s( &now );
  u64 modifiedTime = ( now.time * 1000 ) + now.millitm;

  bool isTransOpen = IsTransOpen();
  if ( !isTransOpen )
  {
    BeginTrans();
  }

  try
  {
    m_CacheDB->MarkFileDeleted( file->m_Id, modifiedTime, m_CacheDB->SelectUsernameID( m_Username )  );
    CreatePatchRecord( (int) PatchOperations::Delete, file->m_Id, file->m_Created, modifiedTime, file->m_Path );

    AppendChangelistDescription( "Deleting", file->m_Path, file->m_Id );

    file->m_Modified = modifiedTime;
    file->m_WasDeleted = true;
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
// Finds the file entry in the Cache DB and calls MarkFileDeleted( File )
void Resolver::DeleteEntry( const tuid id )
{
  FILE_SCOPE_TIMER((""));

  ManagedFilePtr file = GetFile( id );
  if ( file )
  {
    DeleteEntry( file );
  }
}


//*************************************************************************//
//
//  SEARCH API FUNCTIONS
//
//*************************************************************************//


/////////////////////////////////////////////////////////////////////////////
// Get's a list of filePath/tuid pairs, given the file path; populating
// the listOfFiles argument.
void Resolver::FindFilesByPath( const std::string& searchQuery, V_ManagedFilePtr &listOfFiles, bool getOneRow )
{
  FILE_SCOPE_TIMER((""));

  m_CacheDB->SelectListOfFilesByPath( searchQuery, listOfFiles, getOneRow );
}


/////////////////////////////////////////////////////////////////////////////
// Get's a ManagedFilePtr, given the file path
ManagedFilePtr Resolver::GetFile( const std::string& filePath )
{
  FILE_SCOPE_TIMER((""));

  ManagedFilePtr file = m_CacheDB->SelectFileByPath( filePath );

  return file;
}

/////////////////////////////////////////////////////////////////////////////
// Populate the ManagedFile instance with entry data from the Cache DB
ManagedFilePtr Resolver::GetFile( const tuid id )
{
  FILE_SCOPE_TIMER((""));

  ManagedFilePtr file = m_CacheDB->SelectFileByID( id );

  return file;
}


/////////////////////////////////////////////////////////////////////////////
// Get's the TUID of an entry, given its file path
tuid Resolver::GetFileID( const std::string &filePath )
{
  FILE_SCOPE_TIMER((""));

  ManagedFilePtr file = GetFile( filePath );
  if ( file )
  {
    return file->m_Id;
  }

  return TUID::Null;
}


/////////////////////////////////////////////////////////////////////////////
// Get's the file path of an entry, given the TUID
void Resolver::GetFilePath( const tuid id, std::string &filePath )
{
  FILE_SCOPE_TIMER((""));

  filePath = "";

  ManagedFilePtr file = GetFile( id );
  filePath = file->m_Path;
}


/////////////////////////////////////////////////////////////////////////////
// Get's the file path of an entry, given the TUID
bool Resolver::Contains( const tuid id, const bool getDeletedFiles )
{
  FILE_SCOPE_TIMER((""));

  return m_CacheDB->Contains( id, getDeletedFiles );
}



/////////////////////////////////////////////////////////////////////////////
// Get's teh file's history from the CacheDB
void Resolver::GetFileHistory( const ManagedFilePtr& file, S_FileHistory& history )
{
  FILE_SCOPE_TIMER((""));

  m_CacheDB->SelectFileHistory( file, history );
}


/////////////////////////////////////////////////////////////////////////////
// Finds files by who modifed them
void Resolver::FindFilesByHistory
( 
 const std::string& searchQuery, 
 const std::string& modifiedBy, 
 V_ManagedFilePtr &listOfFiles, 
 const std::string& operation,
 bool searchHistoryData
 )
{
  FILE_SCOPE_TIMER((""));

  m_CacheDB->SelectFilesByHistory( searchQuery, modifiedBy, listOfFiles, operation, searchHistoryData );
}

/////////////////////////////////////////////////////////////////////////////
// Creates a DB patch file (or entry to an existing file).
void Resolver::CreatePatchRecord
(
 const int operation,
 const tuid id,
 const u64 creationTime,
 const u64 modifiedTime,
 const std::string& filePath
 )
{
  FILE_SCOPE_TIMER((""));

  std::stringstream eventData;
  eventData << operation << "|" << creationTime << "|" << modifiedTime << "|" << id << "|" << filePath;

  m_TransUserEvents.push_back( m_EventSystem->CreateEvent( eventData.str(), m_UserClientName ) );
}



/////////////////////////////////////////////////////////////////////////////
// Parse, apply and handle outstanding patches to the Cache DB, ordered by patch
void Resolver::PatchCache()
{
  FILE_SCOPE_TIMER((""));

  S_tuid eventIDs;
  m_CacheDB->SelectHandledEvents( eventIDs );

  ES::V_EventPtr listOfEvents;
  m_EventSystem->GetUnhandledEvents( listOfEvents, eventIDs );   

  bool isTransOpen = IsTransOpen();
  if ( !isTransOpen )
  {
    BeginTrans();
  }

  try
  {
    // apply the patch records and add them to the handle events list
    for each ( const ES::EventPtr& event in listOfEvents )
    {
      if ( ApplyPatchRecord( event ) )
      {        
        m_CacheDB->InsertHandledEvent( event->m_Id, event->m_Created, event->m_Username, event->m_Data );
      }
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
// Only a subset of the Resolver PatchOperations are valid event operations
//
bool inline IsValidPatchOperation( PatchOperation operation )
{
  return ( ( operation == PatchOperations::Insert )
    || ( operation == PatchOperations::Update )
    || ( operation == PatchOperations::Delete ) );
}

/////////////////////////////////////////////////////////////////////////////
void Resolver::ParseEvent( const ES::Event* event, Resolver::PatchRecord& output )
{
  FILE_SCOPE_TIMER((""));

  ParseEventData( event->m_Data, output ); 

  // get the absolute path to the file
  //std::string assetAbsolutePath = Finder::ProjectAssets() + output.m_FilePath;

  // get the username and computer name
  output.m_FileUsername = event->m_Username;
  //output.m_FileComputer = event->m_Username;

  std::string::size_type atPos = event->m_Username.rfind( '@' );
  if ( atPos != std::string::npos )
  {
    output.m_FileUsername.erase( atPos );
    
    //if ( ( atPos + 1 ) < event->m_Username.size() )
    //{
    //  output.m_FileComputer.erase( 0, atPos + 1 );
    //}
  }
}

/////////////////////////////////////////////////////////////////////////////
// will throw BadPatchRecordException if the string is not formatted properly
// will throw BatPatchOpException if we get a bad operation
// 
// otherwise will fill out the output structure
// 
void Resolver::ParseEventData( const std::string& patchRecord, Resolver::PatchRecord& output )
{
  FILE_SCOPE_TIMER((""));

  // Used by ApplyPatchRecord to parse the pipe '|' delimited event data with format:
  // <int operator>|<i64 created>|<i64 modified>|<i64 id>|<string filePath>
  const boost::regex s_EventDataPattern( "^(\\d+)\\|(\\d*)\\|(\\d*)\\|(\\d+)\\|(.*?)$" );

  // if we don't match, this record is bad
  boost::cmatch  resultAttributes; 
  if (!boost::regex_match(patchRecord.c_str(), resultAttributes, s_EventDataPattern))
  {
    throw BadPatchRecordException( patchRecord.c_str() ); 
  }

  // get the action based on the operation
  output.m_Operation = (PatchOperation) ResultAsInt(resultAttributes, 1); 

  // ERR: BAD OPERATION
  if ( !IsValidPatchOperation( output.m_Operation ) )
  {
    throw BadPatchOpException( patchRecord.c_str() );
  }

  output.m_FileCreated  = ( u64 )  ResultAsU64(resultAttributes, 2); 
  output.m_FileModified = ( u64 )  ResultAsU64(resultAttributes, 3); 
  output.m_FileId       = ( tuid ) ResultAsU64(resultAttributes, 4); 
  output.m_FilePath =          ResultAsString(resultAttributes, 5); 

}


/////////////////////////////////////////////////////////////////////////////
// Parses the eventData string and fills out the PatchRecord
//
// Will throw BadPatchRecordException or BadPatchOpException
// 
void Resolver::ParseTextFileData(const std::string& eventData, Resolver::PatchRecord& output)
{
  FILE_SCOPE_TIMER((""));

  /////////////////////////////////////////////
  // parse the data string
  // Data:  <string operation>|<i64 created>|<i64 modified>|<string hexId>|<string filePath>
  const boost::regex s_EventTextFileDataPattern( "^Data\\:  (.+?)\\|(\\d*)\\|(\\d*)\\|(.+?)\\|(.*?)$" );

  // if we don't match, this record is bad
  boost::cmatch  dataResultAttr; 
  if ( !boost::regex_match(eventData.c_str(), dataResultAttr, s_EventTextFileDataPattern))
  {
    throw BadPatchRecordException( eventData.c_str() ); 
  }

  // get the action based on the operation
  output.m_Operation  = ( PatchOperation ) GetPatchOperation( ResultAsString(dataResultAttr, 1) ); 

  // ERR: BAD OPERATION
  if ( !IsValidPatchOperation( output.m_Operation ) )
  {
    throw BadPatchOpException( eventData.c_str() );
  }

  // parse the rest of the patch file
  output.m_FileCreated          = ( u64 ) ResultAsU64(dataResultAttr, 2); 
  output.m_FileModified         = ( u64 ) ResultAsU64(dataResultAttr, 3); 

  output.m_FileId = TUID::Null;
  {
    std::string assetIdStr    = ResultAsString(dataResultAttr, 4); 
    std::istringstream stream ( assetIdStr );

    if ( assetIdStr.size() > 2 
      && assetIdStr[0] == '0'
      && assetIdStr[1] == 'x' )
    {
      stream >> std::hex >> output.m_FileId;
    }
    else
    {
      stream >> output.m_FileId;
    }
  }

  output.m_FilePath = ResultAsString(dataResultAttr, 5); 
}


/////////////////////////////////////////////////////////////////////////////
// Parse and apply a single patch record line.
bool Resolver::ApplyPatchRecord( ES::Event* event )
{
  FILE_SCOPE_TIMER((""));

  PatchRecord parsed; 
  ParseEvent( event, parsed );

  u32 userID = m_CacheDB->SelectUsernameID( parsed.m_FileUsername );

  try
  {
    switch( parsed.m_Operation )
    {
      // INSERT
    case PatchOperations::Insert:
      m_CacheDB->InsertFile( parsed.m_FileId, parsed.m_FileCreated, parsed.m_FileModified, parsed.m_FilePath, userID );
      return true;
      break;

      // UPDATE
    case PatchOperations::Update:
      m_CacheDB->UpdateFile( parsed.m_FileId, parsed.m_FileModified, parsed.m_FilePath, userID );
      return true;
      break;

      // DELETE
    case PatchOperations::Delete:
      m_CacheDB->MarkFileDeleted( parsed.m_FileId, parsed.m_FileModified, userID );
      return true;
      break;
    }
  }
  catch( const DuplicateEntryException &ex )
  {
    if ( ex.GetExistingID() != parsed.m_FileId )
    {
      throw DuplicatePatchEntryException( ex.GetExistingID(), parsed.m_FilePath.c_str() );
    }

    return true;
  }

  return false;
}


/////////////////////////////////////////////////////////////////////////////
// Dumps the event.dat file to a human readable event.txt file
//
void Resolver::DumpEventsToTextFile( const std::string& eventsFile, const std::string& txtFile, const std::string& filter )
{
  // setup the input file
  std::string eventsFilename = !eventsFile.empty() ? eventsFile : m_UsersEventsFile ;

  if ( !FileSystem::Exists( eventsFilename ) )
  {
    throw Exception( "Input events file [%s] doesn't exist!", eventsFilename.c_str() );
  }

  // set up the output file
  std::string txtFilename = txtFile;
  if ( txtFilename.empty() )
  {
    txtFilename = eventsFilename;
    FileSystem::SetExtension( txtFilename, ".txt", 1 );
  }
  FileSystem::MakePath( txtFilename, true );

  if ( FileSystem::Exists( txtFilename )
    && FileSystem::HasAttribute( txtFilename, FILE_ATTRIBUTE_READONLY ) )
  {
    throw Exception( "Output file [%s] is read-only!", txtFilename.c_str() );
  }

  // get this user's events
  ES::V_EventPtr listOfEvents;
  m_EventSystem->ReadEventsFile( eventsFilename, listOfEvents, true );

  if ( !filter.empty() )
  {
    int recordsFiltered = 0;

    ES::V_EventPtr filteredEvents;

    ES::V_EventPtr::iterator itr = listOfEvents.begin();
    ES::V_EventPtr::iterator end = listOfEvents.end();
    for ( ; itr != end; ++itr )
    {
      const ES::EventPtr& event = (*itr);

      // parse the data string
      PatchRecord parsed; 
      ParseEventData( event->m_Data, parsed ); 

      if ( parsed.m_FilePath.find( filter ) != std::string::npos )
      {
        ++recordsFiltered;
        continue;
      }
      else
      {
        filteredEvents.push_back( event );
      }
    }

    Console::Print( "%d event records removed by filter: %s\n", recordsFiltered, filter );

    m_EventSystem->WriteEventsFile( txtFilename, filteredEvents );
  }
  else
  {
    m_EventSystem->WriteEventsFile( txtFilename, listOfEvents );
  }
}


/////////////////////////////////////////////////////////////////////////////
// Loads the event.dat file from the event.txt file
// 
void Resolver::LoadEventsFromTextFile( const std::string& txtFile, const std::string& eventsFile )
{
  // Used by LoadEventsFromTextFile to parse the pipe '|' delimited lines:
  // Event: <string created>|<i64 id>|<i64 created>|<string username>
  //const boost::regex s_EventTextFileStringPattern( "^Event\\: .*?\\|(\\d+)\\|(\\d*)\\|(.*?)$" );

  // set up the input file
  std::string txtFilename = txtFile;
  if ( txtFilename.empty() )
  {
    txtFilename = m_UsersEventsFile;
    txtFilename += std::string( ".txt" );
  }

  if ( !FileSystem::Exists( txtFilename ) )
  {
    throw Exception( "Input text events file [%s] doesn't exist!", txtFilename.c_str() );
  }

  m_EventSystem->LoadEventsFromTextFile( txtFilename, eventsFile );

  Recreate();
}
