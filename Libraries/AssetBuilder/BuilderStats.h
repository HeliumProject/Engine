#pragma once

#include "Common/Types.h"

#include "UID/TUID.h"

#include "BuilderInterface.h"

namespace BuilderStats
{
  class TopLevelBuild
  {
  public:
    f32 m_DependencyCheckTime;
    f32 m_DownloadTime;
    f32 m_UploadTime;
    f32 m_BuildTime;
    f32 m_InitializationTime;
    f32 m_JobGatheringTime;
    f32 m_SignatureCreationTime;
    f32 m_TotalTime;
    f32 m_UnaccountedTime;

    TopLevelBuild()
    {
      m_DependencyCheckTime = 0.0f;
      m_DownloadTime = 0.0f;
      m_UploadTime = 0.0f;
      m_BuildTime = 0.0f;
      m_InitializationTime = 0.0f;
      m_JobGatheringTime = 0.0f;
      m_SignatureCreationTime = 0.0f;
      m_TotalTime = 0.0f;
      m_UnaccountedTime = 0.0f;
    }
  };

  void Initialize();
  void Cleanup();

  bool AddBuild( const Nocturnal::Path& path, Asset::AssetType assetType, const std::string& builderName, f32 duration );
  bool AddTopLevelBuild( const Nocturnal::Path& path, Asset::AssetType assetType, TopLevelBuild& topLevelBuild );
}