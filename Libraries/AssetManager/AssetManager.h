#pragma once

#include "API.h"
#include "ManagedAsset.h"

#include "Asset/AssetClass.h"
#include "Asset/AssetFlags.h"
#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"
#include "File/Manager.h"
#include "TUID/TUID.h"

namespace AssetManager 
{
  void ASSETMANAGER_API Initialize();
  void ASSETMANAGER_API Cleanup();

  /////////////////////////////////////////////////////////////////////////////

  void ASSETMANAGER_API GetManagedAsset( ManagedAsset* managedAsset, bool getRelatedFiles = true );
  void ASSETMANAGER_API GetManagedAssetRelatedFiles( ManagedAsset* managedAsset );

  void ASSETMANAGER_API ListMayaFiles( const tuid id );
  void ASSETMANAGER_API ListStaticContentFiles( const tuid id );
  void ASSETMANAGER_API GetStaticContentFiles( const tuid id, V_string& staticContentFiles );
  void ASSETMANAGER_API FindMayaFiles( const S_tuid& fileIdSet, bool recurse, File::S_ManagedFilePtr& mayaFiles, bool needsToExist = true );

  void ASSETMANAGER_API GetManagedFolder( ManagedFolder* managedFolder, bool getRelatedFiles = true );
  void ASSETMANAGER_API GetManagedFolderRelatedFiles( ManagedFolder* managedFolder );
 
  bool ASSETMANAGER_API ValidateAssetOperation( ManagedAsset* managedAsset, std::string& errMsg );

  //
  // Move/Rename
  //
  void ASSETMANAGER_API ProcessRenameAsset( ManagedAsset* managedAsset, const File::ManagerConfig managerConfig = File::ManagerConfigs::Default, bool rename = true );

  //
  // Copy/Duplicate
  //
  void ASSETMANAGER_API ProcessDuplicateAsset( ManagedAsset* managedAsset, const File::ManagerConfig managerConfig = File::ManagerConfigs::Default );
  Asset::AssetClassPtr ASSETMANAGER_API UpdateDuplicateAsset( ManagedAsset* managedAsset );

  //
  // Delete
  //
  void ASSETMANAGER_API ProcessDeleteAsset( ManagedAsset* managedAsset, const File::ManagerConfig managerConfig = File::ManagerConfigs::Default );
} // namespace AssetManager
