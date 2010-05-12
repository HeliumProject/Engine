#include "stdafx.h"

#include "ManagedAsset.h"
#include "GetManagedAssetVisitor.h"
#include "AssetManager.h"

#include "Asset/Exceptions.h"
#include "Common/Container/Insert.h" 
#include "Common/Flags.h"
#include "Common/Types.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "RCS/RCS.h"
#include "Reflect/Class.h"
#include "Reflect/Field.h"
#include "Reflect/Serializers.h"
#include "Reflect/Version.h"
#include "Reflect/Visitor.h"
#include "TUID/TUID.h"

using namespace AssetManager;
using Nocturnal::Insert; 
using namespace Asset;
using namespace Asset::AssetFlags;

using namespace AssetManager;

/////////////////////////////////////////////////////////////////////////////
GetManagedAssetVisitor::GetManagedAssetVisitor( ManagedAsset* managedAsset, u32 assetFlags )
 : m_ManagedAsset( managedAsset )
 , m_AssetFlags( assetFlags )
{
}

/////////////////////////////////////////////////////////////////////////////
bool GetManagedAssetVisitor::VisitField( Reflect::Element* element, const Reflect::Field* field )
{
  if ( Nocturnal::HasFlags<i32>( field->m_Flags, Reflect::FieldFlags::Discard )
    || !Nocturnal::HasFlags<i32>( field->m_Flags, AssetFlags::ManageField  ) )
  {
    return true;
  }

  if ( ( field->m_Flags & Reflect::FieldFlags::FileID ) != 0 )
  {
    //-----------------------------------------------
    if ( field->m_SerializerID == Reflect::GetType< Reflect::U64Serializer >() )
    {
      tuid id;
      Reflect::Serializer::GetValue( field->CreateSerializer( element ), id );

      if ( id == m_ManagedAsset->m_Asset->m_AssetClassID )
        return true;

      ProcessIdField( element, field, id );
    }
    //-----------------------------------------------
    else if ( field->m_SerializerID == Reflect::GetType< Reflect::U64ArraySerializer >() )
    {
      Reflect::SerializerPtr serializer = field->CreateSerializer( element );

      V_tuid vals;
      Reflect::Serializer::GetValue( serializer, vals );

      for each ( const tuid& id in vals )
      {
        ProcessIdField( element, field, id );
      }
    }
    //-----------------------------------------------
    else if ( field->m_SerializerID == Reflect::GetType< Reflect::U64SetSerializer >() )
    {
      Reflect::SerializerPtr serializer = field->CreateSerializer( element );

      S_tuid vals;
      Reflect::Serializer::GetValue( serializer, vals );

      for each ( const tuid& id in vals )
      {
        ProcessIdField( element, field, id );
      }
    }
    else
    {
      NOC_BREAK();
      throw File::Exception( "This field type '%s::%s' has no support for the file manager",
        field->m_Type->m_ShortName.c_str(),
        field->m_Name.c_str() );
    }
  }

  // continue search
  return true;
}

/////////////////////////////////////////////////////////////////////////////
void GetManagedAssetVisitor::ProcessIdField( const Reflect::ElementPtr& element, const Reflect::Field* field, tuid id )
{
  if ( id == TUID::Null )
    return;

  Insert<M_ManagedAssetFiles>::Result inserted = m_ManagedAsset->m_ManagedAssetFiles.insert( M_ManagedAssetFiles::value_type( id, new ManagedAssetFile( AllConfigOptions & field->m_Flags ) ) );
  if ( inserted.second )
  {
    GetManagedAssetFile( element, field, id, inserted.first->second );
  }
}

/////////////////////////////////////////////////////////////////////////////
// Helper function for VisitField which creates a ManagedAssetFile
void GetManagedAssetVisitor::GetManagedAssetFile( const Reflect::ElementPtr& element, const Reflect::Field* field, tuid id, ManagedAssetFile* managedAssetFile )
{
  managedAssetFile->m_File = File::GlobalManager().GetManagedFile( id );
  managedAssetFile->m_NewFile = new File::ManagedFile();

  // best guess new path 
  if( !m_ManagedAsset->m_Path.empty() )
  {
    BestGuessNewAssetPath( m_ManagedAsset, managedAssetFile, Nocturnal::HasFlags<i32>( managedAssetFile->m_ConfigOptions, AssetFlags::CreateSubFolder ) );
  }

  RCS::File rcsFile( managedAssetFile->m_File->m_Path );
  rcsFile.Sync();

  // this tuid may represent an asset file
  if ( FileSystem::HasExtension( managedAssetFile->m_File->m_Path, FinderSpecs::Extension::REFLECT_BINARY.GetExtension() ) )
  {
      Asset::AssetClassPtr assetClass = Asset::AssetClass::FindAssetClass( managedAssetFile->m_File->m_Id );
      if ( assetClass.ReferencesObject() )
      {
        assetClass->Host( *this );
      }
    }
}

///////////////////////////////////////////////////////////////////////////
// fix-up the asset name to match the new path name (if it's being renamed)
//
void GetManagedAssetVisitor::BestGuessNewAssetPath( ManagedAsset* managedAsset, ManagedAssetFile* managedAssetFile, bool addDirectory )
{
  // early out if they are not duplicating or renaming the file
  if ( managedAsset->m_OperationFlag == OperationFlags::Create
    || managedAsset->m_OperationFlag == OperationFlags::Delete
    || managedAsset->m_Path.empty() 
    || managedAsset->m_NewPath.empty() 
    || managedAssetFile->m_File->m_Path.empty() )
  {
    return;
  }

  ////////////////////////////////////
  // Maya Binary
  if ( FileSystem::HasExtension( managedAssetFile->m_File->m_Path, FinderSpecs::Extension::MAYA_BINARY.GetExtension() ) )
  {
    // Animation files
    std::string testAnimArtFilePath = managedAssetFile->m_File->m_Path;
    FileSystem::StripLeaf( testAnimArtFilePath );
    testAnimArtFilePath = FileSystem::GetLeaf( testAnimArtFilePath );

    if ( testAnimArtFilePath.find( "animation" ) != std::string::npos )
    {
      managedAssetFile->m_NewFile->m_Path = managedAsset->m_NewPath;
      FileSystem::StripLeaf( managedAssetFile->m_NewFile->m_Path );
      FileSystem::AppendPath( managedAssetFile->m_NewFile->m_Path, "animations" );
      FileSystem::AppendPath( managedAssetFile->m_NewFile->m_Path, FileSystem::GetLeaf( managedAssetFile->m_File->m_Path ) );
    }
    else
    {
      std::string artFileName = FileSystem::GetLeaf( managedAssetFile->m_File->m_Path );
      FileSystem::StripExtension( artFileName, 1 );

      bool isMaster = false;
      std::string::size_type masterPos = artFileName.find( "_master" );
      if ( masterPos != std::string::npos )
      {
        artFileName.erase( masterPos, 7 );
        isMaster = true;
      }

      std::string assetFileName = FileSystem::GetLeaf( managedAsset->m_Path );
      FileSystem::StripExtension( assetFileName, 2 );

      if ( artFileName == assetFileName )
      {
        managedAssetFile->m_NewFile->m_Path = managedAsset->m_NewPath;
        FileSystem::StripExtension( managedAssetFile->m_NewFile->m_Path, 2 );
        if ( isMaster )
        {
          managedAssetFile->m_NewFile->m_Path += "_master";
        }
        FileSystem::SetExtension( managedAssetFile->m_NewFile->m_Path, FinderSpecs::Extension::MAYA_BINARY.GetExtension() );
      }
      else
      {
        managedAssetFile->m_NewFile->m_Path = managedAsset->m_NewPath;
        FileSystem::StripLeaf( managedAssetFile->m_NewFile->m_Path );
        FileSystem::AppendPath( managedAssetFile->m_NewFile->m_Path, FileSystem::GetLeaf( managedAssetFile->m_File->m_Path ) );
      }
    }
  }
  ////////////////////////////////////
  // Animation Set
  else if ( FileSystem::HasExtension( managedAssetFile->m_File->m_Path, FinderSpecs::Asset::ANIMSET_DECORATION.GetDecoration() ) )
  {
    managedAssetFile->m_NewFile->m_Path = managedAsset->m_NewPath;
    FileSystem::StripLeaf( managedAssetFile->m_NewFile->m_Path );
    FileSystem::AppendPath( managedAssetFile->m_NewFile->m_Path, FileSystem::GetLeaf( managedAssetFile->m_File->m_Path ) );
  }
  ////////////////////////////////////
  // Animation
  else if ( FileSystem::HasExtension( managedAssetFile->m_File->m_Path, FinderSpecs::Asset::ANIM_DECORATION.GetDecoration() ) )
  {
    managedAssetFile->m_NewFile->m_Path = managedAsset->m_NewPath;
    FileSystem::StripLeaf( managedAssetFile->m_NewFile->m_Path );
    FileSystem::AppendPath( managedAssetFile->m_NewFile->m_Path, "animations" );
    FileSystem::AppendPath( managedAssetFile->m_NewFile->m_Path, FileSystem::GetLeaf( managedAssetFile->m_File->m_Path ) );
  }
  ////////////////////////////////////
  // Textures
  //    suggested location should remain in the same directory
  else if( FinderSpecs::Extension::TEXTUREMAP_FILTER.IsExtensionValid( FileSystem::GetExtension( managedAssetFile->m_File->m_Path ) ) )
  {
    managedAssetFile->m_NewFile->m_Path = managedAssetFile->m_File->m_Path;
  }
  ////////////////////////////////////
  // best guess new asset filePath
  else
  {
    std::string folder = managedAsset->m_Path;
    FileSystem::StripLeaf( folder );

    if ( FileSystem::HasPrefix( folder, managedAssetFile->m_File->m_Path ) )
    {
      std::string relativeFilePath = managedAssetFile->m_File->m_Path;
      FileSystem::StripPrefix( folder, relativeFilePath );

      managedAssetFile->m_NewFile->m_Path = managedAsset->m_NewPath;
      FileSystem::StripLeaf( managedAssetFile->m_NewFile->m_Path );    
      FileSystem::AppendPath( managedAssetFile->m_NewFile->m_Path, relativeFilePath );
    }
    else
    {
      managedAssetFile->m_NewFile->m_Path = managedAsset->m_NewPath;
      FileSystem::StripLeaf( managedAssetFile->m_NewFile->m_Path );
      FileSystem::AppendPath( managedAssetFile->m_NewFile->m_Path, FileSystem::GetLeaf( managedAssetFile->m_File->m_Path ) );
    }
  }
}
