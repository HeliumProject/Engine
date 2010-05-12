#include "stdafx.h"
#include "FileInfo.h"

#include "Attribute/AttributeHandle.h"
#include "Asset/ArtFileAttribute.h"
#include "Asset/ColorMapAttribute.h"
#include "Asset/AssetClass.h"

#include "Common/String/Utilities.h"

#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/FinderSpec.h"
#include "Finder/ProjectSpecs.h"

namespace File
{ 

  REFLECT_DEFINE_CLASS( FileInfo );

  void FileInfo::EnumerateClass( Reflect::Compositor<FileInfo>& comp )
  {
  }


  ///////////////////////////////////////////

  static const i32 s_DisplayTimeSize = 32;
  static const u32 InvalidLegacyID = -1;


  ///////////////////////////////////////////////////////////////////////////////
  // Ctor/Dtor
  //
  FileInfo::FileInfo( const tuid id )
    : ManagedFile() 
    , m_Size( 0 )
  {
    m_Id = id;
  }

  FileInfo::FileInfo( const std::string& path )
    : ManagedFile()
    , m_Size( 0 )
  {
    m_Path = path;
  }

  FileInfo::~FileInfo()
  {
  }



  /////////////////////////////////////////////////////////////////////////////
  // Overloaded = operator only compares the file paths
  bool FileInfo::operator==( const FileInfo &rhs )
  {
    return ( m_Path.compare( rhs.m_Path ) == 0 );
  }


  /////////////////////////////////////////////////////////////////////////////
  // Gets and caches the path to the file 
  std::string FileInfo::GetPath()
  {
    if ( m_Path.empty() && GetID() != TUID::Null )
    {
      File::GlobalManager().GetPath( GetID(), m_Path );
    }

    return m_Path;
  }


  /////////////////////////////////////////////////////////////////////////////
  // Gets and caches the leaf portion of the full file path
  std::string FileInfo::GetName()
  {
    if ( m_Name.empty() )
    {
      FileSystem::GetLeaf( GetPath(), m_Name );
    }
    return m_Name;
  }


  /////////////////////////////////////////////////////////////////////////////
  // Gets and caches the file type
  std::string FileInfo::GetExtension()
  {
    if ( m_Extension.empty() )
    {
      try
      {
        m_Extension = Finder::GetFileExtensionSpec( GetName() )->GetModifier();
      }
      catch( Finder::Exception & )
      {
        m_Extension = FileSystem::GetExtension( GetName(), 1 );
      }

    }
    return m_Extension;
  }


  /////////////////////////////////////////////////////////////////////////////
  // Gets and caches the file type
  std::string FileInfo::GetFileType()
  {
    if ( m_FileType.empty() )
    {
      try
      {
        m_FileType = Finder::GetFileExtensionSpec( GetName() )->GetUIName();
      }
      catch( Finder::Exception & )
      {
        m_FileType = FileSystem::GetExtension( GetName(), 1 );
        if ( !m_FileType.empty() && *m_FileType.begin() == '.' )
        {
          m_FileType.erase( 0, 1 );
        }

        if ( m_FileType.empty() )
        {
          m_FileType = "UNKNOWN";
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
  // Gets and caches the folder portion of the full file path
  std::string FileInfo::GetFolder()
  {
    if ( m_Folder.empty() )
    {
      m_Folder = GetPath();
      FileSystem::StripLeaf( m_Folder );
    }
    return m_Folder;
  }


  /////////////////////////////////////////////////////////////////////////////
  // 
  u64 FileInfo::GetCreated()
  {
    return Created();
  }

  u64 FileInfo::Created()
  {
    if ( m_Created <= 0 )
    {
      PopulateStats();
    }
    return m_Created;
  }

  // Pretty prints time
  std::string FileInfo::GetPrintCreated()
  {
    return GetPrintTime( GetCreated() );
  }

  std::string FileInfo::GetCreatedBy()
  {
    if ( m_CreatedBy.empty() )
    {
      if ( m_Username.empty() && m_UserId != 0 )
      {
        File::GlobalManager().GetUsernameByID( m_UserId, m_Username );
      }
        
      m_CreatedBy = m_Username;
    }
    return m_CreatedBy;
  }



  /////////////////////////////////////////////////////////////////////////////
  // 
  u64 FileInfo::GetModified()
  {
    return Modified();
  }

  u64 FileInfo::Modified()
  {
    if ( m_Modified <= 0 )
    {
      PopulateStats();
    }
    return m_Modified;
  }

  // Pretty prints time
  std::string FileInfo::GetPrintModified()
  {
    return GetPrintTime( GetModified() );
  }


  /////////////////////////////////////////////////////////////////////////////
  // Pretty prints time
  bool FileInfo::GetHistory()
  {
    if ( m_History.empty() )
    {
      try
      {
        File::GlobalManager().GetFileHistory( this, m_History );
      }
      catch ( File::Exception & )
      {
        return false;
      }
    }
    return ( !m_History.empty() );
  }


  /////////////////////////////////////////////////////////////////////////////
  // Pretty prints time
  std::string FileInfo::GetPrintTime( u64 time )
  {
    std::string result;

    // try to get a printer friendly version of the datetime
    __time64_t timeT  = ( __time64_t ) ( time / 1000 );

    char timePrint[s_DisplayTimeSize];
    if ( _ctime64_s( timePrint, s_DisplayTimeSize, &timeT ) == 0 )
    {
      // timeT
      result = timePrint;
      result = result.substr( 0, result.length() - 1 );
    }
    else
    {
      sprintf( timePrint, "%ld", time );
      result = timePrint;
    }    
    return result;
  }

  /////////////////////////////////////////////////////////////////////////////
  // 
  u64 FileInfo::GetSize()
  {
    if ( m_Size == 0 )
    {
      PopulateStats();
    }
    return m_Size;
  }


  /////////////////////////////////////////////////////////////////////////////
  std::string FileInfo::GetPrintSize()
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

    return printSize.str();
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns the printable ID
  tuid FileInfo::GetID()
  {
    if ( m_Id == TUID::Null && !GetPath().empty() )
    {
      try
      {
        m_Id = File::GlobalManager().GetID( GetPath() );
      }
      catch( const Nocturnal::Exception& )
      {

      }
    }

    return m_Id;
  }


  /////////////////////////////////////////////////////////////////////////////
  // Returns the printable ID
  std::string FileInfo::GetPrintID()
  {
    if ( m_PrintName.empty() )
    {
      std::stringstream strID;
      strID << TUID::HexFormat << GetID();
      m_PrintName = strID.str();
    }
    return m_PrintName;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Gets the path to the thumbnail from the art file path
  //
  // Example:
  //   x:/rcf/assets/devel/maya/entities/shrubs/shrub0/shrub.mb
  // becomes...
  //   x:/rcf/assets/devel/maya/entities/shrubs/shrub0/shrub.jpg
  //
  std::string FileInfo::GetThumbnailPath( )
  {
    if ( m_ThumbnailPath.empty() )
    {
      std::string extension = FileSystem::GetExtension( GetName() );

      if ( extension == FinderSpecs::Extension::MAYA_BINARY.GetExtension() )
      {
        m_ThumbnailPath = GetPath();
        FileSystem::StripExtension( m_ThumbnailPath );
        m_ThumbnailPath += ".jpg";
      }
      else if ( extension == FinderSpecs::Extension::PNG.GetExtension()
        || extension == FinderSpecs::Extension::TGA.GetExtension()
        || extension == FinderSpecs::Extension::JPG.GetExtension()
        || extension == FinderSpecs::Extension::GIF.GetExtension() )
      {
        m_ThumbnailPath = GetPath();
      }
      else if ( extension == FinderSpecs::Asset::ENTITY_DECORATION.GetDecoration() )
      {
        GetArtFile();

        if ( !m_ArtFile.empty() )
        {
          m_ThumbnailPath = m_ArtFile;
          FileSystem::StripExtension( m_ThumbnailPath );
          m_ThumbnailPath += ".jpg";
        }
      }
      else if ( extension == FinderSpecs::Asset::SHADER_DECORATION.GetDecoration() )
      {
        Asset::AssetClassPtr assetClass = NULL;
        try
        {
          assetClass = Asset::AssetClass::FindAssetClass( GetID() );

          if ( assetClass.ReferencesObject() )
          {
            Asset::ColorMapAttributePtr colorMapFile = Reflect::ObjectCast< Asset::ColorMapAttribute >( assetClass->GetAttribute( Reflect::GetType< Asset::ColorMapAttribute >() ) );

            if ( colorMapFile.ReferencesObject() )
            {
              m_ThumbnailPath = colorMapFile->GetFilePath();
            }
          }       
        }
        catch( const Nocturnal::Exception& )
        {
          // do nothing
        }
      }
    }

    return m_ThumbnailPath;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Get's the art file for the entity class
  //
  std::string FileInfo::GetArtFile( )
  {
    if ( m_ArtFile.empty() )
    {
      Asset::AssetClassPtr assetClass = NULL;
      try
      {
        assetClass = Asset::AssetClass::FindAssetClass( GetID() );

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
  // Populates the stats with a call to get stats and converts time to milliTime
  void FileInfo::PopulateStats( bool force )
  {
    struct _stati64 fileStats;

    if ( FileSystem::GetStats64( GetPath(), fileStats ) )
    {
      m_Size      = ( m_Size <= 0 || force ) ? fileStats.st_size : m_Size;
      m_Modified  = ( m_Modified <= 0 || force ) ? ( fileStats.st_mtime * 1000 ) : m_Modified ;
      m_Created   = ( m_Created <= 0 || force ) ? ( fileStats.st_ctime * 1000 ) : m_Created;
    }
    // else: do nothing for now
  }


} // namespace File