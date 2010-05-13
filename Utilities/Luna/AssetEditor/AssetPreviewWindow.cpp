#include "Precompile.h"

#include "AssetPreviewWindow.h"

#include "AssetPreferences.h"
#include "Asset/GraphShaderAsset.h"
#include "Asset/StandardColorMapAttribute.h"
#include "Asset/StandardNormalMapAttribute.h"
#include "Asset/StandardExpensiveMapAttribute.h"
#include "Asset/StandardDetailMapAttribute.h"
#include "File/Manager.h"
#include "igDXContent/ShaderLoader.h"
#include "ShaderProcess/ShaderProcess.h"
#include "ShaderProcess/ShaderRegistry.h"

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
    m_Scene->m_render_class->m_shader_database->GetShaderFilenames( shaderFiles );
    for ( V_string::iterator itr = shaderFiles.begin(), end = shaderFiles.end(); itr != end; ++itr )
    {
      m_WatchShaders.insert( *itr );
    }

    V_string textureFiles;
    m_Scene->m_render_class->m_shader_database->GetTextureFilenames( textureFiles );
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
    u32 shaderHandle = m_Scene->m_render_class->m_shader_database->FindShader( shaderPath.c_str() );
    if ( shaderHandle == 0xffffffff )
    {
      return;
    }

    igDXRender::Shader* shader = m_Scene->m_render_class->m_shader_database->ResolveShader( shaderHandle );
    NOC_ASSERT( shader );
    
    igDXContent::IRBShaderLoader::UpdateShaderClass( m_Scene->m_render_class->m_shader_database, shaderPath.c_str(), shaderClass->m_AlphaMode );
    
    if ( shaderClass->m_DoubleSided )
    {
      shader->m_flags |= SHDR_FLAG_TWO_SIDED;
    }
    else
    {
      shader->m_flags &= ~SHDR_FLAG_TWO_SIDED;
    }

    Asset::StandardColorMapAttribute* colorMap = shaderClass->GetAttribute< Asset::StandardColorMapAttribute >();
    if ( colorMap )
    {
      igDXRender::TextureSettings settings;
      settings.Clear();

      settings.m_Path = colorMap->GetFilePath();
      settings.m_Anisotropy = 0;
      settings.m_MipBias = colorMap->m_MipBias;
      igDXContent::IRBShaderLoader::SetWrapUV( &settings, shaderClass->m_WrapModeU, shaderClass->m_WrapModeV );
      igDXContent::IRBShaderLoader::SetFilter( &settings, colorMap->m_TexFilter );
      igDXContent::IRBShaderLoader::SetColorFormat( &settings, colorMap->m_TexFormat, igDXRender::Texture::SAMPLER_BASE_MAP );
      
      m_Scene->m_render_class->m_shader_database->UpdateShaderTexture( shaderPath.c_str(), igDXRender::Texture::SAMPLER_BASE_MAP, settings );
    }
    else
    {
      m_Scene->m_render_class->m_shader_database->SetShaderDefaultTexture( shaderPath.c_str(), igDXRender::Texture::SAMPLER_BASE_MAP );
    }
    
    igDXContent::IRBShaderLoader::UpdateShaderColorMap( shader, colorMap );

    Asset::StandardNormalMapAttribute* normalMap = shaderClass->GetAttribute< Asset::StandardNormalMapAttribute >();
    if ( normalMap )
    {
      igDXRender::TextureSettings settings;
      settings.Clear();
      
      settings.m_Path = normalMap->GetFilePath();
      settings.m_Anisotropy = 0;
      settings.m_MipBias = normalMap->m_MipBias;
      igDXContent::IRBShaderLoader::SetWrapUV( &settings, shaderClass->m_WrapModeU, shaderClass->m_WrapModeV );
      igDXContent::IRBShaderLoader::SetFilter( &settings, colorMap->m_TexFilter );
      igDXContent::IRBShaderLoader::SetColorFormat( &settings, colorMap->m_TexFormat, igDXRender::Texture::SAMPLER_NORMAL_MAP );
      
      m_Scene->m_render_class->m_shader_database->UpdateShaderTexture( shaderPath.c_str(), igDXRender::Texture::SAMPLER_NORMAL_MAP, settings );
    }
    else
    {
      m_Scene->m_render_class->m_shader_database->SetShaderDefaultTexture( shaderPath.c_str(), igDXRender::Texture::SAMPLER_BASE_MAP );
    }

    igDXContent::IRBShaderLoader::UpdateShaderNormalMap( shader, normalMap );

    Asset::StandardExpensiveMapAttribute* expensiveMap = shaderClass->GetAttribute< Asset::StandardExpensiveMapAttribute >();
    if ( expensiveMap )
    {
      igDXRender::TextureSettings settings;
      settings.Clear();

      settings.m_Path = expensiveMap->GetFilePath();
      settings.m_Anisotropy = 0;
      settings.m_MipBias = expensiveMap->m_MipBias;
      igDXContent::IRBShaderLoader::SetWrapUV( &settings, shaderClass->m_WrapModeU, shaderClass->m_WrapModeV );
      igDXContent::IRBShaderLoader::SetFilter( &settings, colorMap->m_TexFilter );
      igDXContent::IRBShaderLoader::SetColorFormat( &settings, colorMap->m_TexFormat, igDXRender::Texture::SAMPLER_GPI_MAP );

      if (expensiveMap->m_DetailMapMaskEnabled && settings.m_Format == D3DFMT_DXT1)
      {
        settings.m_Format = D3DFMT_DXT5;
      }

      m_Scene->m_render_class->m_shader_database->UpdateShaderTexture( shaderPath.c_str(), igDXRender::Texture::SAMPLER_GPI_MAP, settings );
    }
    else
    {
      m_Scene->m_render_class->m_shader_database->SetShaderDefaultTexture( shaderPath.c_str(), igDXRender::Texture::SAMPLER_BASE_MAP );
    }

    igDXContent::IRBShaderLoader::UpdateShaderExpensiveMap( shader, expensiveMap );
    
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
  shaderClass->AddAttributeAddedListener( AttributeExistenceSignature::Delegate( this, &AssetPreviewWindow::OnShaderAttributedChanged ) );
  shaderClass->AddAttributeRemovedListener( AttributeExistenceSignature::Delegate( this, &AssetPreviewWindow::OnShaderAttributedChanged ) );
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
  shaderClass->RemoveAttributeAddedListener( AttributeExistenceSignature::Delegate( this, &AssetPreviewWindow::OnShaderAttributedChanged ) );
  shaderClass->RemoveAttributeRemovedListener( AttributeExistenceSignature::Delegate( this, &AssetPreviewWindow::OnShaderAttributedChanged ) );
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
void AssetPreviewWindow::OnShaderAttributedChanged( const AttributeExistenceArgs& args )
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
        if ( m_Scene->m_render_class->m_shader_database->ReloadTexture( (*itr).c_str() ) )
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
