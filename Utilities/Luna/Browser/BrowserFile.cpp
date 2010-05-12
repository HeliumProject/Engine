#include "stdafx.h"

#include "AssetFile.h"

#include "Asset/AssetClass.h"
#include "Asset/ArtFileAttribute.h"
#include "Asset/ColorMapAttribute.h"
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


///////////////////////////////////////////////////////////////////////////////
// Ctor/Dtor
//
AssetFile::AssetFile( const File::ManagedFile* file )
{
  // this will also init the asset cache
  InitializeFileCache();

  SetManagedFile( file );
}

AssetFile::~AssetFile()
{
  m_File = NULL;
  m_AssetClass = NULL;
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::InitializeFileCache()
{
  m_File                    = NULL;

  m_RelativePath            = "";
  m_ModifierSpec            = NULL;
  m_Extension               = "";
  m_FileType                = "";
  m_Size                    = 0;
  m_PrintSize               = "";
  m_ArtFile                 = "";
  m_ScreneShotPath          = "";

  m_RowID                   = 0;

  InitializeAssetCache();
}

void AssetFile::InitializeAssetCache()
{
  m_AssetClass              = NULL;

  m_EngineTypeName          = "";
  m_DependencyIDs.clear();
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::SetManagedFile( const File::ManagedFile* file )
{
  if ( file && file != m_File )
  {
    InitializeFileCache();
    m_File = file;
  }
}

/////////////////////////////////////////////////////////////////////////////
const std::string& AssetFile::GetP4User()
{
  if ( m_P4User.empty() )
  {
    RCS::FileInfo fileInfo( GetFilePath() );
    RCS::GetInfo( fileInfo, true );
    fileInfo.GetLastUser( m_P4User );
  }

  return m_P4User;
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::GetEngineTypeName( const Asset::AssetClass* assetClass, std::string& engineTypeName )
{
  if ( assetClass )
  {
    const Reflect::Enumeration* info = Reflect::Registry::GetInstance()->GetEnumeration( "EngineType" );
    if ( info && info->GetElementLabel( assetClass->GetEngineType(), engineTypeName ) )
    {
      return;
    }
  }
  engineTypeName = "Unknown";
}

/////////////////////////////////////////////////////////////////////////////
void AssetFile::SetAssetClass( const Asset::AssetClass* assetClass )
{
  if ( assetClass && m_AssetClass != assetClass )
  {
    if ( ( GetFileID() == TUID::Null )
      || ( GetFileID() != assetClass->GetFileID() ) )
    {
      File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( assetClass->GetFileID() );
      
      // this will also clear the asset cache
      SetManagedFile( file );
    }

    m_AssetClass = assetClass;
  }
}

/////////////////////////////////////////////////////////////////////////////
const std::string& AssetFile::GetRelativePath()
{
  if ( m_RelativePath.empty() )
  {
    m_RelativePath = GetFilePath();
    FileSystem::StripPrefix( File::GlobalManager().GetManagedAssetsRoot(), m_RelativePath );
  }

  return m_RelativePath;
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
void AssetFile::AddDependencyID( const tuid dependencyID )
{
  m_DependencyIDs.insert( dependencyID );
}


/////////////////////////////////////////////////////////////////////////////
void AssetFile::AddAttribute( const std::string& attrName, const std::string& attrValue )
{
  Nocturnal::Insert<M_string>::Result inserted = m_Attributes.insert( M_string::value_type( attrName, attrValue ) );
  if ( !inserted.second )
  {
    //NOC_BREAK();
  }
}

/////////////////////////////////////////////////////////////////////////////
const std::string& AssetFile::GetEngineTypeName()
{
  if ( m_EngineTypeName.empty() )
  {
    GetEngineTypeName( m_AssetClass, m_EngineTypeName );
  }

  return m_EngineTypeName;
}

/////////////////////////////////////////////////////////////////////////////
// Populates the stats with a call to get stats and converts time to milliTime
void AssetFile::PopulateStats( bool force )
{
  struct _stati64 fileStats;
  if ( FileSystem::GetStats64( GetFilePath(), fileStats ) )
  {
    m_Size = ( m_Size <= 0 || force ) ? fileStats.st_size : m_Size;
    m_PrintSize.clear();
  }
}

/////////////////////////////////////////////////////////////////////////////
u64 AssetFile::GetSize()
{
  if ( m_Size == 0 )
  {
    PopulateStats();
  }
  return m_Size;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& AssetFile::GetPrintSize()
{
  if ( m_PrintSize.empty() )
  {
    u64 size = GetSize();

    std::stringstream printSize;
    if ( size == 0 )
    {
      printSize << "0 KB";
    }
    else if ( size <= 1024 )
    {
      printSize << "1 KB";
    }
    else
    {
      size = size / 1024;
      printSize << size << " KB";
    }

    m_PrintSize = printSize.str();
  }
  return m_PrintSize;
}

/////////////////////////////////////////////////////////////////////////////
// 
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
      m_Extension = FileSystem::GetExtension( GetFilePath(), 1 );
      try
      {
        //std::string dumyFile( "dummy" );
        //dumyFile += m_Extension;
        //m_ModifierSpec = Finder::GetFileExtensionSpec( dumyFile );

        m_ModifierSpec = Finder::GetFileExtensionSpec( m_Extension );
      }
      catch( Finder::Exception & )
      {
      }
    }
  }
  return m_ModifierSpec;
}

/////////////////////////////////////////////////////////////////////////////
// Gets and caches the file type
const std::string& AssetFile::GetExtension()
{
  if ( m_Extension.empty() )
  {
    if ( GetModifierSpec() )
    {
      m_Extension = GetModifierSpec()->GetModifier();
    }
    else
    {
      m_Extension = FileSystem::GetExtension( GetFilePath(), 1 );
    }

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
// Gets the path to the ScreneShot from the art file path
//
// Example:
//   x:/rcf/assets/devel/maya/entities/shrubs/shrub0/shrub.mb
// becomes...
//   x:/rcf/assets/devel/maya/entities/shrubs/shrub0/shrub.jpg
//
const std::string& AssetFile::GetScreneShotPath()
{
  if ( m_ScreneShotPath.empty() )
  {
    const std::string& extension = GetExtension();

    if ( extension == FinderSpecs::Extension::REFLECT_BINARY.GetExtension() )
    {
      m_ScreneShotPath = GetFilePath();
      FinderSpecs::Extension::JPG.Modify( m_ScreneShotPath );
    }
    else if ( extension == FinderSpecs::Extension::PNG.GetExtension()
      || extension == FinderSpecs::Extension::TGA.GetExtension()
      || extension == FinderSpecs::Extension::JPG.GetExtension()
      || extension == FinderSpecs::Extension::GIF.GetExtension() )
    {
      m_ScreneShotPath = GetFilePath();
    }
    else if ( extension == FinderSpecs::Asset::ENTITY_DECORATION.GetDecoration() )
    {
      GetArtFile();

      if ( !m_ArtFile.empty() )
      {
        m_ScreneShotPath = m_ArtFile;
        FinderSpecs::Extension::JPG.Modify( m_ScreneShotPath );
      }
    }
    else if ( extension == FinderSpecs::Asset::SHADER_DECORATION.GetDecoration() )
    {
      Asset::AssetClassPtr assetClass = NULL;
      try
      {
        assetClass = Asset::AssetClass::FindAssetClass( GetFileID() );

        if ( assetClass.ReferencesObject() )
        {
          Asset::ColorMapAttributePtr colorMapFile = Reflect::ObjectCast< Asset::ColorMapAttribute >( assetClass->GetAttribute( Reflect::GetType< Asset::ColorMapAttribute >() ) );

          if ( colorMapFile.ReferencesObject() )
          {
            m_ScreneShotPath = colorMapFile->GetFilePath();
          }
        }       
      }
      catch( const Nocturnal::Exception& )
      {
        // do nothing
      }
    }
  }

  return m_ScreneShotPath;
}

/////////////////////////////////////////////////////////////////////////////
// Get's the art file for the entity class
//
const std::string& AssetFile::GetArtFile()
{
  if ( m_ArtFile.empty() )
  {
    Asset::AssetClassPtr assetClass = NULL;
    try
    {
      assetClass = Asset::AssetClass::FindAssetClass( GetFileID() );

      if ( assetClass.ReferencesObject() )
      {
        Attribute::AttributeViewer< Asset::ArtFileAttribute > artFile ( assetClass );
        if ( artFile.Valid() )
        {
          m_ArtFile = artFile->GetFilePath();
        }
      }       
    }
    catch( const Nocturnal::Exception& )
    {
      // do nothing
    }
  }

  return m_ArtFile;
}

/////////////////////////////////////////////////////////////////////////////
AssetClass* AssetFile::GetAssetClass()
{
  if ( m_AssetClass.ReferencesObject() )
  {
    return m_AssetClass;
  }
  else
  {
    Asset::AssetClassPtr assetClass = NULL;
    try
    {
      assetClass = Asset::AssetClass::FindAssetClass( GetFileID() );

      if ( assetClass.ReferencesObject() )
      {
        return assetClass;
      }       
    }
    catch( const Nocturnal::Exception& )
    {
      // do nothing
    }
    return NULL;
  }
}

/////////////////////////////////////////////////////////////////////////////
File::ManagedFile* AssetFile::GetManagedFile()
{
  return m_File;
}