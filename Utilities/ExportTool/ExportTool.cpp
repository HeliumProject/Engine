#include "Windows/Windows.h"

#include "Worker/Client.h"
#include "Worker/Process.h"
#include "Debug/Exception.h"
#include "AppUtils/AppUtils.h"
#include "Console/Console.h"
#include "Common/CommandLine.h"
#include "Common/InitializerStack.h"
#include "FileSystem/FileSystem.h"
#include "File/Manager.h"
#include "File/ManagedFileUtils.h"
#include "Finder/Finder.h"

#include "IPC/Connection.h"

#include "Content/ContentInit.h"

#include "AssetExporter/AssetExporter.h"
#include "Asset/ExporterJob.h"

#include "MayaContent/MayaContentCmd.h"

using namespace Reflect;
using namespace Content;
using namespace Asset;

bool ExportAssets( int argc, const char** argv )
{
  bool success = true;

  if (argc <= 1)
  {
    Console::Print("ExportTool - Export Manager for Insomniac Games Reflect File Format\n");
    Console::Print("Usage : ExportTool <file pattern>\n");
  }
  else
  {
    try
    {
      std::string filePath;
      S_tuid filesToSearch;
      S_string mayaFiles;
      std::string trackerFilePath;
      std::string cleanAssetSpec;

      bool recurse = true;

      for (int i=1; i<argc; ++i)
      {
        std::string assetSpec = argv[i];

        if ( assetSpec == "-nr" )
        {
          recurse = false;
          continue;
        }

        if ( assetSpec == "-track_state" )
        {
          ++i;
          if ( i >= argc )
          {
            throw Nocturnal::Exception( "No file path passed to -track_state" );
          }

          trackerFilePath = argv[i];

          FileSystem::CleanName( trackerFilePath );

          continue;
        }

        // get the asset files they want to build
        FileSystem::CleanName( assetSpec );

        File::V_ManagedFilePtr possibleMatches;
        File::FindManagedFiles( assetSpec, possibleMatches );

        if ( possibleMatches.empty() )
        {
          return false;
        }

        File::V_ManagedFilePtr::const_iterator itr = possibleMatches.begin();
        File::V_ManagedFilePtr::const_iterator end = possibleMatches.end();
        for ( ; itr != end; ++itr )
        {
          filesToSearch.insert( (*itr)->m_Id );
        }
      }

      AssetExporter::Export( filesToSearch, recurse, trackerFilePath );
    }
    catch( const Nocturnal::Exception& e )
    {
      Console::Error( "Failed to export: %s\n", e.what() );
      success = false;
    }
  }

  return success;
}

bool ExportAssets()
{
  if (!Worker::Client::Initialize())
  {
    return false;
  }

  Console::Print("Waiting for export job...\n");

  IPC::Message* msg = Worker::Client::Receive();

  bool success = true;

  if ( msg )
  {
    std::strstream stream ((char*)msg->GetData(), msg->GetSize());

    Asset::ExporterJobPtr job = Reflect::ObjectCast< Asset::ExporterJob > (Reflect::Archive::FromStream(stream, Reflect::ArchiveTypes::Binary, Reflect::GetType< Asset::ExporterJob >()));

    if (!job.ReferencesObject())
    {
#ifdef _DEBUG
      ::MessageBoxA(NULL, "Unable to decode message from foreground!", "Error", MB_OK);
      return false;
#else
      throw Nocturnal::Exception("Unable to decode message from foreground!");
#endif
    }

    try
    {
      FileSystem::CleanName( job->m_StateTrackerFile );
      AssetExporter::Export( job->m_AssetIds, job->m_Recursive, job->m_StateTrackerFile, job->m_SetupLighting );
    }
    catch( const Nocturnal::Exception& e )
    {
      Console::Error( "Failed to export: %s\n", e.what() );
      success = false;
    }

    delete msg;
  }

  return success;
}

int Main(int argc, const char** argv)
{
  bool success = true;

  Nocturnal::InitializerStack initializerStack( true );
  initializerStack.Push( AssetExporter::Initialize, AssetExporter::Cleanup );

  if (Nocturnal::GetCmdLineFlag( Worker::Args::Worker ))
  {
    success = ExportAssets();
  }
  else
  {
    success = ExportAssets( argc, argv );
  }

  return success ? 0 : 1;
}

int main(int argc, const char** argv)
{
  return AppUtils::StandardMain( &Main, argc, argv );
}
