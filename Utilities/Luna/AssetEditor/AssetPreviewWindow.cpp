#include "Precompile.h"

#include "AssetPreviewWindow.h"
#include "AssetPreferences.h"

#include "Pipeline/Asset/Components/StandardColorMapComponent.h"
#include "Pipeline/Asset/Components/StandardNormalMapComponent.h"
#include "Pipeline/Asset/Components/StandardExpensiveMapComponent.h"
#include "Pipeline/Asset/Components/StandardDetailMapComponent.h"

#include "Render/RBShaderLoader.h"
#include "Render/RBObjectLoader.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AssetPreviewWindow::AssetPreviewWindow( AssetManager* manager, wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: PreviewWindow( parent, winid, pos, size, style, name )
, m_Manager( manager )
{
  m_Manager->AddAssetLoadedListener( AssetLoadSignature::Delegate ( this, &AssetPreviewWindow::OnAssetLoaded ) );
  m_Manager->AddAssetUnloadingListener( AssetLoadSignature::Delegate ( this, &AssetPreviewWindow::OnAssetUnloaded ) );

  AssetPreferences* preferences = GetAssetEditorPreferences();
  preferences->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &AssetPreviewWindow::OnPreferencesChanged ) );
  __super::DisplayReferenceAxis( preferences->DisplayPreviewAxis() );
 
  m_FileWatcherTimer.SetOwner( this );
  Connect( m_FileWatcherTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( AssetPreviewWindow::OnWatchFiles ), NULL, this );
  m_FileWatcherTimer.Start( 100 );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AssetPreviewWindow::~AssetPreviewWindow()
{
  m_FileWatcherTimer.Stop();
  Disconnect( m_FileWatcherTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( AssetPreviewWindow::OnWatchFiles ), NULL, this );

  RemoveFileChangedListeners();
  
  AssetPreferences* preferences = GetAssetEditorPreferences();
  preferences->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &AssetPreviewWindow::OnPreferencesChanged ) );

  m_Manager->RemoveAssetLoadedListener( AssetLoadSignature::Delegate ( this, &AssetPreviewWindow::OnAssetLoaded ) );
  m_Manager->RemoveAssetUnloadingListener( AssetLoadSignature::Delegate ( this, &AssetPreviewWindow::OnAssetUnloaded ) );
}

///////////////////////////////////////////////////////////////////////////////
// SetupScene is used to load a new mesh in the preview window.
// 
void AssetPreviewWindow::SetupScene( std::string meshPath )
{
  LoadScene( meshPath );

  if ( m_Scene )
  {
    m_WatchMeshes.insert( meshPath );
    
    V_string shaderFiles;
    m_Scene->m_renderer->m_shader_manager.GetShaderFilenames( shaderFiles );
    for ( V_string::iterator itr = shaderFiles.begin(), end = shaderFiles.end(); itr != end; ++itr )
    {
      m_WatchShaders.insert( *itr );
    }

    V_string textureFiles;
    m_Scene->m_renderer->m_shader_manager.GetTextureFilenames( textureFiles );
    for ( V_string::iterator itr = textureFiles.begin(), end = textureFiles.end(); itr != end; ++itr )
    {
      m_WatchTextures.insert( *itr );
    }
    
    AddFileChangedListeners();
    
    Refresh();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Clear the scene and stop listening for files to be watched.
// 
void AssetPreviewWindow::RemoveScene()
{
  RemoveFileChangedListeners();

  m_WatchMeshes.clear();
  m_WatchShaders.clear();
  m_WatchTextures.clear();
  
  ClearScene();
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to pass the setting to the preferences.
// 
void AssetPreviewWindow::DisplayReferenceAxis( bool display )
{
  AssetPreferences* preferences = GetAssetEditorPreferences();
  NOC_ASSERT( preferences );

  preferences->SetDisplayPreviewAxis( display );
}

///////////////////////////////////////////////////////////////////////////////
// This function does the work of updating the shader in the renderer.
// 
void AssetPreviewWindow::UpdateShader( Asset::ShaderAsset* shaderClass )
{
  if ( m_Scene )
  {
    std::string shaderPath = shaderClass->GetFilePath().c_str();
    u32 shaderHandle = m_Scene->m_renderer->m_shader_manager.FindShader( shaderPath.c_str() );
    if ( shaderHandle == 0xffffffff )
    {
      return;
    }

    Render::Shader* shader = m_Scene->m_renderer->m_shader_manager.ResolveShader( shaderHandle );
    NOC_ASSERT( shader );
    
    Content::RBShaderLoader::UpdateShaderClass( &m_Scene->m_renderer->m_shader_manager, shaderPath.c_str(), shaderClass->m_AlphaMode );
    
    if ( shaderClass->m_DoubleSided )
    {
      shader->m_flags |= SHDR_FLAG_TWO_SIDED;
    }
    else
    {
      shader->m_flags &= ~SHDR_FLAG_TWO_SIDED;
    }

    Asset::StandardColorMapComponent* colorMap = shaderClass->GetComponent< Asset::StandardColorMapComponent >();
    if ( colorMap )
    {
      Render::TextureSettings settings;
      settings.Clear();

      settings.m_Path = colorMap->GetPath().Get();
      settings.m_Anisotropy = 0;
      settings.m_MipBias = colorMap->m_MipBias;
      Content::RBShaderLoader::SetWrapUV( &settings, shaderClass->m_WrapModeU, shaderClass->m_WrapModeV );
      Content::RBShaderLoader::SetFilter( &settings, colorMap->m_TexFilter );
      Content::RBShaderLoader::SetColorFormat( &settings, colorMap->m_TexFormat, Render::Texture::SAMPLER_BASE_MAP );
      
      m_Scene->m_renderer->m_shader_manager.UpdateShaderTexture( shaderPath.c_str(), Render::Texture::SAMPLER_BASE_MAP, settings );
    }
    else
    {
      m_Scene->m_renderer->m_shader_manager.SetShaderDefaultTexture( shaderPath.c_str(), Render::Texture::SAMPLER_BASE_MAP );
    }
    
    Content::RBShaderLoader::UpdateShaderColorMap( shader, colorMap );

    Asset::StandardNormalMapComponent* normalMap = shaderClass->GetComponent< Asset::StandardNormalMapComponent >();
    if ( normalMap )
    {
      Render::TextureSettings settings;
      settings.Clear();
      
      settings.m_Path = normalMap->GetPath().Get();
      settings.m_Anisotropy = 0;
      settings.m_MipBias = normalMap->m_MipBias;
      Content::RBShaderLoader::SetWrapUV( &settings, shaderClass->m_WrapModeU, shaderClass->m_WrapModeV );
      Content::RBShaderLoader::SetFilter( &settings, colorMap->m_TexFilter );
      Content::RBShaderLoader::SetColorFormat( &settings, colorMap->m_TexFormat, Render::Texture::SAMPLER_NORMAL_MAP );
      
      m_Scene->m_renderer->m_shader_manager.UpdateShaderTexture( shaderPath.c_str(), Render::Texture::SAMPLER_NORMAL_MAP, settings );
    }
    else
    {
      m_Scene->m_renderer->m_shader_manager.SetShaderDefaultTexture( shaderPath.c_str(), Render::Texture::SAMPLER_BASE_MAP );
    }

    Content::RBShaderLoader::UpdateShaderNormalMap( shader, normalMap );

    Asset::StandardExpensiveMapComponent* expensiveMap = shaderClass->GetComponent< Asset::StandardExpensiveMapComponent >();
    if ( expensiveMap )
    {
      Render::TextureSettings settings;
      settings.Clear();

      settings.m_Path = expensiveMap->GetPath().Get();
      settings.m_Anisotropy = 0;
      settings.m_MipBias = expensiveMap->m_MipBias;
      Content::RBShaderLoader::SetWrapUV( &settings, shaderClass->m_WrapModeU, shaderClass->m_WrapModeV );
      Content::RBShaderLoader::SetFilter( &settings, colorMap->m_TexFilter );
      Content::RBShaderLoader::SetColorFormat( &settings, colorMap->m_TexFormat, Render::Texture::SAMPLER_GPI_MAP );

      if (expensiveMap->m_DetailMapMaskEnabled && settings.m_Format == D3DFMT_DXT1)
      {
        settings.m_Format = D3DFMT_DXT5;
      }

      m_Scene->m_renderer->m_shader_manager.UpdateShaderTexture( shaderPath.c_str(), Render::Texture::SAMPLER_GPI_MAP, settings );
    }
    else
    {
      m_Scene->m_renderer->m_shader_manager.SetShaderDefaultTexture( shaderPath.c_str(), Render::Texture::SAMPLER_BASE_MAP );
    }

    Content::RBShaderLoader::UpdateShaderExpensiveMap( shader, expensiveMap );
    
    Refresh();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when an asset is loaded. Check if it's a shader and listen
// for updates so we can live update the mesh
// 
void AssetPreviewWindow::OnAssetLoaded( const AssetLoadArgs& args )
{
  Luna::ShaderAsset* shaderClass = Reflect::ObjectCast< Luna::ShaderAsset >( args.m_AssetClass );
  if ( shaderClass == NULL )
  {
    return;
  }

  m_ListenShaders.insert( shaderClass );
  shaderClass->AddShaderChangedListener( ShaderChangedSignature::Delegate( this, &AssetPreviewWindow::OnShaderChanged ) );
  shaderClass->AddComponentAddedListener( ComponentExistenceSignature::Delegate( this, &AssetPreviewWindow::OnShaderComponentdChanged ) );
  shaderClass->AddComponentRemovedListener( ComponentExistenceSignature::Delegate( this, &AssetPreviewWindow::OnShaderComponentdChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback when an asset is loaded. Check if it's a shader and remove the
// listener.
// 
void AssetPreviewWindow::OnAssetUnloaded( const AssetLoadArgs& args )
{
  Luna::ShaderAsset* shaderClass = Reflect::ObjectCast< Luna::ShaderAsset >( args.m_AssetClass );
  if ( shaderClass == NULL )
  {
    return;
  }

  m_ListenShaders.erase( shaderClass );
  shaderClass->RemoveShaderChangedListener( ShaderChangedSignature::Delegate( this, &AssetPreviewWindow::OnShaderChanged ) );
  shaderClass->RemoveComponentAddedListener( ComponentExistenceSignature::Delegate( this, &AssetPreviewWindow::OnShaderComponentdChanged ) );
  shaderClass->RemoveComponentRemovedListener( ComponentExistenceSignature::Delegate( this, &AssetPreviewWindow::OnShaderComponentdChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// This is the callback for when a mesh is updated (called by the
// FileWatcher).  It is used to keep track of what files have changed.
// 
void AssetPreviewWindow::OnMeshUpdated( const Nocturnal::FileChangedArgs& e )
{
  m_ReloadMeshes.insert( e.m_Path );
}

///////////////////////////////////////////////////////////////////////////////
// This is the callback for when a shader is updated (called by the
// FileWatcher).  It is used to keep track of what files have changed.
// 
void AssetPreviewWindow::OnShaderUpdated( const Nocturnal::FileChangedArgs& e )
{
  m_ReloadShaders.insert( e.m_Path );
}

///////////////////////////////////////////////////////////////////////////////
// This is the callback for when a texture is updated (called by the
// FileWatcher).  It is used to keep track of what files have changed.
// 
void AssetPreviewWindow::OnTextureUpdated( const Nocturnal::FileChangedArgs& e )
{
  m_ReloadTextures.insert( e.m_Path );
}

///////////////////////////////////////////////////////////////////////////////
// This callback is used when a shader is changed in memory.
// 
void AssetPreviewWindow::OnShaderChanged( const ShaderChangedArgs& args )
{
  UpdateShader( args.m_ShaderClass.Ptr() );
}

///////////////////////////////////////////////////////////////////////////////
// This is an event that will fire when a shader attribute is added or
// removed.
// 
void AssetPreviewWindow::OnShaderComponentdChanged( const ComponentExistenceArgs& args )
{
  Asset::ShaderAsset* shaderAsset = Reflect::ObjectCast< Asset::ShaderAsset >( args.m_Asset->GetPackage() );
  NOC_ASSERT( shaderAsset );
  UpdateShader( shaderAsset );
}

///////////////////////////////////////////////////////////////////////////////
// Timer event that will watch files for a specified number of milliseconds
// 
void AssetPreviewWindow::OnWatchFiles( wxTimerEvent& args )
{
  m_FileWatcher.Watch( 1 );
  
  if ( m_ReloadMeshes.size() )
  {
#pragma TODO( "Implement mesh reloading when functionality exists in scenemanager" )
    m_ReloadMeshes.clear();
    Refresh();
  }
  
  if ( m_ReloadShaders.size() )
  {
#pragma TODO( "Implement shader reloading when functionality exists in shadermanager" )
    m_ReloadShaders.clear();
    Refresh();
  }

  if ( m_Scene )
  {
    if ( m_ReloadTextures.size() )
    {
      S_string::iterator itr = m_ReloadTextures.begin();
      while ( itr != m_ReloadTextures.end() )
      {
        if ( m_Scene->m_renderer->m_shader_manager.ReloadTexture( (*itr).c_str() ) )
        {
          itr = m_ReloadTextures.erase( itr );
        }
        else
        {
          ++itr;
        }
      }

      Refresh();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the browser preferences are edited.  Updates the Axis display
// setting.
// 
void AssetPreviewWindow::OnPreferencesChanged( const Reflect::ElementChangeArgs& args )
{
  AssetPreferences* preferences = GetAssetEditorPreferences();
  NOC_ASSERT( preferences );

  if ( args.m_Element == preferences )
  {
    if ( args.m_Field == preferences->DisplayPreviewAxisField() || args.m_Field == NULL )
    {
      __super::DisplayReferenceAxis( preferences->DisplayPreviewAxis() );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Make the current list of files watched by the file watcher.
// 
void AssetPreviewWindow::AddFileChangedListeners()
{
  Nocturnal::FileChangedSignature::Delegate meshChangedDelegate( this, &AssetPreviewWindow::OnMeshUpdated );
  for ( S_string::iterator itr = m_WatchMeshes.begin(), end = m_WatchMeshes.end(); itr != end; ++itr )
  {
    m_FileWatcher.Add( *itr, meshChangedDelegate );
  }

  Nocturnal::FileChangedSignature::Delegate shaderChangedDelegate( this, &AssetPreviewWindow::OnShaderUpdated );
  for ( S_string::iterator itr = m_WatchShaders.begin(), end = m_WatchShaders.end(); itr != end; ++itr )
  {
    m_FileWatcher.Add( *itr, shaderChangedDelegate );
  }

  Nocturnal::FileChangedSignature::Delegate textureChangedDelegate( this, &AssetPreviewWindow::OnTextureUpdated );
  for ( S_string::iterator itr = m_WatchTextures.begin(), end = m_WatchTextures.end(); itr != end; ++itr )
  {
    m_FileWatcher.Add( *itr, textureChangedDelegate );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Make the current list of files stop being watched by the file watcher.
// 
void AssetPreviewWindow::RemoveFileChangedListeners()
{
  Nocturnal::FileChangedSignature::Delegate meshChangedDelegate( this, &AssetPreviewWindow::OnMeshUpdated );
  for ( S_string::iterator itr = m_WatchMeshes.begin(), end = m_WatchMeshes.end(); itr != end; ++itr )
  {
    m_FileWatcher.Remove( *itr, meshChangedDelegate );
  }

  Nocturnal::FileChangedSignature::Delegate shaderChangedDelegate( this, &AssetPreviewWindow::OnShaderUpdated );
  for ( S_string::iterator itr = m_WatchShaders.begin(), end = m_WatchShaders.end(); itr != end; ++itr )
  {
    m_FileWatcher.Remove( *itr, shaderChangedDelegate );
  }

  Nocturnal::FileChangedSignature::Delegate textureChangedDelegate( this, &AssetPreviewWindow::OnTextureUpdated );
  for ( S_string::iterator itr = m_WatchTextures.begin(), end = m_WatchTextures.end(); itr != end; ++itr )
  {
    m_FileWatcher.Remove( *itr, textureChangedDelegate );
  }
}
