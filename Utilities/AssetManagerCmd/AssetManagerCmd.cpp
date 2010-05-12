#include "Windows/Windows.h"

#include "ShaderUsages.h"

#include "AppUtils/AppUtils.h"
#include "Asset/ArtFileAttribute.h"
#include "Asset/AssetClass.h"
#include "Asset/AssetInit.h"
#include "Asset/ConversionTable.h"
#include "Asset/Exceptions.h"
#include "Asset/ShaderUsagesAttribute.h"
#include "AssetManager/AssetManager.h"
#include "AssetManager/ManagedAsset.h"
#include "Attribute/Attribute.h"
#include "Attribute/AttributeHandle.h"
#include "Common/Container/Insert.h"
#include "Common/CommandLine.h"
#include "Common/Environment.h"
#include "Common/Exception.h"
#include "Common/InitializerStack.h"
#include "Common/Compiler.h"
#include "Common/String/Utilities.h"
#include "Common/Types.h"
#include "Console/Console.h"
#include "Content/ContentInit.h"
#include "EventSystem/EventSystem.h"
#include "File/ManagedFile.h"
#include "File/ManagedFileUtils.h"
#include "File/Manager.h"
#include "File/Resolver.h"
#include "FileSystem/FileSystem.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ContentSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/Finder.h"
#include "Finder/ProjectSpecs.h"
#include "Asset/EntityManifest.h"
#include "RCS/RCS.h"
#include "Reflect/Class.h"
#include "Reflect/Field.h"
#include "Symbol/SymbolInit.h"
#include "TUID/TUID.h"
#include "Windows/Process.h"
#include "Reflect/Element.h"
#include "Reflect/Registry.h"
#include "Reflect/Serializers.h"
#include "Reflect/Version.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/regex.hpp> 

#include <boost/program_options/errors.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
namespace BoostPO = boost::program_options;


using Nocturnal::Insert; 
using namespace AssetManager;

#define MAX_MATCHES 20

#define ASSET_MANAGER_OK     1
#define ASSET_MANAGER_ERROR  0

//
// Function Prototypes
//

// Parse Commandline
bool ParseProgramOptions( int argc, const char** argv );
bool CheckConflictingOpts( const V_string opts );
int  ProcessOptions( int argc );

// File::Manager Operations
int AddFile();
int MoveFileByPath( bool move = true );
int RemoveFile();

// Search Functions
int FindFiles();
int LegacyLookup();

// Shader Usages
int ListAssetsShaders();
int ListMayaFiles();
int ListStaticContentFiles();
int UsesShader();
int LoadShaderUsagesDB();

int RelocateMayaFile();


//
// Globals
//

BoostPO::variables_map          g_OptionsMap;
std::vector< BoostPO::option >  g_ExtraOptions;
V_string                        g_ExtraOptionStrs;
std::stringstream               g_UsageMsg;

File::ManagerConfig             g_ManagerConfig;
File::PrintFileFormat           g_PrintFileFormat = File::PrintFileFormats::Default;
bool                            g_IsUserAssetAdmin = false;
bool                            g_AreAssetsLocked = true;
bool                            g_Yes = false;

const boost::regex s_MatchYes("^[yY]([eE]{1}[sS]{1}){0,1}$", boost::match_single_line); 

///////////////////////////////////////////////////////////////////////////////
// Main entry point for the application.
// 
int Main( int argc, const char** argv )
{
  //
  // Startup
  //

  Nocturnal::InitializerStack initializerStack( true );
  initializerStack.Push( Finder::Initialize, Finder::Cleanup );
  initializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
  initializerStack.Push( Content::Initialize, Content::Cleanup );
  initializerStack.Push( Symbol::Initialize, Symbol::Cleanup );
  initializerStack.Push( Asset::Initialize, Asset::Cleanup );
  initializerStack.Push( Asset::ConversionTableInit, Asset::ConversionTableCleanup );
  initializerStack.Push( File::Initialize, File::Cleanup );
  initializerStack.Push( AssetManager::Initialize, AssetManager::Cleanup );
  initializerStack.Push( ShaderUsages::Initialize, ShaderUsages::Cleanup );

  //
  // Run
  //

  int result = ASSET_MANAGER_ERROR;
  //throw Nocturnal::Exception( "Testing exception reporting" );

  try
  {
    if ( ParseProgramOptions( argc, argv ) ) 
    {
      result = ProcessOptions( argc );
    }
  }
  catch( const File::DuplicateEntryException& ex )
  {
    std::stringstream idString;
    idString << ex.GetExistingID();
    Console::Error( "A file already exists with that path, file id: %s\n", idString.str().c_str() );
  }
  catch( const Nocturnal::Exception& ex )
  {
    Console::Error( "%s\n", ex.what() );
  }
  catch( const BoostPO::error& ex )
  {
    Console::Error( "%s\n", ex.what() );
  }

  Console::Print( "\n" );

  //
  // Shutdown
  //

  g_UsageMsg.clear();
  g_ExtraOptions.clear();
  g_ExtraOptionStrs.clear(); 

  Finder::Cleanup();

  result = ( result == ASSET_MANAGER_ERROR && Console::GetErrorCount() > 0 ) ? 1 : 0;

  return result;
}

int main( int argc, const char** argv )
{
  return AppUtils::StandardMain( &Main, argc, argv );
}

///////////////////////////////////////////////////////////////////////////////
// Checks that no two conflicting options are not specified on the command line.
//
bool CheckConflictingOpts( const V_string opts )
{
  V_string::const_iterator it = opts.begin();
  V_string::const_iterator itEnd = opts.end();
  for ( ; it != itEnd ; ++it )
  {
    V_string::const_iterator subIt = it;
    for ( ; subIt != itEnd ; ++subIt )
    {
      if ( (*it) == (*subIt) )
        continue;

      if ( g_OptionsMap.count( (*it) ) && !g_OptionsMap[(*it).c_str()].defaulted() &&
        g_OptionsMap.count( (*subIt) ) && !g_OptionsMap[(*subIt).c_str()].defaulted() )
      {
        Console::Error( "Only one operation can be processed at a time; found: '%s' and '%s'.", (*it).c_str(), (*subIt).c_str() );
        return false;
      }
    }
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Create and pParses command line arguments 
//
bool ParseProgramOptions( int argc, const char** argv )
{
  /////////////////////////////////////////////////////////////////////////////
  // Create options

  /////////////////////////////////////
  // File manager operations
  BoostPO::options_description operations( "Asset Management" );
  operations.add_options()
    ( "find,f"  , BoostPO::value< std::string >()                   , "find file(s) by ID or partial file path ('*' wild card)" )
    ( "add,a"   , BoostPO::value< std::vector< std::string > >()    , "add a file" )
    ( "move,m"  , BoostPO::value< std::vector< std::string > >()    , "move (rename) a file" )
    ( "copy,c"  , BoostPO::value< std::vector< std::string > >()    , "copy a file" )
    ( "delete,d", BoostPO::value< std::string >()                   , "delete a file" )
    ( "legacy,l", BoostPO::value< std::vector< std::string > >()    , "legacy asset lookup <type> <number>" )
    ( "reorg"   , BoostPO::value< std::string >()                   , "move an assets dependency files to the correct folder location" )
    ;

  /////////////////////////////////////
  // Command line flag options
  BoostPO::options_description flags( "Asset Management Flags" );
  flags.add_options()
    ( "update"    , "updates the File::Resolver Cache DB" )
    ( "preview"   , "preview the manager operation" )
    ( "idOnly"    , "only perform the operation on the tuid" )
    ( "details"   , "print the file details in output" )
    ( "barePath"  , "print the file list in bare format" )
    ( "bareID"    , "print the file list in bare format" )
    ;

  /////////////////////////////////////
  // Asset migration options
  BoostPO::options_description shaderUsages( "Shader Usage" );
  shaderUsages.add_options()
    ( "listShaders"     , BoostPO::value< std::string >(), "list the shaders the given asset uses" ) 
    ( "listMBs"         , BoostPO::value< std::string >(), "list the given asset's maya file dependencies" ) 
    ( "listStaticContent", BoostPO::value< std::string >(), "list the given asset's content irb path" )
    ( "loadShaderUsages", "loads the shader usages DB by processing over all entity files" )
    ( "usesShader"      , BoostPO::value< std::string >(), "list files that use given shader" )
    ;

  /////////////////////////////////////
  // Hidden options
  BoostPO::options_description hidden( "Hidden Options (available but not shown in release)" );
  hidden.add_options()
    ( "yes,y"        , "say yes to any prompt" )
    ( "noFail"       , "don't fail the move or copy operation if the target file exists" )
    ( "loadEvents"   , "[path] loads the user's events.dat file from .txt" )
    ( "dumpEvents"   , "[path] dumps the user's events.dat file to .txt" )
    ( "convertEvents", "converts all users' binary events file to text" )
    ( "filter"       , BoostPO::value< std::string >(),  "[filter string] filters the events dump to .txt" )
    ( "asset_admin"  , "" )
    ( "filePath"     , BoostPO::value< std::string >(), "file path" )
    ;
  BoostPO::positional_options_description filePath;
  filePath.add( "filePath" , -1 );

  /////////////////////////////////////
  // Command line flag options
  BoostPO::options_description additional_flags;
  additional_flags.add_options()
    ( "gui"     , "show the FileBrowser" )
    ( "help,h"  , "print this usage text" )
    ;

  /////////////////////////////////////
  // build the usage message
  g_UsageMsg << "\nUsage: assetmanager [flags] [operation] file-path [file-path]\n\n";
  g_UsageMsg << operations;
  g_UsageMsg << "\n";
  g_UsageMsg << flags; 
  g_UsageMsg << "\n";
  g_UsageMsg << shaderUsages;
  g_UsageMsg << "\n";
#ifdef _DEBUG
  g_UsageMsg << hidden;
  g_UsageMsg << "\n";
#endif
  g_UsageMsg << additional_flags;
  g_UsageMsg << "\n";
  g_UsageMsg << "\nExample usage:";
  g_UsageMsg << "\n\t assetmanager --find *.txt";
  g_UsageMsg << "\n\t assetmanager -f 0x2E83BF4EF0C9F30F";
  g_UsageMsg << "\n\n"; 


  /////////////////////////////////////////////////////////////////////////////
  // Parse options      

  /////////////////////////////////////
  // Add all options to commandline
  BoostPO::options_description commandline;
  commandline.add( operations ).add( flags ).add( shaderUsages ).add( additional_flags ).add( hidden );

  // see if there are unparsed options
  BoostPO::parsed_options parsedOptions = BoostPO::command_line_parser( argc, (char**)argv ).options( commandline ).positional( filePath ).allow_unregistered().run();
  g_ExtraOptions = parsedOptions.options;
  //g_ExtraOptionStrs = BoostPO::collect_unrecognized( g_ExtraOptions , BoostPO::include_positional );
  V_string extraOptionStrs = BoostPO::collect_unrecognized( g_ExtraOptions , BoostPO::include_positional );

  V_string cmdLine;
  for ( int i = 1 ; i < argc ; ++i )
  {
    if ( !extraOptionStrs.empty() )
    {
      bool found = false;
      for each ( const std::string& op in extraOptionStrs )
      {
        if ( !op.empty() && *op.begin() == '-' )
        {
          g_ExtraOptionStrs.push_back( op );
          if ( op == std::string( argv[i] ) )
          {
            found = true;
            break;
          }
        }
      }

      if ( found )
      {
        continue;
      }
    }
    cmdLine.push_back( argv[i] );    
  }

  BoostPO::store( BoostPO::command_line_parser( cmdLine ).options( commandline ).positional( filePath ).allow_unregistered().run(), g_OptionsMap );
  BoostPO::notify( g_OptionsMap );


  // Accept only one operation option in the command line
  V_string conflictingOps;
  conflictingOps.push_back( "add" );
  conflictingOps.push_back( "move" );
  conflictingOps.push_back( "copy" );
  conflictingOps.push_back( "delete" );
  conflictingOps.push_back( "find" );
  conflictingOps.push_back( "legacy" );
  conflictingOps.push_back( "reorg" );
  return CheckConflictingOpts( conflictingOps );
};

/////////////////////////////////////////////////////////////////////////////
// Process Options
int ProcessOptions( int argc )
{
  /////////////////////////////////////
  // print out the usage text
  if ( argc < 2 || g_OptionsMap.count( "help" ) )
  {
    std::cout << g_UsageMsg.str();
    return ASSET_MANAGER_OK;
  }

  /////////////////////////////////////
  // show the FileBrowser via "luna -b"
  if ( g_OptionsMap.count( "gui" ) )
  {
    std::string runFileBrowser = "luna -b";
    Windows::Execute( runFileBrowser, false, false );
    return ASSET_MANAGER_OK;
  }

  /////////////////////////////////////
  // parse the manager configs
  g_ManagerConfig = File::ManagerConfigs::Default;

  if ( g_OptionsMap.count( "idOnly" ) > 0 )
  {
    g_ManagerConfig = ( File::ManagerConfigs::Resolver );
  }

  if ( g_OptionsMap.count( "noFail" ) > 0 )
  {
    g_ManagerConfig = ( g_ManagerConfig & ~File::ManagerConfigs::FailIfExists );
  }

  if ( g_OptionsMap.count( "preview" ) > 0 )
  {
    g_ManagerConfig = ( g_ManagerConfig | File::ManagerConfigs::Preview );
  }

  g_Yes = ( g_OptionsMap.count( "yes" ) > 0 );


  if ( g_OptionsMap.count( "details" ) )
  {
    g_PrintFileFormat = File::PrintFileFormats::Verbose;
  }
  else if ( g_OptionsMap.count( "bareID" ) )
  {
    g_PrintFileFormat = File::PrintFileFormats::BareId;
  }
  else if ( g_OptionsMap.count( "barePath" ) )
  {
    g_PrintFileFormat = File::PrintFileFormats::BarePath;
  }

  /////////////////////////////////////
  // Determine asset admin settings
  g_IsUserAssetAdmin = Nocturnal::GetCmdLineFlag( "asset_admin" ) || ( g_OptionsMap.count( "asset_admin" ) > 0 );
  g_AreAssetsLocked = false;
  const char* networkLockAssetsPath = getenv( NOCTURNAL_STUDIO_PREFIX "NETWORK_LOCK_ASSETS_FILE" );
  if ( networkLockAssetsPath != NULL )
  {
    // Faster than FileSystem::Exists
    WIN32_FILE_ATTRIBUTE_DATA lockFileStats;
    if ( ::GetFileAttributesEx( networkLockAssetsPath, GetFileExInfoStandard, &lockFileStats ) )
    {
      g_AreAssetsLocked = true;
    }
    else
    {
      g_AreAssetsLocked = false;
    }
  }

  if ( g_AreAssetsLocked
    && ( g_OptionsMap.count( "move" ) || g_OptionsMap.count( "delete" ) ) )
  {
    if ( g_IsUserAssetAdmin )
    {
      Console::Warning( "Operation (%s) is currently not allowed, you are overriding this rule as an asset admin.", ( g_OptionsMap.count( "move" ) ? "move" : "detele" ) );
    }
    else
    {
      Console::Error( "Operation (%s) is currently not allowed unless performed by an asset admin.", ( g_OptionsMap.count( "move" ) ? "move" : "detele" ) );
      return ASSET_MANAGER_ERROR;
    }
  }

  /////////////////////////////////////
  // parse the operation
  if ( g_OptionsMap.count( "add" ) )
  {
    return AddFile();
  }
  //---------------------------------------------
  else if ( g_OptionsMap.count( "move" ) )
  {
    return MoveFileByPath();
  }
  //---------------------------------------------
  else if ( g_OptionsMap.count( "copy" ) )
  {
    return MoveFileByPath( false );
  }
  //---------------------------------------------
  else if ( g_OptionsMap.count( "delete" ) )
  {
    return RemoveFile();
  }
  //---------------------------------------------
  else if ( g_OptionsMap.count( "find" ) )
  {
    return FindFiles();
  }
  //---------------------------------------------
  else if ( g_OptionsMap.count( "legacy" ) )
  {
    return LegacyLookup();
  }
  //---------------------------------------------
  else if ( g_OptionsMap.count( "update" ) )
  {
    File::GlobalManager().Update();
    return ASSET_MANAGER_OK;
  }
  //---------------------------------------------
  else if ( g_OptionsMap.count( "loadEvents" ) )
  {
    std::string outPutFile;
    if ( g_OptionsMap.count( "filePath" ) )
    {
      outPutFile = g_OptionsMap["filePath"].as< std::string >();
      FileSystem::CleanName( outPutFile );
    }
    File::GlobalManager().LoadEventsFromTextFile( outPutFile );
    return ASSET_MANAGER_OK;
  }
  //---------------------------------------------
  else if ( g_OptionsMap.count( "dumpEvents" ) )
  {
    std::string outPutFile;
    if ( g_OptionsMap.count( "filePath" ) )
    {
      outPutFile = g_OptionsMap["filePath"].as< std::string >();
      FileSystem::CleanName( outPutFile );
    }
    std::string filter;
    if ( g_OptionsMap.count( "filter" ) )
    {
      filter = g_OptionsMap["filter"].as< std::string >();
    }
    File::GlobalManager().DumpEventsToTextFile( filter, outPutFile );
    return ASSET_MANAGER_OK;
  }
  //---------------------------------------------
  else if ( g_OptionsMap.count( "convertEvents" ) )
  {
    std::string eventsDir = FinderSpecs::Project::FILE_RESOLVER_FOLDER.GetFolder();
    FileSystem::AppendPath( eventsDir, FinderSpecs::Project::EVENT_SYSTEM_FOLDER.GetRelativeFolder() );
    ES::EventSystemPtr eventSystem = new ES::EventSystem( eventsDir );

    // Get the list of event record files in eventsDirPath
    u32 findFlags = ( FileSystem::FindFlags::Default | FileSystem::FindFlags::Recursive | FileSystem::FindFlags::NoSort );

    // Binary events
    V_string datEventFiles; 
    FileSystem::GetFiles( eventsDir, datEventFiles, FinderSpecs::Project::EVENTS_DAT_DECORATION.GetFilter(), findFlags );

    std::string filePath;
    ES::V_EventPtr listOfEvents;

    V_string::iterator itr = datEventFiles.begin();
    V_string::iterator end = datEventFiles.end();
    for( ; itr != end; ++itr )
    {
      filePath = (*itr);

      listOfEvents.clear();
      eventSystem->ReadEventsFile( filePath, listOfEvents, true );

      FileSystem::SetExtension( filePath, ".txt", 1 );
      eventSystem->WriteEventsFile( filePath, listOfEvents );
    }
    return ASSET_MANAGER_OK;
  }
  //---------------------------------------------
  else if ( g_OptionsMap.count( "usesShader" ) )
  {
    return UsesShader();
  }
  //---------------------------------------------
  else if ( g_OptionsMap.count( "listShaders" ) )
  {
    return ListAssetsShaders();
  }
  //---------------------------------------------
  else if ( g_OptionsMap.count( "listMBs" ) )
  {
    return ListMayaFiles();
  }
  //---------------------------------------------
  else if ( g_OptionsMap.count( "listStaticContent" ) )
  {
    return ListStaticContentFiles();
  }
  //---------------------------------------------
  else if ( g_OptionsMap.count( "loadShaderUsages" ) )
  {
    return LoadShaderUsagesDB();
  }
  //---------------------------------------------
  else if ( g_OptionsMap.count( "reorg" ) )
  {
    return RelocateMayaFile();
  }

  //---------------------------------------------
  // No options passed, show the help
  Console::Error( "Unknown option passed to AssetManager.\n" );
  std::cout << g_UsageMsg.str();
  return ASSET_MANAGER_ERROR;
}



///////////////////////////////////////////////////////////////////////////////
//------- ADD FILE
//
int AddFile()
{
  V_string addArgs = g_OptionsMap["add"].as< std::vector< std::string > >();
  std::string path = addArgs.at(0);
  FileSystem::CleanName( path );

  tuid addId = TUID::Null;
  if ( g_OptionsMap.count( "filePath" ) )
  {
    std::string assetIdStr = g_OptionsMap["filePath"].as< std::string >();

    tuid tryId = TUID::Null;
    if ( TUID::Parse( assetIdStr, tryId ) )
    {
      Console::Print( "Adding file %s, with id: "TUID_HEX_FORMAT"...\n", path.c_str(), tryId );
      addId = File::GlobalManager().Add( path.c_str(), tryId, g_ManagerConfig );
    }
    else
    {
      Console::Error( "Could not add file, invalid asset id: %s\n", assetIdStr.c_str() );
      return ASSET_MANAGER_ERROR;
    }
  }
  else
  {
    Console::Print( "Adding file %s...\n", path.c_str() );

    addId = File::GlobalManager().Add( path.c_str(), TUID::Null, g_ManagerConfig );
  }

  Console::Print( "File has been successfully added:\n\t%s \n\t"TUID_HEX_FORMAT"\n", path.c_str(), addId );
  return ASSET_MANAGER_OK;
}

///////////////////////////////////////////////////////////////////////////////
//------- MOVE FILE
//
int MoveFileByPath( bool move )
{
  std::string operation = ( move ? "move" : "copy" );

  V_string copyArgs = g_OptionsMap[operation.c_str()].as< std::vector< std::string > >();

  if ( g_OptionsMap.count( "filePath" ) )
  {
    copyArgs.push_back( g_OptionsMap["filePath"].as< std::string >() );
  }

  if ( copyArgs.size() != 2 )
  {
    Console::Error( "Two arguments are required by the %s operation.", operation.c_str() );
    return ASSET_MANAGER_ERROR;
  }

  std::string source = copyArgs.at( 0 );
  std::string target = copyArgs.at( 1 );

  FileSystem::CleanName( source );
  FileSystem::CleanName( target );

  Console::Print( "%s file... \n\tFrom:  %s \n\tTo:    %s\n", move ? "Moving" : "Copying", source.c_str(), target.c_str() );

  tuid assetId = File::GlobalManager().Move( source.c_str(), target.c_str(), g_ManagerConfig, move );

  if ( assetId != TUID::Null )
  {
    Console::Print( "ID: "TUID_HEX_FORMAT"\n", assetId );
  }  

  return ASSET_MANAGER_OK;
}

///////////////////////////////////////////////////////////////////////////////
// Deletes a file from the asset manager
//
int RemoveFile()
{
  std::string removeArg = g_OptionsMap["delete"].as< std::string >();
  FileSystem::CleanName( removeArg );

  File::V_ManagedFilePtr possibleMatches;
  File::FindManagedFiles( removeArg, possibleMatches, MAX_MATCHES );

  if ( possibleMatches.empty() )
  {
    return ASSET_MANAGER_ERROR;
  }

  // determine which asset(s) to process from the list
  File::V_ManagedFilePtr deleteFiles;
  bool yes = g_Yes;
  if ( !yes )
  {
    V_i32 assetIndicies;
    File::DetermineAssetIndicies( possibleMatches, assetIndicies, "delete" );

    if ( assetIndicies.empty() )
    {
      Console::Print( "Delete operation canceled.\n" );
      return ASSET_MANAGER_OK;
    }

    Console::Print( "\n" );
    Console::Print( "\nPlease confirm that the following %s will be deleted:\n", ( assetIndicies.size() > 1 ? "files" : "file" ) );

    V_i32::iterator itrChoice = assetIndicies.begin();
    V_i32::iterator endChoice = assetIndicies.end();
    for( ; itrChoice != endChoice ; ++itrChoice )
    {
      NOC_ASSERT( (i32) ( *itrChoice ) < (i32) possibleMatches.size() );
      Console::Print( " - %s\n", possibleMatches.at( *itrChoice )->m_Path.c_str() );
      deleteFiles.push_back( possibleMatches.at( *itrChoice ) );
    }

    // confirm that they would like to delete the file(s)
    if ( deleteFiles.size() > 1 )
    {
      Console::Print( "\n" );
      Console::Print("Are you sure you would like to DELETE all of these files? [y|n] (default: no): \n" ); 
    }
    else if ( deleteFiles.size() == 1 )
    {
      Console::Print( "\n" );
      Console::Print("Are you sure you would like to DELETE this file? [y|n] (default: no): \n" ); 
    }

    std::string yesOrNo;
    std::cin >> yesOrNo;

    boost::smatch matchResult;    
    yes = boost::regex_match( yesOrNo, matchResult, s_MatchYes );
  }
  else
  {
    deleteFiles = possibleMatches;
  }

  if ( !yes )
  {
    Console::Print( "\n" );
    Console::Print("Delete operation canceled.\n" );
    return ASSET_MANAGER_OK;
  }
  else
  {
    Console::Print( "\n" );
    Console::Print("Deleting file(s)...\n" );
    File::V_ManagedFilePtr::iterator itr = deleteFiles.begin();
    File::V_ManagedFilePtr::iterator end = deleteFiles.end();
    for ( ; itr != end ; ++itr )
    {
      const File::ManagedFilePtr& deleteFile = (*itr);

      Console::Print( " - %s...", deleteFile->m_Path.c_str() );
      File::GlobalManager().Delete( deleteFile->m_Path, g_ManagerConfig );
      Console::Print( " DELETED\n" );
    }
  }

  return ASSET_MANAGER_OK;
}

///////////////////////////////////////////////////////////////////////////////
//------- FIND FILE(S)
int FindFiles()
{
  std::string findArg = g_OptionsMap["find"].as< std::string >();
  FileSystem::CleanName( findArg );

  File::V_ManagedFilePtr listOfFiles; 
  File::FindManagedFiles( findArg, listOfFiles );

  if ( listOfFiles.empty() )
  {
    return ASSET_MANAGER_OK;
  }

  File::PrintFiles( listOfFiles, g_PrintFileFormat );
  return ASSET_MANAGER_OK;
}

///////////////////////////////////////////////////////////////////////////////
// Lookup a file by it's legacyinformation
int LegacyLookup()
{
  V_string legacyArgs = g_OptionsMap["legacy"].as< std::vector< std::string > >();

  if ( g_OptionsMap.count( "filePath" ) )
  {
    legacyArgs.push_back( g_OptionsMap["filePath"].as< std::string >() );
  }

  if ( legacyArgs.size() != 2 )
  {
    Console::Error( "Two arguments are required by the legacy lookup operation: <type> <number>" );
    return ASSET_MANAGER_ERROR;
  }

  std::string legacyType  = legacyArgs.at( 0 );
  u32 legacyNumber        = (u32) atoi( legacyArgs.at( 1 ).c_str() );

  tuid assetId = TUID::Null;
  Asset::GlobalConversionTable().FindLegacyAsset( legacyType, legacyNumber, assetId ); 

  if ( assetId == TUID::Null )
  {
    Console::Error( "Legacy asset not found!" );
    return ASSET_MANAGER_ERROR;
  }

  File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( assetId );

  File::V_ManagedFilePtr listOfFiles; 
  if( file )
  {
    listOfFiles.push_back( file ); 
  }

  if ( (int) listOfFiles.size() < 1 )
  {
    Console::Error( "No files were found that matched your search criteria.\n" );
    return ASSET_MANAGER_ERROR;
  }

  Console::Print( "%d %s found...\n", (int)listOfFiles.size(), ((int)listOfFiles.size() == 1 ? "file" : "files" ));
  File::PrintFiles( listOfFiles, g_PrintFileFormat );

  return ASSET_MANAGER_OK;
}

///////////////////////////////////////////////////////////////////////////////
// List all of the shaders that the given asset is using
//
int ListAssetsShaders()
{
  std::string assetFilePath = g_OptionsMap["listShaders"].as< std::string >();
  FileSystem::CleanName( assetFilePath );

  File::V_ManagedFilePtr shaderFiles;
  ShaderUsages::ListAssetsShaders( assetFilePath, shaderFiles );

  Console::Print( "%d %s found...\n", (int)shaderFiles.size(), ((int)shaderFiles.size() == 1 ? "file" : "files" ));
  File::PrintFiles( shaderFiles, g_PrintFileFormat );

  return ASSET_MANAGER_OK;
}

///////////////////////////////////////////////////////////////////////////////
// List all of the maya files that the given asset is using
//
int ListMayaFiles()
{
  std::string findArg = g_OptionsMap["listMBs"].as< std::string >();
  File::V_ManagedFilePtr possibleMatches;
  File::FindManagedFiles( findArg, possibleMatches );

  int whichAsset = 0;

  if ( possibleMatches.size() > 1 )
  {
    // determine which asset to build from the list
    whichAsset = File::DetermineAssetIndex( possibleMatches, "find Maya files for" );
    if ( whichAsset == -1 )
    {
      return false;
    }
  }
  else if( possibleMatches.empty() )
  {
    Console::Error( "Couldn't find a matching asset name.\n" );
    return false;
  }
  tuid assetId = possibleMatches[ whichAsset ]->m_Id;

  AssetManager::ListMayaFiles( assetId );

  return ASSET_MANAGER_OK;
}

///////////////////////////////////////////////////////////////////////////////
// List all of the maya files that the given asset is using
//
int ListStaticContentFiles()
{
  std::string findArg = g_OptionsMap["listStaticContent"].as< std::string >();
  File::V_ManagedFilePtr possibleMatches;
  File::FindManagedFiles( findArg, possibleMatches );

  int whichAsset = 0;

  if ( possibleMatches.size() > 1 )
  {
    // determine which asset to build from the list
    whichAsset = File::DetermineAssetIndex( possibleMatches, "find content.irb files for" );
    if ( whichAsset == -1 )
    {
      return false;
    }
  }
  else if( possibleMatches.empty() )
  {
    Console::Error( "Couldn't find a matching asset name.\n" );
    return false;
  }
  tuid assetId = possibleMatches[ whichAsset ]->m_Id;

  AssetManager::ListStaticContentFiles( assetId );

  return ASSET_MANAGER_OK;
}


///////////////////////////////////////////////////////////////////////////////
// Finds all assest that use the given shader
//
int UsesShader()
{
  std::string shaderFilePath = g_OptionsMap["usesShader"].as< std::string >();
  FileSystem::CleanName( shaderFilePath );

  File::V_ManagedFilePtr assetFiles;
  ShaderUsages::UsesShader( shaderFilePath, assetFiles );

  Console::Print( "%d %s found...\n", (int)assetFiles.size(), ((int)assetFiles.size() == 1 ? "file" : "files" ));
  File::PrintFiles( assetFiles, g_PrintFileFormat );

  return ASSET_MANAGER_OK;
}

///////////////////////////////////////////////////////////////////////////////
// Load the shader usages DB
//
int LoadShaderUsagesDB()
{
  ShaderUsages::LoadShaderUsagesDB();

  return ASSET_MANAGER_OK;
}


///////////////////////////////////////////////////////////////////////////////
// Relocate an asset's maya file to the correct directory
int RelocateMayaFile()
{
  std::string assetSpec = g_OptionsMap["reorg"].as< std::string >();
  FileSystem::CleanName( assetSpec );

  File::V_ManagedFilePtr possibleMatches;
  File::FindManagedFiles( assetSpec, possibleMatches, -1, FinderSpecs::Extension::REFLECT_BINARY.GetExtension() );

  if ( possibleMatches.empty() )
  {
    return ASSET_MANAGER_ERROR;
  }

  // determine which asset(s) to process from the list
  File::V_ManagedFilePtr reorganizeFiles;

  bool yes = g_Yes;
  if ( !yes )
  {
    V_i32 assetIndicies;
    File::DetermineAssetIndicies( possibleMatches, assetIndicies, "reorganize" );

    if ( assetIndicies.empty() )
    {
      Console::Print( "\n" );
      Console::Print("Reorganize asset files canceled.\n" );
      return ASSET_MANAGER_OK;
    }

    V_i32::iterator itrChoice = assetIndicies.begin();
    V_i32::iterator endChoice = assetIndicies.end();
    for( ; itrChoice != endChoice ; ++itrChoice )
    {
      NOC_ASSERT( (i32) ( *itrChoice ) < (i32) possibleMatches.size() );
      reorganizeFiles.push_back( possibleMatches.at( *itrChoice ) );
    }
  }
  else
  {
    reorganizeFiles = possibleMatches;
  }

  File::GlobalManager().BeginTrans( g_ManagerConfig, false );
  try
  { 
    M_string assetFiles;
    M_string relatedFiles;

    File::V_ManagedFilePtr::iterator itr = reorganizeFiles.begin();
    File::V_ManagedFilePtr::iterator end = reorganizeFiles.end();
    for ( ; itr != end ; ++itr )
    {
      const File::ManagedFilePtr& managedFile = (*itr);

      // Load the asset class
      if ( !FileSystem::HasExtension( managedFile->m_Path, FinderSpecs::Extension::REFLECT_BINARY.GetExtension() ) )
      {
        Console::Error( "Skipping file %s, it doesn't have asset .irb extension.\n", managedFile->m_Path.c_str() );
        continue;
      }

      Asset::AssetClassPtr assetClass = NULL;
      try
      {
        if ( ( assetClass = Asset::AssetClass::FindAssetClass( managedFile->m_Id ) ) == NULL )
        {
          Console::Error( "Skipping file %s, failed to load asset class.\n", managedFile->m_Path.c_str() );
          continue;
        }
      }
      catch( const Nocturnal::Exception& ex )
      {
        Console::Error( "Skipping file %s, failed to load asset class: %s.\n", managedFile->m_Path.c_str(), ex.what() );
        continue;
      }

      ManagedAsset managedAsset( assetClass, AssetManager::OperationFlags::Rename ) ;
      managedAsset.m_Path = managedFile->m_Path;
      managedAsset.m_NewPath = managedFile->m_Path;
      GetManagedAsset( &managedAsset, true );

      M_ManagedAssetFiles::iterator itElem = managedAsset.m_ManagedAssetFiles.begin();
      M_ManagedAssetFiles::iterator itEndElem = managedAsset.m_ManagedAssetFiles.end();
      for ( ; itElem != itEndElem ; ++itElem )
      {
        ManagedAssetFilePtr& subManagedAssetPtr = itElem->second;

        if ( ( subManagedAssetPtr->m_File->m_Id == TUID::Null )
          || ( subManagedAssetPtr->m_File->m_Path == subManagedAssetPtr->m_NewFile->m_Path )
          || ( subManagedAssetPtr->m_File->m_Path.empty() )
          || ( subManagedAssetPtr->m_NewFile->m_Path.empty() ) 
          || ( assetFiles.find( subManagedAssetPtr->m_File->m_Path ) != assetFiles.end() ) )
        {
          continue;
        }

        std::string error;
        bool canMoveFile = File::GlobalManager().ValidateCanMoveFile( subManagedAssetPtr->m_File->m_Path, subManagedAssetPtr->m_NewFile->m_Path, g_ManagerConfig, File::GlobalManager().GetManagedAssetsRoot(), error, true);
        if ( !canMoveFile )
        {
          Console::Error( "Skipping file %s, %s\n", subManagedAssetPtr->m_File->m_Path.c_str(), error.c_str() );
          continue;
        }


        if ( !yes )
        {
          /////////////////////////////////////////
          // Confirm the rename
          Console::Print( "\n" );
          Console::Print("Would you like to move this file? [y|n] (default: no):\n  o %s\n  o %s\n",
            subManagedAssetPtr->m_File->m_Path.c_str(),
            subManagedAssetPtr->m_NewFile->m_Path.c_str() ); 

          std::string yesOrNo;
          std::cin >> yesOrNo;

          boost::smatch matchResult;    
          yes = boost::regex_match( yesOrNo, matchResult, s_MatchYes );
        }

        if ( !yes )
        {
          Console::Print( "Skipping file %s...\n", managedFile->m_Path.c_str() );
          continue;
        }
        else
        {
          Console::Print( "\n" );
          Console::Print("Moving:\n  o %s\n  o %s\n",
            subManagedAssetPtr->m_File->m_Path.c_str(),
            subManagedAssetPtr->m_NewFile->m_Path.c_str() ); 

          assetFiles.insert( M_string::value_type( subManagedAssetPtr->m_File->m_Path, subManagedAssetPtr->m_NewFile->m_Path ) );
          relatedFiles.insert( subManagedAssetPtr->m_RelatedFiles.begin(), subManagedAssetPtr->m_RelatedFiles.end() );
        }
      }
    }

    File::GlobalManager().Move( assetFiles, g_ManagerConfig );

    // Copy the related files
    File::ManagerConfig relatedManagerConfig = ( g_ManagerConfig & ~File::ManagerConfigs::Resolver );
    File::GlobalManager().Move( relatedFiles, relatedManagerConfig );
  }
  catch ( const Nocturnal::Exception& e )
  {
    Console::Error( "\n" );
    Console::Print("Errors occurred while relocating asset's maya file: %s.\n", e.what() );    
    File::GlobalManager().RollbackTrans();
    return ASSET_MANAGER_ERROR;
  }
  catch ( ... )
  {
    File::GlobalManager().RollbackTrans();
    throw;
  }

  File::GlobalManager().CommitTrans();

  return ASSET_MANAGER_OK;
}
