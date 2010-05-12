#pragma once

#include <vector>

#include "Common/Exception.h" 
#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"
#include "File/ManagedFile.h"
#include "SQL/SQLiteDB.h"
#include "tuid/tuid.h"


namespace Asset
{
  class AssetClass;
}

namespace AssetManager 
{ 
  namespace ShaderUsages
  {
    void Initialize();
    void Cleanup();

    void ListAssetsShaders( const std::string& assetFilePath, File::V_ManagedFilePtr& shaderFiles );
    void UsesShader( const std::string& shaderFilePath, File::V_ManagedFilePtr& assetFiles );
    void LoadShaderUsagesDB();
  }
}