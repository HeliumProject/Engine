#include "Precompile.h"
#include "BuildServer.h"

#include "Foundation/Environment.h"
#include "Foundation/Boost/Regex.h"
#include "Foundation/Log.h"
#include "FileSystem/FileSystem.h"

#include <fstream>

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// 
// 
BuildServer::BuildServer()
{
  if ( !Nocturnal::GetEnvVar( "NOC_BUILDSERVER_BUILDS", m_BuildLocation ) )
  {
    Log::Error( "Failed to find environment variable IG_BUILDSERVER_BUILDS.\n" );
  }
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
BuildServer::~BuildServer()
{
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
const BuildInfo* BuildServer::FindBuild( const std::string& name ) const
{
  BuildInfo* found = NULL;
  M_Builds::const_iterator itr = m_Builds.find( name );
  if ( itr != m_Builds.end() )
  {
    found = itr->second;
  }
  return found;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
const M_Builds& BuildServer::GetBuilds() const
{
  return m_Builds;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void BuildServer::RefreshBuilds()
{
  m_Builds.clear();

  V_string directories;
  if ( FileSystem::GetDirectories( m_BuildLocation, directories ) > 0 )
  {
    struct _stati64 fileStats;
    V_string::const_iterator dirItr = directories.begin();
    V_string::const_iterator dirEnd = directories.end();
    for ( ; dirItr != dirEnd; ++dirItr )
    {
      const std::string& directory = *dirItr;

      BuildInfoPtr build = new BuildInfo( FileSystem::GetLeaf( directory ) );

      // Build time 
      if ( FileSystem::GetStats64( directory, fileStats ) )
      {
        build->m_Timestamp = fileStats.st_mtime;
      }

      // Build status
      std::string state( directory );
      FileSystem::AppendPath( state, "state" );
      if ( FileSystem::Exists( state ) )
      {
        std::string code( state );
        FileSystem::AppendPath( code, "code_game_done.txt" );
        build->m_CodeSucceeded = FileSystem::Exists( code );

        std::string symbols( state );
        FileSystem::AppendPath( symbols, "xml_done.txt" );
        build->m_SymbolsSucceeded = FileSystem::Exists( symbols );
      }

      // Build config settings
      std::string config( directory );
      FileSystem::AppendPath( config, "config.xml" );
      if ( FileSystem::Exists( config ) )
      {
        const boost::regex templatePattern( "build_template\\s*=\\s*\"(.*?)\"" );
        const boost::regex levelPattern( "<level>(.+)</level>" );
        std::fstream file( config.c_str(), std::ios_base::in );
        while ( !file.fail() )
        {
          std::string line;
          std::getline( file, line );
          boost::smatch results;
          if ( boost::regex_search( line, results, templatePattern ) )
          {
            // Template
            if ( results[1].matched )
            {
              build->m_Template = Nocturnal::BoostMatchResultAsString( results, 1 );
            }
          }
          else if ( boost::regex_search( line, results, levelPattern ) )
          {
            // Level
            if ( results[1].matched )
            {
              LevelInfoPtr level = new LevelInfo();
              level->m_Name = Nocturnal::BoostMatchResultAsString( results, 1 );

              std::string levelFile( state );
              FileSystem::AppendPath( levelFile, "level_" + level->GetStrippedName() + "_done.txt" );
              level->m_HasServerData = FileSystem::Exists( levelFile );

              build->m_Levels.push_back( level );
            }
          }
        }
        file.close();
      }

      // Levels
      //std::string levelFile( directory );
      //FileSystem::AppendPath( levelFile, "build_levels.txt" );
      //if ( FileSystem::Exists( levelFile ) )
      //{
      //  std::fstream file( levelFile.c_str(), std::ios_base::in );
      //  while ( !file.fail() )
      //  {
      //    std::string line;
      //    std::getline( file, line );
      //    if ( !line.empty() )
      //    {
      //      LevelInfoPtr level = new LevelInfo();
      //      level->m_Name = line;
      //      build->m_Levels.push_back( line );
      //    }
      //  }
      //}

      m_Builds.insert( M_Builds::value_type( build->m_Name, build ) );
    }
  }
}
