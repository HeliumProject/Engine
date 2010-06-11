#pragma once

#include "AssetManager.h"
#include "ShaderAsset.h"

#include "Foundation/File/FileWatcher.h"
#include "Editor/PreviewWindow.h"
#include "Platform/Thread.h"


namespace Luna
{
  struct D3DEventArgs;

  class AssetPreviewWindow : public PreviewWindow
  {
  public:
    AssetPreviewWindow( AssetManager* manager, wxWindow *parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE, const wxString& name = "Luna::AssetPreviewWindow" );
    virtual ~AssetPreviewWindow();

    void SetupScene( std::string meshPath );
    void RemoveScene();
  
    virtual void DisplayReferenceAxis( bool display ) NOC_OVERRIDE;

  private:
    void UpdateShader( Asset::ShaderAsset* shaderClass );
    void OnAssetLoaded( const AssetLoadArgs& args );
    void OnAssetUnloaded( const AssetLoadArgs& args );
    void OnShaderChanged( const ShaderChangedArgs& args );
    void OnShaderAttributedChanged( const AttributeExistenceArgs& args );
    void OnMeshUpdated( const Nocturnal::FileChangedArgs& e );
    void OnShaderUpdated( const Nocturnal::FileChangedArgs& e );
    void OnTextureUpdated( const Nocturnal::FileChangedArgs& e );
    void OnWatchFiles( wxTimerEvent& args );
    void OnPreferencesChanged( const Reflect::ElementChangeArgs& args );
    
    void AddFileChangedListeners();
    void RemoveFileChangedListeners();

    AssetManager* m_Manager;
    Nocturnal::FileWatcher m_FileWatcher;

    std::set< Luna::ShaderAsset* > m_ListenShaders;
    
    S_string m_WatchMeshes;
    S_string m_WatchShaders;
    S_string m_WatchTextures;

    S_string m_ReloadMeshes;
    S_string m_ReloadShaders;
    S_string m_ReloadTextures;
    Nocturnal::FileChangedSignature::Delegate m_FileChangedDelegate;
    
    wxTimer m_FileWatcherTimer;
  };
}
