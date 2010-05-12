#include "AssetExporter.h"

#include "AppUtils/AppUtils.h"

#include "Common/InitializerStack.h"
#include "Console/Console.h"
#include "FileSystem/FileSystem.h"
#include "File/Manager.h"
#include "Finder/Finder.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/ContentSpecs.h"

#include "Asset/AssetVersion.h"
#include "Asset/AssetClass.h"
#include "Asset/AssetInit.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/ArtFileAttribute.h"
#include "Asset/AnimationAttribute.h"
#include "Asset/AnimationSetAsset.h"

#include "Content/ContentInit.h"

#include "MayaUtils/MayaUtils.h"
#include "MayaUtils/File.h"
#include "MayaContent/MayaContentCmd.h"


#include <maya/MFileIO.h>
#include <maya/MStatus.h> 
#include <maya/MTypes.h> 
#include <maya/MArgList.h>

#include "RCS/rcs.h"
#include "../maya/MayaShaderManager/MayaShaderManager.h"
#include "../maya/igSetupLighting/igSetupLightingCmd.h"

using namespace Asset;

namespace AssetExporter
{
  static i32 g_InitCount = 0;
  Nocturnal::InitializerStack g_InitializerStack;

  void Initialize()
  {
    if ( ++g_InitCount == 1 )
    {
      Maya::Init("AssetExporter");
      MGlobal::sourceFile( "igLib" );
      MGlobal::sourceFile( "hasTimeAnimCurves" );
      MGlobal::executeCommand( "loadPlugin igAsset" );
      MGlobal::executeCommand( "loadPlugin MayaNodes" );

      g_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
      g_InitializerStack.Push( Finder::Initialize, Finder::Cleanup );
      g_InitializerStack.Push( File::Initialize, File::Cleanup );
      g_InitializerStack.Push( Content::Initialize, Content::Cleanup );
      g_InitializerStack.Push( Asset::Initialize, Asset::Cleanup );
    }
  }

  void Cleanup()
  {
    if ( --g_InitCount == 0 )
    {
      g_InitializerStack.Cleanup();
      Maya::Cleanup();
    }
  }


  void FindMayaFiles( const S_tuid& fileIdSet, bool recurse, S_string& mayaFiles )
  {
    static std::string mayaExtension    = FinderSpecs::Extension::MAYA_BINARY.GetExtension();
    static std::string reflectExtension = FinderSpecs::Extension::REFLECT_BINARY.GetExtension();

    S_tuid fileIds = fileIdSet;
    while( !fileIds.empty() )
    {
      S_tuid fileIdsForThisPass = fileIds;
      fileIds.clear();

      std::string filePath;

      for each( const tuid fileId in fileIdsForThisPass )
      {
        try 
        {
          filePath = File::GlobalManager().GetPath( fileId );
          if ( !FileSystem::Exists( filePath ) )
            continue;
        }
        catch( Nocturnal::Exception& )
        {
          continue;
        }

        if ( FileSystem::HasExtension( filePath, mayaExtension ) )
        {
          mayaFiles.insert( filePath );
          continue;
        }

        if ( FileSystem::HasExtension( filePath, reflectExtension ) )
        {
          Asset::AssetClassPtr assetClass;
          try
          {
            assetClass = AssetClass::FindAssetClass( fileId );
            if ( !assetClass.ReferencesObject() )
              continue;
          }
          catch( Nocturnal::Exception& )
          {
            continue;
          }

          Asset::AnimationSetAssetPtr animationSet = Reflect::ObjectCast< Asset::AnimationSetAsset >( assetClass );
          if( animationSet.ReferencesObject() )
          {
            V_AnimationClipData clips;
            animationSet->GetAnimationClips( clips );

            for each ( const AnimationClipDataPtr& clip in clips )
            {
              fileIds.insert( clip->m_ArtFile );
            }
          }

          Attribute::AttributeViewer< Asset::ArtFileAttribute > model ( assetClass );
          if ( model.Valid() )
            fileIds.insert( model->m_FileID );

          // if we want to recurse into nested assets
          if ( recurse )
          {
            Attribute::AttributeViewer< Asset::AnimationAttribute > animation( assetClass );
            if ( animation.Valid() && animation->m_AnimationSetId != TUID::Null )
              fileIds.insert( animation->m_AnimationSetId );

#pragma TODO("This is pretty harsh and special case, if there were a nicer way to do this, it would be cool..." )
            if ( assetClass->GetEngineType() == Asset::EngineTypes::Moby )
            {
              File::V_ManagedFilePtr cinematicFiles;
              File::GlobalManager().Find( "*.cinematic.irb", cinematicFiles );

              for each ( const File::ManagedFilePtr& cinematicFile in cinematicFiles )
              {
                Asset::AssetClassPtr cinematicAsset ;
                try
                {
                  cinematicAsset = AssetClass::FindAssetClass( cinematicFile->m_Id );
                }
                catch ( const Nocturnal::Exception& e )
                {
                  Console::Warning( "Error opening cinematic with ID "TUID_HEX_FORMAT", skipping.\n(%s)\n", cinematicFile->m_Id, e.what() );
                  continue;
                }

                if ( !cinematicAsset.ReferencesObject() )
                  continue;

                Attribute::AttributeViewer< Asset::ArtFileAttribute > cinematicArtFile( cinematicAsset );
                if ( !cinematicArtFile.Valid() )
                  continue;

                std::string cinematicArtFilePath = File::GlobalManager().GetPath( cinematicArtFile->m_FileID );
                std::string cinematicAnimationFile = FinderSpecs::Content::ANIMATION_DECORATION.GetExportFile( cinematicArtFilePath );

                if ( !FileSystem::Exists( cinematicAnimationFile ) )
                  continue;

                Content::Scene cinematicScene( cinematicAnimationFile );
                for each ( const Content::AnimationClipPtr& animationClip in cinematicScene.m_AnimationClips )
                {
                  if ( animationClip->m_OptionalEntityID == assetClass->m_AssetClassID )
                    fileIds.insert( cinematicArtFile->m_FileID );
                }
              }
            }
          }
        }
      }
    }
  }

  void Export( const S_tuid& fileIdSet, bool recurse, const std::string& stateTrackerFile, bool setupForLighting )
  {
    bool trackState = false;
    S_string exportedFiles;
    if ( !stateTrackerFile.empty() )
    {
      Console::Print( "Using state tracker file '%s'\n", stateTrackerFile.c_str() );

      std::ifstream in;
      in.open( stateTrackerFile.c_str() );

      if ( in.is_open() )
      {
        while ( !in.eof() )
        {
          std::string filePath;

          std::getline( in, filePath );

          exportedFiles.insert( filePath );
        }

        in.close();
      }

      trackState = true;
    }

    std::ofstream out;
    if ( trackState )
    {
      out.open( stateTrackerFile.c_str(), std::ios_base::app );

      if ( !out.is_open() )
      {
        throw Nocturnal::Exception( "Failed to open tracker file '%s' for write", stateTrackerFile.c_str() );
      }
    }

    S_string mayaFiles;
    FindMayaFiles( fileIdSet, recurse, mayaFiles );

    size_t totalMayaFiles = mayaFiles.size();
    Console::Print( "Preparing to export %d files:\n", totalMayaFiles );
    {
      u32 fileCount = 1;
      for ( S_string::const_iterator fileItr = mayaFiles.begin(), fileEnd = mayaFiles.end(); fileItr != fileEnd; ++fileItr, ++fileCount )
      {
        Console::Print( "%d) %s\n", fileCount, ( *fileItr ).c_str() );
      }
      Console::Print( "\n" );
    }

    u32 fileCount = 0;
    for each( const std::string& mayaFile in mayaFiles )
    {
      ++fileCount;
      try
      {
        if ( trackState )
        {
          if ( !exportedFiles.insert( mayaFile ).second )
          {
            Console::Print( "\n=====================================================\n" );
            Console::Print( "Skipping file '%s' (tracker file says it's already exported)\n", mayaFile.c_str() );
            Console::Print( "=====================================================\n" );

            continue;
          }
        }

        Console::Print( "\n=====================================================\n" );
        Console::Print( "Exporting file %d / %d\n%s\n", fileCount, totalMayaFiles, mayaFile.c_str() );
        Console::Print( "=====================================================\n" );

        Maya::OpenFile( mayaFile );
        Maya::UpdateShaders();
        
        if( setupForLighting )
        {
      
          try
          {
            RCS::File rcsFile( mayaFile );
            rcsFile.GetInfo();
            bool alreadyOpen = rcsFile.IsOpen();

            if( !alreadyOpen )
              rcsFile.Open();

            igSetupLighting cmd;

            MStatus status = cmd.SetupLighting();
            if( status != MS::kFailure )
            {
              Maya::SaveFile();
            }
            else
            {
              try
              {
                if( !alreadyOpen )
                  rcsFile.Revert();
              }              
              catch (Nocturnal::Exception& e)
              {
                Console::Warning(" Unable to automatically revert file due to lighting setup failure: %s", e.what() );
              }
              if( cmd.m_ErrorLockNodeExists )
                Console::Warning( " Art file has been manually locked from Automatic Lighting Setup by %s.  You will need to set it up manually.", cmd.m_UserName.c_str() );
              else
                Console::Warning( " Unable to automatically setup file for lighting by, you may need to set it up manually" );
            }
            
          }
          catch (Nocturnal::Exception& e)
          {
            Console::Warning( " Unable to setup file for lighting: %s", e.what() );
          }         
        }
       
        MayaContent::MayaContentCmd::ExportCurrentScene();

        if ( trackState )
        {
          out << mayaFile << std::endl;
        }

        // we have to call this so we can safely close the file without hanging
        MGlobal::executeCommand( "igLibCleanPolyLoopSplit", false, false );

        Maya::CloseFile();
      }
      catch( Nocturnal::Exception& e )
      {
        Console::Error( "Failed to export: %s\n", e.what() );
      }
    }

    if ( trackState )
    {
      out.close();
    }
  }

  void Export( const tuid fileId, bool recurse, const std::string& stateTrackerFile, bool setupForLighting )
  {
    S_tuid fileIdSet;
    fileIdSet.insert( fileId );
    Export( fileIdSet, recurse, stateTrackerFile, setupForLighting );
  }
}
