#include "stdafx.h"

#include "AssetTracker.h"
#include "AssetDB.h"
#include "AssetTrackerVisitor.h"

#include "Asset/AssetInit.h"
#include "Asset/EntityManifest.h"
#include "Asset/ArtFileAttribute.h"
#include "Asset/ConversionTable.h"
#include "Asset/Exceptions.h"
#include "Asset/ShaderUsagesAttribute.h"
#include "Attribute/AttributeHandle.h"
#include "AppUtils/AppUtils.h"
#include "Common/Container/Insert.h" 
#include "Common/Flags.h"
#include "Common/String/Utilities.h"
#include "Common/Types.h"
#include "Console/Console.h"
#include "Content/ContentInit.h"
#include "Common/InitializerStack.h"
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
#include "Reflect/Class.h"
#include "Reflect/Serializers.h"
#include "Reflect/Version.h"
#include "Reflect/Visitor.h"
#include "TUID/TUID.h"


using namespace Asset;

//
// Typedefs
//

typedef std::map< tuid, S_tuid > M_AssetDependencies; 

//
// Init/Cleanup
//

int g_InitRef = 0;
Nocturnal::InitializerStack g_InitializerStack;

AssetTracker::AssetTracker()
{
}

AssetTracker::~AssetTracker()
{
  Cleanup();
}

bool AssetTracker::Initialize()
{
  if ( ++g_InitRef > 1 )
  {
    return true;
  }

  ASSETTRACKER_SCOPE_TIMER((""));

  g_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
  g_InitializerStack.Push( Finder::Initialize, Finder::Cleanup );
  g_InitializerStack.Push( File::Initialize, File::Cleanup );
  g_InitializerStack.Push( Asset::Initialize, Asset::Cleanup );
  g_InitializerStack.Push( Content::Initialize, Content::Cleanup );

  m_AssetDB = new AssetDB();
  m_AssetDB->Initialize();
  m_AssetDB->RegisterTracker( this );

  return true;
}


void AssetTracker::Cleanup()
{
  if ( --g_InitRef )
  {
    return;
  }

  ASSETTRACKER_SCOPE_TIMER((""));

  g_InitializerStack.Cleanup();

  m_AssetDB = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Entry point for AssetTrackerVisitor
//
bool AssetTracker::TrackAssetFile( const File::ManagedFilePtr& file, M_AssetFiles* assetFiles )
{
  ASSETTRACKER_SCOPE_TIMER((""));
  
  if ( !assetFiles )
  {
    NOC_BREAK()
  }

  if ( !FileSystem::Exists( file->m_Path ) )
  {
    return false;
  }

  Nocturnal::Insert<M_AssetFiles>::Result inserted = assetFiles->insert( M_AssetFiles::value_type( file->m_Id, new AssetFile( file ) ) );
  if ( inserted.second )
  {
    // this tuid may represent an asset file
    if ( FileSystem::HasExtension( file->m_Path, FinderSpecs::Extension::REFLECT_BINARY.GetExtension() ) )
    {
      try
      {
        Asset::AssetClassPtr assetClass = Asset::AssetClass::FindAssetClass( file->m_Id, false );
        if ( assetClass.ReferencesObject() )
        {
          AssetFilePtr& assetFile = inserted.first->second;
          assetFile->SetAssetClass( assetClass );

          AssetTrackerVisitor assetTrackerVisitor( assetFiles, assetClass, m_AssetDB );
          assetClass->Host( assetTrackerVisitor );
        }
      }
      catch ( ... )
      {
        // don't do anything. We weren't able to get the asset class but we still want the info we do have 
        // added to the DB
      }
    }
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////
bool AssetTracker::TrackFile( const File::ManagedFilePtr& file )
{
  ASSETTRACKER_SCOPE_TIMER((""));
  
  bool result = false;

  if ( m_AssetDB->AssetHasChangedOnDisc( file ) )
  {
    M_AssetFiles assetFiles;
    if ( TrackAssetFile( file, &assetFiles ) )
    {
      // update the DB
      M_AssetFiles::iterator found = assetFiles.find( file->m_Id );
      if ( found != assetFiles.end() )
      {
        AssetFilePtr& assetFile = found->second;
        m_AssetDB->InsertAssetFile( assetFile, &assetFiles );
        result = true;
      }
    }

    if ( !result )
    {
      // still insert the file, even if we have no data for it
      AssetFilePtr assetFile = new AssetFile( file );
      m_AssetDB->InsertAssetFile( assetFile, &assetFiles );
    }
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
bool AssetTracker::TrackFile( const tuid id )
{
  ASSETTRACKER_SCOPE_TIMER((""));
  
  File::ManagedFilePtr file;

  try
  {
    file = File::GlobalManager().GetManagedFile( id, true );
  }
  catch( const Nocturnal::Exception& )
  {
    return false;
  }

  return TrackFile( file );
}


///////////////////////////////////////////////////////////////////////////////
bool AssetTracker::TrackFiles( const File::V_ManagedFilePtr& files )
{
  ASSETTRACKER_SCOPE_TIMER((""));
  
  bool result = true;
  
  File::V_ManagedFilePtr::const_iterator itrFile = files.begin();
  File::V_ManagedFilePtr::const_iterator endFile = files.end();
  for( ; itrFile != endFile, result; ++itrFile )
  {
    result = result && TrackFile(*itrFile);
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Process over all of the assets and insert them into the asset DB
// 
void AssetTracker::TrackEverything()
{
  ASSETTRACKER_SCOPE_TIMER((""));

  const static char* entityDec = "*.entity.irb";  // for now, start just with entities

  File::V_ManagedFilePtr managedFiles;
  //File::GlobalManager().Find( "*.irb", managedFiles );
  //File::GlobalManager().Find( "*.entity.irb", managedFiles );
  //File::GlobalManager().Find( "*.sky.irb", managedFiles );
  //File::GlobalManager().Find( "*.shader.irb", managedFiles );
  File::GlobalManager().Find( "rachels_level.level.irb", managedFiles );
  //File::GlobalManager().Find( "*.foliage.irb", managedFiles );
  //File::GlobalManager().Find( "*.level.irb", managedFiles );

  while ( !managedFiles.empty() )
  {
    try
    {
      TrackFile( managedFiles.back() );
    }
    catch( ... )
    {
    }
    managedFiles.pop_back();
  }
}
