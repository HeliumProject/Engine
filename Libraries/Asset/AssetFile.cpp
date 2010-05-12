#include "AssetFile.h"

#include "AssetClass.h"
#include "ArtFileAttribute.h"
#include "ColorMapAttribute.h"

#include "CacheDB.h"

#include "Attribute/AttributeHandle.h"
#include "Common/String/Utilities.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/FinderSpec.h"
#include "Finder/ProjectSpecs.h"
#include "RCS/RCS.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( AssetFile );
void AssetFile::EnumerateClass( Reflect::Compositor<AssetFile>& comp )
{
}

///////////////////////////////////////////////////////////////////////////////
/// class AssetFile
///////////////////////////////////////////////////////////////////////////////

AssetFile::AssetFile()
{
  Init();
}

AssetFile::AssetFile( const File::ManagedFile* file )
{
  Init();

  SetManagedFile( file );
}

AssetFile::~AssetFile()
{
}

/////////////////////////////////////////////////////////////////////////////
AssetFilePtr AssetFile::FindAssetFile( const tuid id, bool useCacheDB )
{
  NOC_ASSERT( id != TUID::Null );
  
  AssetFilePtr assetFile = new AssetFile();

  File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( id );
  if ( file )
  {
    assetFile->SetManagedFile( file );
  }
  else if ( useCacheDB )
  {
    assetFile->m_IsTemporary = true;
    assetFile->m_Id = id;
    Asset::GlobalCacheDB()->SelectAssetPathByID( id, assetFile->m_Path );
  }
  else
  {
    // TODO: what do we want to do here?
    return NULL;
  }

  if ( useCacheDB )
  {
    // select the file from the cache
    Asset::GlobalCacheDB()->SelectAssetByID( id, assetFile );
  }

  return assetFile;
}

/////////////////////////////////////////////////////////////////////////////
AssetFilePtr AssetFile::FindAssetFile( const std::string& path, bool useCacheDB )
{
  NOC_ASSERT( !path.empty() );

  AssetFilePtr assetFile = new AssetFile();

  File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( path );
  if ( file )
  {
    assetFile->SetManagedFile( file );

    if ( useCacheDB && assetFile->m_Id != TUID::Null )
    {
      // select the file from the cache
      Asset::GlobalCacheDB()->SelectAssetByID( assetFile->m_Id, assetFile );
    }
  }
  else
  {
    // TODO: what do we want to do here?

    // Generate an Id so that this file is unique - hopefully other
    // interfaces can add the file to the Resolver when it is
    // referenced by an asset
    assetFile->m_IsTemporary = true;
    assetFile->m_Path = path;
    TUID::Generate( assetFile->m_Id );
  }

  return assetFile;
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::Init()
{
  m_ShortName               = "";
  m_ModifierSpec            = NULL;
  m_Extension               = "";
  m_FileType                = "";
  m_EngineType              = Asset::EngineTypes::Null;
  m_EngineTypeName          = "";
  m_Size                    = 0;
  m_P4User                  = "";
  m_Attributes.clear();
  m_DependencyIDs.clear();
  m_RowID                   = 0;
  m_P4LocalRevision         = 0;
  m_IsTemporary             = false;
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::SetManagedFile( const File::ManagedFile* file )
{
  if ( file )
  {
    this->m_Id        = file->m_Id;         
    this->m_Created   = file->m_Created;    
    this->m_Modified  = file->m_Modified;   
    this->m_Path      = file->m_Path;  
    this->m_Username  = file->m_Username;
    this->m_WasDeleted = file->m_WasDeleted;

    Init();
  }
}

/////////////////////////////////////////////////////////////////////////////
const std::string& AssetFile::GetShortName()
{
  if ( m_ShortName.empty() )
  {
    FileSystem::GetLeaf( GetFilePath(), m_ShortName );
    FileSystem::StripExtension( m_ShortName );
  }
  return m_ShortName;
}

/////////////////////////////////////////////////////////////////////////////
const Finder::ModifierSpec* AssetFile::GetModifierSpec()
{
  if ( !m_ModifierSpec )
  {
    try
    {
      m_ModifierSpec = Finder::GetFileExtensionSpec( GetFilePath() );
    }
    catch( Finder::Exception & )
    {
      // try again below
    }

    if (!m_ModifierSpec )
    {
      std::string extension = FileSystem::GetExtension( GetFilePath(), 1 );
      try
      {
        m_ModifierSpec = Finder::GetFileExtensionSpec( extension );
      }
      catch( Finder::Exception & )
      {
      }
    }
  }
  return m_ModifierSpec;
}

/////////////////////////////////////////////////////////////////////////////
// Gets and caches the file extension
const std::string& AssetFile::GetExtension()
{
  if ( m_Extension.empty() )
  {
    FileSystem::GetExtension( GetFilePath(), m_Extension );
  }
  return m_Extension;
}

/////////////////////////////////////////////////////////////////////////////
// Gets and caches the file type
const std::string& AssetFile::GetFileType()
{
  if ( m_FileType.empty() )
  {
    if ( GetModifierSpec() )
    {
      m_FileType = GetModifierSpec()->GetUIName();
    }
    else
    {
      m_FileType = FileSystem::GetExtension( GetFilePath(), 1 );

      if ( !m_FileType.empty() && *m_FileType.begin() == '.' )
      {
        m_FileType.erase( 0, 1 );
      }

      if ( m_FileType.empty() )
      {
        m_FileType = "Unknown";
      }
      else
      {
        toUpper( m_FileType );
      }
    }
  }
  return m_FileType;
}

/////////////////////////////////////////////////////////////////////////////
u64 AssetFile::GetSize()
{
  if ( m_Size == 0 )
  {
    struct _stati64 fileStats;
    if ( FileSystem::GetStats64( GetFilePath(), fileStats ) )
    {
      m_Size = fileStats.st_size;
    }
  }
  return m_Size;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& AssetFile::GetP4User()
{
  if ( m_P4User.empty() )
  {
    RCS::File rcsFile( GetFilePath() );
    rcsFile.GetCreatedByUser( m_P4User );
  }

  return m_P4User;
}

/////////////////////////////////////////////////////////////////////////////
i32 AssetFile::GetP4Revision()
{
  if ( m_P4LocalRevision == 0 )
  {
    RCS::File rcsFile( GetFilePath() );
    rcsFile.GetInfo( RCS::GetInfoFlags::GetHistory );
    m_P4LocalRevision = rcsFile.m_LocalRevision;
  }

  return m_P4LocalRevision;
}

/////////////////////////////////////////////////////////////////////////////
EngineType AssetFile::GetEngineType()
{
  if ( m_EngineType == EngineTypes::Null )
  {
    const std::string& engineTypeName = GetEngineTypeName();
    if ( engineTypeName != "Unknown" || engineTypeName != "Null" )
    {
      const Reflect::Enumeration* info = Reflect::Registry::GetInstance()->GetEnumeration( "EngineType" );
      if ( info )
      {
        info->GetElementValue( engineTypeName, (u32&) m_EngineType );
      }
    }
  }
  return m_EngineType;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& AssetFile::GetEngineTypeName()
{
  if ( m_EngineTypeName.empty() )
  {
    AssetClassPtr assetClass = AssetFile::GetAssetClass( this );
    if ( assetClass )
    {
      m_EngineTypeName = assetClass->GetEngineTypeName();
    }
    else
    {
      m_EngineTypeName = "Unknown";
    }
  }
  return m_EngineTypeName; 
}

/////////////////////////////////////////////////////////////////////////////
AssetClassPtr AssetFile::GetAssetClass( const AssetFile* assetFile )
{
  if ( !assetFile )
    return NULL;

  if ( assetFile->GetFileID() != TUID::Null
    && !assetFile->GetFilePath().empty() 
    && FileSystem::HasExtension( assetFile->GetFilePath(), FinderSpecs::Extension::REFLECT_BINARY.GetExtension() ) )
  {
    Asset::AssetClassPtr assetClass = NULL;
    try
    {
      assetClass = Asset::AssetClass::FindAssetClass( assetFile->GetFileID() );

      if ( assetClass.ReferencesObject() )
      {
        return assetClass;
      }       
    }
    catch( const Nocturnal::Exception& ex )
    {
      // do nothing
      Console::Warning( Console::Levels::Verbose, "AssetFile: %s\n", ex.what() );
    }
  }
  return NULL;
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::AddAttribute( const std::string& attrName, const std::string& attrValue, bool canAppend )
{
  Nocturnal::Insert<M_string>::Result inserted = m_Attributes.insert( M_string::value_type( attrName, attrValue ) );
  if ( !inserted.second && inserted.first->second != attrValue )
  {
    std::string& attributes = inserted.first->second;
    if ( canAppend && attributes.find( attrValue ) == std::string::npos )
    {
      // append it to the existing one
      attributes += attributes.empty() ? "" : ", ";
      attributes += attrValue;
    }
    else
    {
      // why are we inserting the same attribute value twice?
      NOC_BREAK();
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::AddDependencyID( const tuid dependencyID )
{
  m_DependencyIDs.insert( dependencyID );
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::SetDependencyIDs( const S_tuid& dependencyIDs )
{
  m_DependencyIDs.clear();
  m_DependencyIDs = dependencyIDs;
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::GetDependencyIDsOfType( M_AssetFiles* assetFiles, i32 type, S_tuid& dependencies )
{
  S_tuid visitedTuids;
  return GetDependencyIDsOfType( assetFiles, type, dependencies, visitedTuids, 0 );
}

void AssetFile::GetDependencyIDsOfType( M_AssetFiles* assetFiles, i32 type, S_tuid& dependencies, S_tuid& visitedTuids, u32 depth )
{
  ++depth;

  if ( this->HasDependencies() )
  {
    S_tuid assetDependencies = GetDependencyIDs();
    S_tuid::const_iterator itr = assetDependencies.begin();
    S_tuid::const_iterator end = assetDependencies.end();
    for ( ; itr != end; ++itr )
    {
      if ( *itr == TUID::Null )
        continue;

      File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( *itr );

      if ( FileSystem::HasExtension( file->m_Path, FinderSpecs::Extension::REFLECT_BINARY.GetExtension() ) )
      {
        if ( visitedTuids.find( file->m_Id ) == visitedTuids.end() )
        {
          visitedTuids.insert( file->m_Id );

          M_AssetFiles::iterator foundFile = assetFiles->find( *itr );
          if ( foundFile != assetFiles->end() )
          {
            AssetFile* assetDependency = foundFile->second;
            AssetClassPtr assetClass = AssetFile::GetAssetClass( assetDependency );
            if ( assetClass && assetClass->HasType( type ) )
            {
              dependencies.insert( file->m_Id );
            }
            S_tuid depIDs;
            assetDependency->GetDependencyIDsOfType( assetFiles, type, depIDs, visitedTuids, depth );
            dependencies.insert( depIDs.begin(), depIDs.end() );
          }
        }
      }
    }
  }

  if ( --depth == 0 )
  {
    visitedTuids.clear();
  }
}


/////////////////////////////////////////////////////////////////////////////
//void AssetFile::GetEngineTypeName( const Asset::AssetClass* assetClass, std::string& engineTypeName )
//{
//  if ( assetClass )
//  {
//    const Reflect::Enumeration* info = Reflect::Registry::GetInstance()->GetEnumeration( "EngineType" );
//    if ( info && info->GetElementLabel( assetClass->GetEngineType(), engineTypeName ) )
//    {
//      return;
//    }
//  }
//  engineTypeName = "Unknown";
//}

/////////////////////////////////////////////////////////////////////////////
//const std::string& AssetFile::GetRelativePath()
//{
//  if ( m_RelativePath.empty() )
//  {
//    m_RelativePath = GetFilePath();
//    FileSystem::StripPrefix( File::GlobalManager().GetManagedAssetsRoot(), m_RelativePath );
//  }
//
//  return m_RelativePath;
//}

/////////////////////////////////////////////////////////////////////////////
//const std::string& AssetFile::GetPrintSize()
//{
//  if ( m_PrintSize.empty() )
//  {
//    u64 size = GetSize();
//
//    std::stringstream printSize;
//    if ( size == 0 )
//    {
//      printSize << "0 KB";
//    }
//    else if ( size <= 1024 )
//    {
//      printSize << "1 KB";
//    }
//    else
//    {
//      size = size / 1024;
//      printSize << size << " KB";
//    }
//
//    m_PrintSize = printSize.str();
//  }
//  return m_PrintSize;
//}

/////////////////////////////////////////////////////////////////////////////
// Gets the path to the ScreneShot from the art file path
//
// Example:
//   x:/rcf/assets/devel/maya/entities/shrubs/shrub0/shrub.mb
// becomes...
//   x:/rcf/assets/devel/maya/entities/shrubs/shrub0/shrub.jpg
//
//const std::string& AssetFile::GetScreneShotPath()
//{
//  if ( m_ScreneShotPath.empty() )
//  {
//    const std::string& extension = GetExtension();
//
//    if ( extension == FinderSpecs::Extension::REFLECT_BINARY.GetExtension() )
//    {
//      m_ScreneShotPath = GetFilePath();
//      FinderSpecs::Extension::JPG.Modify( m_ScreneShotPath );
//    }
//    else if ( extension == FinderSpecs::Extension::PNG.GetExtension()
//      || extension == FinderSpecs::Extension::TGA.GetExtension()
//      || extension == FinderSpecs::Extension::JPG.GetExtension()
//      || extension == FinderSpecs::Extension::GIF.GetExtension() )
//    {
//      m_ScreneShotPath = GetFilePath();
//    }
//    else if ( extension == FinderSpecs::Asset::ENTITY_DECORATION.GetDecoration() )
//    {
//      GetArtFile();
//
//      if ( !m_ArtFile.empty() )
//      {
//        m_ScreneShotPath = m_ArtFile;
//        FinderSpecs::Extension::JPG.Modify( m_ScreneShotPath );
//      }
//    }
//    else if ( extension == FinderSpecs::Asset::SHADER_DECORATION.GetDecoration() )
//    {
//      Asset::AssetClassPtr assetClass = NULL;
//      try
//      {
//        assetClass = Asset::AssetClass::FindAssetClass( GetFileID() );
//
//        if ( assetClass.ReferencesObject() )
//        {
//          Asset::ColorMapAttributePtr colorMapFile = Reflect::ObjectCast< Asset::ColorMapAttribute >( assetClass->GetAttribute( Reflect::GetType< Asset::ColorMapAttribute >() ) );
//
//          if ( colorMapFile.ReferencesObject() )
//          {
//            m_ScreneShotPath = colorMapFile->GetFilePath();
//          }
//        }       
//      }
//      catch( const Nocturnal::Exception& )
//      {
//        // do nothing
//      }
//    }
//  }
//
//  return m_ScreneShotPath;
//}

/////////////////////////////////////////////////////////////////////////////
// Get's the art file for the entity class
//
//const std::string& AssetFile::GetArtFile()
//{
//  if ( m_ArtFile.empty() )
//  {
//    Asset::AssetClassPtr assetClass = NULL;
//    try
//    {
//      assetClass = Asset::AssetClass::FindAssetClass( GetFileID() );
//
//      if ( assetClass.ReferencesObject() )
//      {
//        Attribute::AttributeViewer< Asset::ArtFileAttribute > artFile ( assetClass );
//        if ( artFile.Valid() )
//        {
//          m_ArtFile = artFile->GetFilePath();
//        }
//      }       
//    }
//    catch( const Nocturnal::Exception& )
//    {
//      // do nothing
//    }
//  }
//
//  return m_ArtFile;
//}
