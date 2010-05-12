#include "ManagedFileUtils.h"
#include "Manager.h"
#include "Resolver.h"

#include "Common/Boost/Regex.h"
#include "Common/String/Tokenize.h"
#include "Common/Types.h"
#include "Console/Console.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"

#include <boost/regex.hpp> 

using namespace File;

#define MAX_REQUEST_INPUT   5

#define INVALID_ASSET_INDEX -1
#define SELECT_ALL_CHAR     "#"
#define EXIT_CHAR           "!"

#define TIME_SIZE 32

///////////////////////////////////////////////////////////////////////////////
// Pretty print a file to the console
//
// *** WARNING ***
// The buildserver currecnly parses the output from this function, 
// DO NOT change the format without talking to Giac (or at least understanding 
// what needs to be fixed).
//
void File::PrintFile( const File::ManagedFilePtr& file, File::PrintFileFormat printformat )
{
  NOC_ASSERT(file); 
  if(!file)
  {
    return; 
  }

  if ( printformat == File::PrintFileFormats::BarePath )
  {
    Console::Print( "%s\n", file->m_Path.c_str() );
    return;
  }
  else if ( printformat == File::PrintFileFormats::BareId )
  {
    Console::Print( TUID_HEX_FORMAT"\n", file->m_Id );
    return;
  }
  else if ( printformat == File::PrintFileFormats::Default )
  {
    Console::Print( "  o %s ("TUID_HEX_FORMAT")\n", file->m_Path.c_str(), file->m_Id );
    return;
  }
  else if ( printformat == File::PrintFileFormats::Verbose )
  {
    Console::Print( "File:     %s\n", file->m_Path.c_str() );
    Console::Print( "ID:       "TUID_HEX_FORMAT"\n", file->m_Id );

    __time64_t created  = ( __time64_t ) ( file->m_Created / 1000 );
    __time64_t modified = ( __time64_t ) ( file->m_Modified / 1000 );

    char createdPrint[TIME_SIZE];
    char modifiedPrint[TIME_SIZE];

    // try to get a printer friendly version of the dates
    if ( ( _ctime64_s( createdPrint, TIME_SIZE, &created ) == 0 ) &&
      ( _ctime64_s( modifiedPrint, TIME_SIZE, &modified ) == 0 ) )
    {
      Console::Print( "Created:  %s\n", createdPrint );
      Console::Print( "Modified: %s\n", modifiedPrint );
    }
    else
    {
      Console::Print( "Created:  %I64u\n", created );
      Console::Print( "Modified: %I64u\n", modified );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// Pretty print a list of files to the console
//
void File::PrintFiles( File::V_ManagedFilePtr& listOfFiles, File::PrintFileFormat printformat )
{
  if ( (int) listOfFiles.size() < 1 )
  {
    return;
  }

  File::SortFiles( listOfFiles );

  for each ( const File::ManagedFilePtr& file in listOfFiles )
  {
    File::PrintFile( file, printformat );
  }
}


/////////////////////////////////////////////////////////////////////////////
bool SortFilesAlphabetical( const File::ManagedFilePtr& lhs, const File::ManagedFilePtr& rhs )
{
  return FileSystem::CompareFilePath( lhs->m_Path, rhs->m_Path );
}

bool SortFilesByID( const File::ManagedFilePtr& rhs, const File::ManagedFilePtr& lhs )
{
  return rhs->m_Id < lhs->m_Id;
}

bool SortFilesByCreatedBy( const File::ManagedFilePtr& rhs, const File::ManagedFilePtr& lhs )
{
  return rhs->m_Created < lhs->m_Created;
}

bool SortFilesByModifiedBy( const File::ManagedFilePtr& rhs, const File::ManagedFilePtr& lhs )
{
  return rhs->m_Modified < lhs->m_Modified;
}

void File::SortFiles( V_ManagedFilePtr &listOfFiles, SortFileMethod method, bool reverse )
{
  switch( method )
  {
  default:
  case SortFileMethods::Alphabetical:
    std::sort( listOfFiles.begin(), listOfFiles.end(), SortFilesAlphabetical );
    break;
  
  case SortFileMethods::ID:
    std::sort( listOfFiles.begin(), listOfFiles.end(), SortFilesByID );
    break;
  
  case SortFileMethods::CreatedBy:
    std::sort( listOfFiles.begin(), listOfFiles.end(), SortFilesByCreatedBy );
    break;  

  case SortFileMethods::ModifiedBy:
    std::sort( listOfFiles.begin(), listOfFiles.end(), SortFilesByModifiedBy );
    break; 
  }

  if ( reverse )
  {
    std::reverse( listOfFiles.begin(), listOfFiles.end() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Find managed files from a string that can be one of:
//  - hex TUID:       0xBCF58FFCFEF9541C
//  - numeric tuid:   13615947365196846108
//  - partial path:   fragrubbletrench.en
//                    chicago/start_area_alley/fragments/fragrubbletrench
//                    x:/r2/assets/devel/entities/unique/chicago/start_area_alley
//  - full path:      x:/r2/assets/devel/entities/unique/chicago/start_area_alley/fragments/fragrubbletrench/fragrubbletrench.entity.irb
//
void File::FindManagedFiles( const std::string& searchQuery, File::V_ManagedFilePtr& listOfFiles, const int maxMatches, const std::string& extension , bool verbose )
{
  tuid id = TUID::Null;
  if ( TUID::Parse( searchQuery, id ) )
  {  
    if ( verbose )
    {
      Console::Print( "Searching for a file with the ID: "TUID_HEX_FORMAT"...\n", id );
    }

    File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( id );

    if( file )
    {
      listOfFiles.push_back( file ); 
    }
  }

  // if it's not a TUID string or, for whatever reason,
  // we didn't find a file above, search for it by path
  if ( listOfFiles.empty() )
  { 
    std::string assetSpec = searchQuery;

    if ( !extension.empty() 
      && ( !FileSystem::HasExtension( assetSpec, extension ) ) )
    {
      // wildcard the back and apply the extension
      if ( *assetSpec.rbegin() != '*' )
      {
        assetSpec += "*";
      }

      assetSpec += extension;
    }

    if ( verbose )
    {
      Console::Print( "Searching for files that match the pattern: %s...\n", assetSpec.c_str() );
    }

    File::GlobalManager().Find( assetSpec, listOfFiles );    
  }

  if ( listOfFiles.empty() )
  {
    Console::Error( "Could not locate any assets that match the criteria: %s\n", searchQuery.c_str() );
    return;
  }
  else if ( maxMatches == 1 && listOfFiles.size() > 1 )
  {
    Console::Error( "Found too many matches (%d) when using the \"-nomultiple flag\"! \nPlease provide more specific search criteria so that only one match exists.\n", (i32) listOfFiles.size() );
    listOfFiles.clear();
    return;
  }
  else if ( maxMatches > 1 && (int) listOfFiles.size() > maxMatches )
  {
    Console::Error( "Found too many matches (%d)! Please provide more specific search criteria.\n", (i32) listOfFiles.size() );
    listOfFiles.clear();
    return;
  } 
  else
  {
    if ( verbose )
    {
      Console::Print( "Found %d %s.\n", listOfFiles.size(), listOfFiles.size() > 1 ? "matches" : "match" );
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
// Let the user chose which asset they would like to use.
//
int File::DetermineAssetIndex( const File::V_ManagedFilePtr& listOfFiles, const char* operation )
{
  V_i32 indicies;
  DetermineAssetIndicies( listOfFiles, indicies, operation, true );
  return ( indicies.empty() ? INVALID_ASSET_INDEX: indicies.at( 0 ) );
}


bool ValidateAndInsertIndex( const int index, const int numAssets, const File::V_ManagedFilePtr& listOfFiles, V_i32& indicies )
{
  if ( index > 0 && index <= numAssets )
  {
    if ( FileSystem::Exists( listOfFiles.at( index - 1 )->m_Path ) )
    {
      indicies.push_back( index - 1 );
      return true;
    }

    Console::Error( "Asset file number \"%d\" (\"%s\") does not exist on disk, please select a different asset or run getassets and try again.  ", index, listOfFiles.at( index - 1 )->m_Path.c_str() );
  }
  else
  {
    Console::Error( "Invalid asset number \"%d\", please try again. ", index );
  }

  return false;
}


bool File::DetermineAssetIndicies( const File::V_ManagedFilePtr& listOfFiles, V_i32& indicies, const char* operation, bool selectOne )
{
  if ( listOfFiles.size() < 1 )
  {
    return false;
  }

  if ( listOfFiles.size() == 1 )
  {
    indicies.push_back( 0 );
    return true;
  }

  Console::Print( "More than one asset matched your search query, please select the %s you'd like to %s:\n\n", ( selectOne ? "asset" : "asset(s)" ), operation ); 

  int numAssets = 0;

  File::V_ManagedFilePtr::const_iterator it    = listOfFiles.begin();
  File::V_ManagedFilePtr::const_iterator itEnd = listOfFiles.end();
  for( ; it != itEnd ; ++it )
  {
    //if ( FileSystem::Exists( (*it)->m_Path ) )
    //{
      Console::Print( "%3d: %s\n", ++numAssets, (*it)->m_Path.c_str() ); 
    //}
    //else
    //{
    //  Console::Print( Console::Colors::Yellow, "%3d: (DOESN'T EXIST ON DISK) %s\n", ++numAssets, (*it)->m_Path.c_str() );
    //}
  }

  if ( !selectOne && numAssets > 1 )
  {
    Console::Print( "  "SELECT_ALL_CHAR": %s\n\n", "Select All" );
  }
  
  Console::Print( "Enter the asset %s you'd like to %s (%sor '"EXIT_CHAR"' to exit ):  ", 
    ( selectOne ? "index" : "indixcies" ),
    operation,
    ( selectOne ? "" : SELECT_ALL_CHAR" to select all, " ) ); 
  std::string selection;
  
  // loop, until they choose something valid or MAX_REQUEST_INPUT
  bool doneSelecting = false;
  int recurse = 0;
  while( !doneSelecting && recurse < MAX_REQUEST_INPUT )
  {
    std::cin >> selection;
    Console::Print( "" );
    indicies.clear();

    // Exit
    if ( selection.empty() || selection.find( EXIT_CHAR ) != std::string::npos )
    {
      Console::Print( "No asset(s) selected.\n" );
      doneSelecting = true;
      break;
    }
    // Select One
    else if ( selectOne )
    {
      int assetChoice = 0;
      assetChoice = atoi( selection.c_str() );

      if ( ValidateAndInsertIndex( assetChoice, numAssets, listOfFiles, indicies ) )
      {
        doneSelecting = true;
        break;
      }
    }
    // Select All
    else if ( selection.find( SELECT_ALL_CHAR ) != std::string::npos )
    {
      for ( int i = 0; i < numAssets ; ++i ) 
      {
        indicies.push_back( i );
      }
     
      doneSelecting = true;
      break;
    }
    else
    {
      S_i32 assetChoices;

      // split on spaces or commas
      S_string selections;
      Tokenize( selection, selections, " ,;" );
      
      bool errorOccurred = false;
      // expand ranges
      boost::cmatch matchResult;
      const boost::regex s_IndexRangePattern( "^\\s*(\\d+)\\s*\\-\\s*(\\d+)\\s*$" );
      
      S_string::iterator itr = selections.begin();
      S_string::iterator end = selections.end();
      for( ; itr != end ; ++itr )
      {
        if ( boost::regex_match( (*itr).c_str(), matchResult, s_IndexRangePattern ) )
        {
          int beginRange = ResultAsInt( matchResult, 1 ); 
          int endRange = ResultAsInt( matchResult, 2 ); 

          if ( beginRange <= endRange )
          {
            Console::Error( "Invalid range: %s\n", (*itr).c_str() );
            errorOccurred = true;
            break;
          }

          for ( ; beginRange <= endRange ; ++beginRange )
          {
            assetChoices.insert( beginRange );
          }
        }
        else
        {
          int assetChoice = 0;
          assetChoice = atoi( (*itr).c_str() );

          if ( assetChoice <= 0 )
          {
            Console::Error( "Invalid range: %s\n", (*itr).c_str() );
            errorOccurred = true;
            break;
          }

          assetChoices.insert( assetChoice );
        }
      }

      if ( !errorOccurred )
      {
        // insert selections into vector
        S_i32::iterator itrChoice = assetChoices.begin();
        S_i32::iterator endChoice = assetChoices.end();
        for( ; itrChoice != endChoice ; ++itrChoice )
        {
          if ( !ValidateAndInsertIndex( (*itrChoice), numAssets, listOfFiles, indicies ) )
          {
            break;
          }
        }

        doneSelecting = true;
      }
    }


    ++recurse;
  }

  return !indicies.empty();
}
