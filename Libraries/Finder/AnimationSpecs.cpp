#include "AnimationSpecs.h"

#include "AssetSpecs.h"
#include "CodeSpecs.h"
#include "ExtensionSpecs.h"
#include "Finder.h"

#include "Common/Environment.h"

namespace FinderSpecs
{
  const FolderSpec Animation::CONFIG_FOLDER     ( "Animation::CONFIG_FOLDER",       "assets/animation",           FolderRoots::ProjectCode );
  const FolderSpec Animation::QUERY_FOLDER      ( "Animation::QUERY_FOLDER",        "game/anim",                  FolderRoots::ProjectCode );

  const FileSpec Animation::ANIMSET_FILE        ( "Animation::ANIMSET_FILE",        "built.dat",        "26 - ahastings"            );

  // NOTE: changing this spec probably requires that you change Cinematic::BUILT_FILE as well
  const FileSpec Animation::ANIMCLIPS_FILE      ( "Animation::ANIMCLIPS_FILE",      "clips.dat",        "54 - phaile"            );

  const FileSpec Animation::ENUM_MANIFEST_FILE  ( "Animation::ENUM_MANIFEST_FILE",  "enum_manifest.txt","1 - jfaust"                );
  const FileSpec Animation::SHARED_GROUP_FILE   ( "Animation::SHARED_GROUP_FILE",   "",                 ""                          );
	const FileSpec Animation::ENUM_FILE           ( "Animation::ENUM_FILE",           "animation_enums",  Extension::XML              );
  const FileSpec Animation::ENUM_COMPARISON_FILE( "Animation::ENUM_COMPARISON_FILE","animation_enums_comparison", Extension::XML    );
  const FileSpec Animation::ENUM_DEPENDENCY_CHECK_FILE( "Animation::ENUM_DEPENDENCY_CHECK_FILE","animation_enums_dependency_check",Extension::XML);
  const FileSpec Animation::UNCATEGORIZED_GROUP ( "Animation::UNCATEGORIZED_GROUP", "uncategorized",    Asset::ANIMGROUP_DECORATION );
  const FileSpec Animation::CATEGORIES_FILE     ( "Animation::CATEGORIES_FILE",     "Categories",       Code::HEADER_FILE_EXTENSION );
  const FileSpec Animation::SUBCATEGORIES_FILE  ( "Animation::SUBCATEGORIES_FILE",  "SubCategories",    Code::HEADER_FILE_EXTENSION );
  const FileSpec Animation::MODIFIERS_FILE      ( "Animation::MODIFIERS_FILE",      "Modifiers",        Code::HEADER_FILE_EXTENSION );

  const FileSpec Animation::EVENTS_FILE         ( "Animation::EVENTS_FILE",          "",     "" );
  const SuffixSpec Animation::EVENTS_SUFFIX     ( "Animation::EVENTS_SUFFIX",        "_events" );
  const DecorationSpec Animation::EVENTS_DECORATION( "Animation::EVENTS_DECORATION", "Events File",    EVENTS_SUFFIX,     Extension::REFLECT_BINARY );

  void Animation::Init()
  {
    // awesome
    std::string value;
    if (Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"ANIM_MODE", value ) && value == "CURRENT")
    {
      // awesome++
      FolderSpec& folder = (FolderSpec&)Animation::CONFIG_FOLDER;
      folder.SetValue( "." );
      folder.SetFolderRoot( FolderRoots::None );
    }
    else
    {
      if (Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"GAME", value ))
      {
        {
          // awesome++
          FolderSpec& folder = (FolderSpec&)Animation::CONFIG_FOLDER;
          folder.SetValue( value + '/' + folder.GetValue() );
        }

        {
          // awesome++
          FolderSpec& folder = (FolderSpec&)Animation::QUERY_FOLDER;
          folder.SetValue( value + '/' + folder.GetValue() );
        }
      }
      else
      {
        throw Nocturnal::Exception( NOCTURNAL_STUDIO_PREFIX"GAME is not defined in the environment" );
      }
    }
  }
}
  
