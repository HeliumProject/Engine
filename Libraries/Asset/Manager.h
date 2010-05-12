#pragma once

#include "API.h"

#include "AssetClass.h"
#include "AssetFlags.h"
#include "AssetFile.h"
#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"
#include "File/Manager.h"
#include "TUID/TUID.h"

namespace Asset 
{
  namespace Manager
  {
    void ASSET_API Initialize();
    void ASSET_API Cleanup();

    void ASSET_API GetRelatedFiles( AssetFile* assetFile, S_string& relatedFiles );

    /*
    void ASSET_API GetManagedAsset( ManagedAsset* managedAsset, bool getRelatedFiles = true );
    void ASSET_API GetManagedAssetRelatedFiles( ManagedAsset* managedAsset );
    void ASSET_API GetManagedAssetShaders( AssetClass* assetClass, S_tuid& shaderIDs );

    void ASSET_API ListMayaFiles( const tuid id );
    void ASSET_API ListStaticContentFiles( const tuid id );
    void ASSET_API GetStaticContentFiles( const tuid id, V_string& staticContentFiles );
    void ASSET_API FindMayaFiles( const S_tuid& fileIdSet, bool recurse, File::S_ManagedFilePtr& mayaFiles );

    void ASSET_API GetManagedFolder( ManagedFolder* managedFolder, bool getRelatedFiles = true );
    void ASSET_API GetManagedFolderRelatedFiles( ManagedFolder* managedFolder );
   
    bool ASSET_API ValidateAssetOperation( ManagedAsset* managedAsset, std::string& errMsg );

    //
    // Move/Rename
    //
    void ASSET_API ProcessRenameAsset( ManagedAsset* managedAsset, const File::ManagerConfig managerConfig = File::ManagerConfigs::Default, bool rename = true );
    void ASSET_API UpdateRenameAsset( ManagedAsset* managedAsset, bool rename = true );

    //
    // Copy/Duplicate
    //
    void ASSET_API ProcessDuplicateAsset( ManagedAsset* managedAsset, const File::ManagerConfig managerConfig = File::ManagerConfigs::Default );
    Asset::AssetClassPtr ASSET_API UpdateDuplicateAsset( ManagedAsset* managedAsset );

    //
    // Delete
    //
    void ASSET_API ProcessDeleteAsset( ManagedAsset* managedAsset, const File::ManagerConfig managerConfig = File::ManagerConfigs::Default );
    
    //
    // Migrate
    //
    void ASSET_API MigrateFile( const File::ManagedFilePtr& sourceAssetFile, File::Manager* sourceManager, const File::ManagerConfig managerConfig = File::ManagerConfigs::Default );
  */
  } // namespace Manager
} // namespace Asset
