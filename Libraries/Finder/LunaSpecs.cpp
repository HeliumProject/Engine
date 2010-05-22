#include "LunaSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"
#include "ProjectSpecs.h"
#include "Common/Environment.h"
#include "FileSystem/FileSystem.h"

namespace FinderSpecs
{
  const FolderSpec Luna::GLOBAL_CONFIG_FOLDER    ( "Luna::GLOBAL_CONFIG_FOLDER", "config/luna",                FolderRoots::ProjectRoot );
  const FolderSpec Luna::SPLASH_SCREEN_FOLDER    ( "Luna::SPLASH_SCREEN_FOLDER", "splash",                     Luna::GLOBAL_CONFIG_FOLDER );
  const FolderSpec Luna::CONFIG_FOLDER           ( "Luna::CONFIG_FOLDER",        "data/luna/config",           FolderRoots::ProjectTools );
  const FolderSpec Luna::THEMES_FOLDER           ( "Luna::THEMES_FOLDER",        "data/luna/themes",           FolderRoots::ProjectTools );
  const FolderSpec Luna::DEFAULT_THEME_FOLDER    ( "Luna::DEFAULT_THEME_FOLDER", "default",                    Luna::THEMES_FOLDER );
  const FolderSpec Luna::GAME_THEME_FOLDER       ( "Luna::GAME_THEME_FOLDER",    Finder::ProjectGame().c_str(), Luna::THEMES_FOLDER );
  const FolderSpec Luna::PREFERENCES_FOLDER      ( "Luna::PREFERENCES_FOLDER",   "preferences",                FolderRoots::ProjectTemp );
  
  const FileSpec Luna::EMPTY_MAYA_FILE           ( "Luna::EMPTY_MAYA_FILE",           "EmptyMayaFile",              Extension::MAYA_BINARY );
  const FileSpec Luna::TYPE_CONFIGURATIONS       ( "Luna::TYPE_CONFIGURATIONS",       "TypeConfigurations",         Extension::XML );
  const FileSpec Luna::MISC_SETTINGS             ( "Luna::MISC_SETTINGS",             "MiscSettings",               Extension::XML );
  const FileSpec Luna::GLOBAL_PREFS              ( "Luna::GLOBAL_PREFS",              "LunaGlobalPrefs",            Extension::REFLECT_BINARY );
  const FileSpec Luna::ASSET_EDITOR_PREFS        ( "Luna::ASSET_EDITOR_PREFS",        "LunaAssetEditorPrefs",       Extension::REFLECT_BINARY );
  const FileSpec Luna::SCENE_EDITOR_PREFS        ( "Luna::SCENE_EDITOR_PREFS",        "LunaSceneEditorPrefs",       Extension::REFLECT_BINARY );
  const FileSpec Luna::BROWSER_PREFS             ( "Luna::BROWSER_PREFS",             "LunaBrowserPrefs",           Extension::REFLECT_BINARY );
  const FileSpec Luna::ANIMATION_EVENTS_EDITOR_PREFS( "Luna::ANIMATION_EVENTS_EDITOR_PREFS","LunaAnimationEventsEditorPrefs",Extension::REFLECT_BINARY );
  const FileSpec Luna::CINEMATIC_EVENTS_EDITOR_PREFS( "Luna::CINEMATIC_EVENTS_EDITOR_PREFS", "LunaCinematicEventsEditorPrefs", Extension::REFLECT_BINARY );
  const FileSpec Luna::CHARACTER_EDITOR_PREFS    ( "Luna::CHARACTER_EDITOR_PREFS","LunaCharacterEditorPrefs",   Extension::REFLECT_BINARY );

  const FamilySpec Luna::ASSET_COLLECTION_FAMILY ( "Luna::ASSET_COLLECTION_FAMILY", "Asset Collection", "collection" );
  const DecorationSpec Luna::ASSET_COLLECTION_RB_DECORATION( "Luna::ASSET_COLLECTION_RB_DECORATION", "Asset Collection", ASSET_COLLECTION_FAMILY , Extension::REFLECT_BINARY );
  const DecorationSpec Luna::ASSET_COLLECTION_XML_DECORATION( "Luna::ASSET_COLLECTION_XML_DECORATION", "Asset Collection", ASSET_COLLECTION_FAMILY , Extension::XML );

  const FamilySpec Luna::SESSION_FAMILY          ( "Luna::SESSION_FAMILY",     "Luna Session",                "session" );
  const DecorationSpec Luna::SESSION_DECORATION  ( "Luna::SESSION_DECORATION", "Luna Session File",           Luna::SESSION_FAMILY,         Extension::REFLECT_BINARY );

  void Luna::Init()
  {
    std::string homeDir;
    std::string project;
    if ( Nocturnal::GetEnvVar( "USERPROFILE", homeDir ) && !homeDir.empty() && Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"PROJECT_NAME", project ) )
    {
      FolderSpec& folder = const_cast< FolderSpec& >( Luna::PREFERENCES_FOLDER );
      FileSystem::CleanName( homeDir );
      FileSystem::AppendPath( homeDir, ".insomniac/luna/" );
      if ( !project.empty() )
      {
        FileSystem::AppendPath( homeDir, project );
      }
      folder.SetValue( homeDir );
      folder.SetFolderRoot( FolderRoots::None );
    }
  }
}
