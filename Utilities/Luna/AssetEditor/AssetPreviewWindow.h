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
    void OnShaderComponentdChanged( const ComponentExistenceArgs& args );
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
    
    std::set< std::string > m_WatchMeshes;
    std::set< std::string > m_WatchShaders;
    std::set< std::string > m_WatchTextures;

    std::set< std::string > m_ReloadMeshes;
    std::set< std::string > m_ReloadShaders;
    std::set< std::string > m_ReloadTextures;
    Nocturnal::FileChangedSignature::Delegate m_FileChangedDelegate;
    
    wxTimer m_FileWatcherTimer;
  };
}
