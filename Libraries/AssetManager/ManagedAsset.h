#pragma once

#include "API.h"

#include "Asset/AssetClass.h"
#include "Asset/AssetFlags.h"
#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"
#include "File/Manager.h"
#include "Reflect/Field.h"

namespace AssetManager 
{
  ///////////////////////////////////////////////////////////////////////////// 
  // OperationFlags
  //
  namespace OperationFlags
  {
    enum OperationFlag
    {
      Create,
      Duplicate,
      Rename,
      Delete,

      Count,
    };
  }
  typedef OperationFlags::OperationFlag OperationFlag;

  ///////////////////////////////////////////////////////////////////////////// 
  // ConfigOptions see Asset/AssetFlags.h
  
  typedef ASSETMANAGER_API i32 ConfigOptions;

  static const ConfigOptions AllConfigOptions = ( Asset::AssetFlags::CreateSubFolder | Asset::AssetFlags::CannotOverrideCSF | Asset::AssetFlags::PerformOperation | Asset::AssetFlags::CannotOverridePO | Asset::AssetFlags::EnsureParentDir | Asset::AssetFlags::CannotOverrideEPD );  

  ///////////////////////////////////////////////////////////////////////////// 
  // ManagedAssetFile

  class ASSETMANAGER_API ManagedAssetFile : public Nocturnal::RefCountBase< ManagedAssetFile >
  {
  public:
    ConfigOptions         m_ConfigOptions;

    File::ManagedFilePtr  m_File;
    File::ManagedFilePtr  m_NewFile;      // Used when m_OperationFlag is either OperationFlags::Duplicate or OperationFlags::Rename

    M_string              m_RelatedFiles; // mapping of filePath to newPath files that are related to the ManagedAssetFile, like export files

    // Ctor
    ManagedAssetFile( ConfigOptions configOptions )
      : m_ConfigOptions( configOptions )
    {
    }
  };
  ASSETMANAGER_API typedef Nocturnal::SmartPtr< ManagedAssetFile > ManagedAssetFilePtr;
  ASSETMANAGER_API typedef std::map< tuid, ManagedAssetFilePtr > M_ManagedAssetFiles;


  /////////////////////////////////////////////////////////////////////////////
  // ManagedAsset
  // 
  class ASSETMANAGER_API ManagedPackage
  {
  public: 
    OperationFlag         m_OperationFlag;

    std::string           m_Path;
    std::string           m_NewPath;    // Used when m_OperationFlag is either OperationFlags::Duplicate or OperationFlags::Rename

    bool                  m_UseNewPath;
    M_ManagedAssetFiles   m_ManagedAssetFiles;

    ManagedPackage( OperationFlag operationFlag, const std::string& path = std::string( "" ), const std::string& newPath = std::string( "" ) )
      : m_OperationFlag( operationFlag )
      , m_Path( path )
      , m_NewPath( newPath )
      , m_UseNewPath( false )
    {
      m_UseNewPath = ( ( operationFlag == OperationFlags::Rename ) || ( operationFlag == OperationFlags::Duplicate ) );
    }
  };


  /////////////////////////////////////////////////////////////////////////////
  // ManagedAsset
  // 
  class ASSETMANAGER_API ManagedAsset : public ManagedPackage, public Nocturnal::RefCountBase< ManagedAsset >
  {
  public: 
    Asset::AssetClassPtr  m_Asset;

    ManagedAsset( Asset::AssetClass* assetClass, OperationFlag operationFlag, const std::string& path = std::string( "" ), const std::string& newPath = std::string( "" ) )
      : ManagedPackage( operationFlag, path, newPath )
      , m_Asset( assetClass )
    {
      if ( m_Path.empty() && m_Asset.ReferencesObject() )
      {
        m_Path = m_Asset->GetFilePath();
      }
    }
  };
  ASSETMANAGER_API typedef Nocturnal::SmartPtr< ManagedAsset > ManagedAssetPtr;
  ASSETMANAGER_API typedef std::vector< ManagedAssetPtr > V_ManagedAsset;


  /////////////////////////////////////////////////////////////////////////////
  // ManagedFolder
  // 
  class ASSETMANAGER_API ManagedFolder : public ManagedPackage, public Nocturnal::RefCountBase< ManagedFolder >
  {
  public:
    V_ManagedAsset        m_ManagedAssets;
    M_string              m_AdditionalFiles;
    
    ManagedFolder( OperationFlag operationFlag, const std::string& path, const std::string& newPath = std::string( "" ) )
      : ManagedPackage( operationFlag, path, newPath )
    {
    }
  };
  ASSETMANAGER_API typedef Nocturnal::SmartPtr< ManagedFolder > ManagedFolderPtr;

} // namespace AssetManager
