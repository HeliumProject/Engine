#pragma once

namespace Luna
{
  // Forwards
  class AssetEditor;

  /////////////////////////////////////////////////////////////////////////////
  // Helper functions for doing lengthy animation operations.
  // 
  namespace AnimationHelper
  {
    bool CreateDefaultMayaFile( const std::string& path, std::string& error );
    void AddNewAnimationGroup( AssetEditor* assetEditor );
    void EditAnimationGroup( AssetEditor* assetEditor );
    void AddClipToNewChain( AssetEditor* assetEditor );
    void AddClipToExistingChain( AssetEditor* assetEditor );
  }
};
