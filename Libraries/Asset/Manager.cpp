#include "Windows/Windows.h"

#include "Manager.h"

#include "Exceptions.h"
#include "AssetInit.h"
#include "EntityManifest.h"
#include "ArtFileAttribute.h"

#include "Attribute/AttributeHandle.h"
#include "AppUtils/AppUtils.h"
#include "Common/Container/Insert.h" 
#include "Common/Flags.h"
#include "Common/String/Utilities.h"
#include "Common/Types.h"
#include "Content/Scene.h"
#include "File/Manager.h"
#include "File/ManagedFileUtils.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/LunaSpecs.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ContentSpecs.h"
#include "Finder/AnimationSpecs.h"
#include "Finder/FinderSpec.h"
#include "Finder/ProjectSpecs.h"
#include "Console/Console.h"
#include "RCS/RCS.h"
#include "Reflect/Class.h"
#include "Reflect/Field.h"
#include "Reflect/Serializers.h"
#include "Reflect/Version.h"
#include "TUID/TUID.h"

using Nocturnal::Insert; 

using namespace Asset;
using namespace Asset::AssetFlags;

#define CONFIRM_RENAME_STRING_SIZE 1024
#define MAX_DEPTH 30

/////////////////////////////////////////////////////////////////////////////
//
//  INITIALIZE & CLEANUP FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////

namespace RelatedFileTypes
{
  enum RelatedFileType
  {
    Export,
    MetaData,
    Modify,
  };
}
typedef RelatedFileTypes::RelatedFileType RelatedFileType;

class RelatedFile
{
public:
  const FinderSpecs::ModifierSpec* m_Spec;
  RelatedFileType m_Type;

  RelatedFile( const FinderSpecs::ModifierSpec& spec, RelatedFileType function )
    : m_Spec( &spec )
    , m_Type( function )
  {}
};
typedef std::vector< RelatedFile > M_RelatedFile;
M_RelatedFile g_RelatedFiles;

int g_InitAssetManagerCount = 0;
void Manager::Initialize()
{
  if ( ++g_InitAssetManagerCount > 1 )
  {
    return;
  }

  ASSETMANAGER_SCOPE_TIMER((""));

  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::MANIFEST_DECORATION, RelatedFileTypes::Export ) );
  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::ANIMATION_DECORATION, RelatedFileTypes::Export ) );
  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::RIGGED_DECORATION, RelatedFileTypes::Export ) );
  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::STATIC_DECORATION, RelatedFileTypes::Export ) );
  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::COLLISION_DECORATION, RelatedFileTypes::Export ) );
  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::DESTRUCTION_GLUE_DECORATION, RelatedFileTypes::Export ) );
  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::PATHFINDING_DECORATION, RelatedFileTypes::Export ) );
  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::CINESCENE_DECORATION, RelatedFileTypes::Export ) );

  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Animation::EVENTS_DECORATION, RelatedFileTypes::MetaData ) );

  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Extension::JPG, RelatedFileTypes::Modify ) );
}

void Manager::Cleanup()
{
  if ( --g_InitAssetManagerCount )
  {
    return;
  }

  ASSETMANAGER_SCOPE_TIMER((""));

  g_RelatedFiles.clear();
}


/////////////////////////////////////////////////////////////////////////////
//
//  GET RELATED FILES
//
/////////////////////////////////////////////////////////////////////////////

std::string GetRelatedFilePath( const std::string& path, const Finder::ModifierSpec* spec, RelatedFileType function )
{
  switch ( function )
  {
  default:
    return std::string( "" );

  case RelatedFileTypes::Export:
    return ((const Finder::DecorationSpec*)spec)->GetExportFile( path );

  case RelatedFileTypes::MetaData:
    return ((const Finder::DecorationSpec*)spec)->GetMetaDataFile( path );
    break;

  case RelatedFileTypes::Modify:
    std::string newPath = path;
    spec->Modify( newPath );
    return newPath;
  }    
}

/////////////////////////////////////////////////////////////////////////////
void Manager::GetRelatedFiles( AssetFile* assetFile, S_string& relatedFiles )
{
  for each ( const RelatedFile& info in g_RelatedFiles )
  {
    std::string file = GetRelatedFilePath( assetFile->m_Path, info.m_Spec, info.m_Type );
    if ( FileSystem::Exists( file ) )
    {
      relatedFiles.insert( file );
    }
    else
    {
      // we may have previously added it, remove the file if it no longer matters
      relatedFiles.erase( file );
    }
  }
}
