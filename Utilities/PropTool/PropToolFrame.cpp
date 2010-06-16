#include "PropToolFrame.h"

#include <sstream>
#include <wx/filedlg.h>
#include <wx/aboutdlg.h>

#include "igDXRender/render.h"
#include "igDXRender/scenemanager.h"
#include "igDXRender/OBJObjectLoader.h"

#include "Application/UI/ImageManager.h"
#include "Foundation/Version.h"

const char* g_Name = "PropTool";

void FileWatchTimer::Notify()
{
  if ( !m_Frame->m_FileWatcher.Watch( 0 ) )
  {
    printf( "There was an error watching files\n" );
  }
};

PropToolFrame::PropToolFrame( wxWindow* parent )
: PropToolFrameBase( parent )
, m_FileWatchTimer( this )
{
  SetTitle( _( g_Name ) );

  Nocturnal::ImageManagerInit( "" );

  FileDropTarget* target = NULL;

  target = new FileDropTarget("obj,dds");
  target->AddListener( FileDroppedSignature::Delegate( this, &PropToolFrame::WorkspaceFileDropped ) );
  m_Workspace->SetDropTarget( target );

  target = new FileDropTarget("tga");
  target->AddListener( FileDroppedSignature::Delegate( this, &PropToolFrame::ColorMapChanged ) );
  m_ColorMapPreviewPanel->SetDropTarget( target );

  target = new FileDropTarget("tga");
  target->AddListener( FileDroppedSignature::Delegate( this, &PropToolFrame::ColorMapChanged ) );
  m_ColorMapFilePicker->SetDropTarget( target );

  target = new FileDropTarget("tga");
  target->AddListener( FileDroppedSignature::Delegate( this, &PropToolFrame::NormalMapChanged ) );
  m_NormalMapPreviewPanel->SetDropTarget( target );

  target = new FileDropTarget("tga");
  target->AddListener( FileDroppedSignature::Delegate( this, &PropToolFrame::NormalMapChanged ) );
  m_NormalMapFilePicker->SetDropTarget( target );

  target = new FileDropTarget("tga");
  target->AddListener( FileDroppedSignature::Delegate( this, &PropToolFrame::GPIMapChanged ) );
  m_GPIMapFilePicker->SetDropTarget( target );

  target = new FileDropTarget("tga");
  target->AddListener( FileDroppedSignature::Delegate( this, &PropToolFrame::GPIMapChanged ) );
  m_GlossMapPreviewPanel->SetDropTarget( target );

  target = new FileDropTarget("tga");
  target->AddListener( FileDroppedSignature::Delegate( this, &PropToolFrame::GPIMapChanged ) );
  m_ParallaxMapPreviewPanel->SetDropTarget( target );

  target = new FileDropTarget("tga");
  target->AddListener( FileDroppedSignature::Delegate( this, &PropToolFrame::GPIMapChanged ) );
  m_IncanMapPreviewPanel->SetDropTarget( target );

  m_FileWatchTimer.Start( 100 );
}

PropToolFrame::~PropToolFrame()
{
  m_FileWatchTimer.Stop();

  Nocturnal::ImageManagerCleanup();
}

void PropToolFrame::OnOpen( wxCommandEvent& event )
{
	wxFileDialog* openDialog = new wxFileDialog( this, _("Choose a file to open"), wxEmptyString, wxEmptyString, _("Models (*.obj)|*.obj"), wxFD_OPEN, wxDefaultPosition );
 
	if ( openDialog->ShowModal() == wxID_OK )
	{
    Open( openDialog->GetPath().c_str() );
	}

  openDialog->Destroy();
}

void PropToolFrame::OnExit( wxCommandEvent& event )
{
  Close();
}

void PropToolFrame::OnAbout( wxCommandEvent& event )
{
  wxAboutDialogInfo info;
  info.SetName( _( g_Name ) );
  info.SetVersion( _( NOCTURNAL_VERSION_STRING ) );
  info.SetCopyright( _T( "(c) Insomniac Games, Inc." ) );

  wxAboutBox( info );
}

void PropToolFrame::OBJChanged( const FileDroppedArgs& args )
{
  Open( args.m_Path );
}

void PropToolFrame::EnvChanged( const FileDroppedArgs& args )
{
  igDXRender::Scene* scene =  m_Workspace->GetScene();  

  // don't load and env if there is no mesh loaded, not that it matters but it'll make more sense
  if (scene->m_mesh_handle==0xffffffff)
  {
    return;
  }

  u32 env_handle = scene->LoadEnvironment(args.m_Path.c_str(),0x40404040);
  scene->SetEnvironmentHandle(env_handle);
  scene->m_render_env_cube = true;

  m_Workspace->Refresh();
}

void PropToolFrame::WorkspaceFileDropped( const FileDroppedArgs& args )
{
  if ( args.m_Path.Extension() == "obj" )
  {
    OBJChanged( args );
  }
  else if ( args.m_Path.Extension() == "dds" )
  {
    EnvChanged( args );
  }
}

void PropToolFrame::OnShaderSelected( wxCommandEvent& event )
{
  int selection = m_ShaderListBox->GetSelection();
  if (selection != wxNOT_FOUND)
  {
    wxString str = m_ShaderListBox->GetString(selection);
    if ( !str.empty() )
    {
      printf("Selected shader %s\n", str.c_str());
      igDXRender::Scene* scene = m_Workspace->GetScene();

      if ( scene )
      {
        u32 shader_handle = scene->m_render_class->m_shader_database->FindShader( str + ".xml" );
        if ( shader_handle == 0xffffffff )
        {
          return;
        }

        igDXRender::Shader* sh = scene->m_render_class->m_shader_database->ResolveShader( shader_handle );
        if ( sh )
        {
          m_AlphaMode->SetSelection( sh->m_alpha_type );

          igDXRender::Texture* base_tex = scene->m_render_class->m_shader_database->ResolveTexture( sh->m_textures[ igDXRender::Texture::SAMPLER_BASE_MAP ] );
          if ( base_tex )
          {
            m_ColorMapFilePicker->SetPath( Nocturnal::Path ( base_tex->m_filename ).Native() );
            ColorMapChanged( base_tex->m_filename );
          }

          igDXRender::Texture* norm_tex = scene->m_render_class->m_shader_database->ResolveTexture( sh->m_textures[ igDXRender::Texture::SAMPLER_NORMAL_MAP ] );
          if ( norm_tex )
          {
            m_NormalMapFilePicker->SetPath( Nocturnal::Path ( norm_tex->m_filename ).Native() );
            NormalMapChanged( norm_tex->m_filename );
          }

          igDXRender::Texture* gpi_tex = scene->m_render_class->m_shader_database->ResolveTexture( sh->m_textures[ igDXRender::Texture::SAMPLER_GPI_MAP ] );
          if ( gpi_tex )
          {
            m_GPIMapFilePicker->SetPath( Nocturnal::Path ( gpi_tex->m_filename ).Native() );
            GPIMapChanged( gpi_tex->m_filename );
          }
        }
      }
    }
  }
}

void PropToolFrame::OnAlphaModeChanged( wxCommandEvent& event )
{
  int selection = m_ShaderListBox->GetSelection();
  if (selection != wxNOT_FOUND)
  {
    wxString shader = m_ShaderListBox->GetString(selection);
    if ( !shader.empty() )
    {
      igDXRender::Scene* scene = m_Workspace->GetScene();
      if ( scene )
      {
        u32 shader_handle = scene->m_render_class->m_shader_database->FindShader( ( std::string( shader ) + ".xml" ).c_str() );
        if ( shader_handle != 0xffffffff )
        {
          igDXRender::Shader* sh = scene->m_render_class->m_shader_database->ResolveShader( shader_handle );
          if ( sh )
          {
            sh->m_alpha_type = m_AlphaMode->GetSelection();
          }
        }
      }
    }
  }

  m_Workspace->Refresh();
}

static void UpdateShaderTexture( igDXRender::Scene* scene, const char* shader, const char* texture, int stage )
{
  u32 shader_handle = scene->m_render_class->m_shader_database->FindShader( ( std::string( shader ) + ".xml" ).c_str() );
  if ( shader_handle != 0xffffffff )
  {
    igDXRender::Shader* sh = scene->m_render_class->m_shader_database->ResolveShader( shader_handle );
    if ( sh )
    {
      u32 texture_handle = scene->m_render_class->m_shader_database->FindTexture( texture );
      if ( texture_handle == 0xffffffff )
      {
        igDXRender::TextureSettings settings;
        settings.m_Path = texture;
        scene->m_render_class->m_shader_database->LoadTextureWithSettings( settings, sh, stage );
      }
      else
      {
        igDXRender::Texture* tex = scene->m_render_class->m_shader_database->ResolveTexture( texture_handle );
        if ( tex )
        {
          scene->m_render_class->m_shader_database->ReloadTexture( texture );
        }

        if ( sh->m_textures[ stage ] != texture_handle )
        {
          sh->ReplaceTexture( texture_handle, stage );
        }
      }

      if ( stage == igDXRender::Texture::SAMPLER_GPI_MAP )
      {
        sh->m_flags |= SHDR_FLAG_GPI_MAP;
      }
    }
  }
}

void PropToolFrame::ColorMapChanged( const FileDroppedArgs& args )
{
  if ( m_ColorMapFilePicker->GetPath() != args.m_Path )
  {
    m_ColorMapFilePicker->SetPath( Nocturnal::Path ( args.m_Path ).Native() );
  }

  Nocturnal::FileChangedSignature::Delegate delegate ( this, &PropToolFrame::OnColorMapFileModification );
  m_FileWatcher.Remove( m_ColorMapFilePicker->GetPath().c_str(), delegate );

  Nocturnal::Path f( args.m_Path );
  if ( f.Exists() )
  {
    m_FileWatcher.Add( args.m_Path, delegate );
    OnColorMapFileModification( Nocturnal::FileChangedArgs (args.m_Path.Get()) );
  }
  else
  {
    m_ColorMapPreviewPanel->ClearImage();
  }
}

void PropToolFrame::OnColorMapFileChanged( wxFileDirPickerEvent& event )
{
  ColorMapChanged( FileDroppedArgs ( m_ColorMapFilePicker->GetPath().c_str() ) );
}

void PropToolFrame::OnColorMapFileModification( const Nocturnal::FileChangedArgs& args )
{
  m_ColorMapPreviewPanel->SetImage( args.m_Path );

  int selection = m_ShaderListBox->GetSelection();
  if (selection != wxNOT_FOUND)
  {
    wxString str = m_ShaderListBox->GetString(selection);
    if ( !str.empty() )
    {
      igDXRender::Scene* scene = m_Workspace->GetScene();
      if ( scene )
      {
        UpdateShaderTexture( scene, str.c_str(), args.m_Path.c_str(), igDXRender::Texture::SAMPLER_BASE_MAP );
        m_Workspace->Refresh();
      }
    }
  }
}

void PropToolFrame::NormalMapChanged( const FileDroppedArgs& args )
{
  if ( m_NormalMapFilePicker->GetPath() != args.m_Path )
  {
    m_NormalMapFilePicker->SetPath( Nocturnal::Path ( args.m_Path ).Native() );
  }

  Nocturnal::FileChangedSignature::Delegate delegate ( this, &PropToolFrame::OnNormalMapFileModification );
  m_FileWatcher.Remove( m_NormalMapFilePicker->GetPath().c_str(), delegate );
  
  Nocturnal::Path f ( args.m_Path );
  if ( f.Exists() )
  {
    m_FileWatcher.Add( args.m_Path, delegate );
    OnNormalMapFileModification( Nocturnal::FileChangedArgs (args.m_Path) );
  }
  else
  {
    m_NormalMapPreviewPanel->ClearImage();
  }
}

void PropToolFrame::OnNormalMapFileChanged( wxFileDirPickerEvent& event )
{
  NormalMapChanged( FileDroppedArgs ( m_NormalMapFilePicker->GetPath().c_str() ) );
}

void PropToolFrame::OnNormalMapFileModification( const Nocturnal::FileChangedArgs& args )
{
  m_NormalMapPreviewPanel->SetImage( args.m_Path );

  int selection = m_ShaderListBox->GetSelection();
  if (selection != wxNOT_FOUND)
  {
    wxString str = m_ShaderListBox->GetString(selection);
    if ( !str.empty() )
    {
      igDXRender::Scene* scene = m_Workspace->GetScene();
      if ( scene )
      {
        UpdateShaderTexture( scene, str.c_str(), args.m_Path.c_str(), igDXRender::Texture::SAMPLER_NORMAL_MAP );
        m_Workspace->Refresh();
      }
    }
  }
}

void PropToolFrame::GPIMapChanged( const FileDroppedArgs& args )
{
  if ( m_GPIMapFilePicker->GetPath() != args.m_Path )
  {
    m_GPIMapFilePicker->SetPath( Nocturnal::Path ( args.m_Path ).Native() );
  }

  Nocturnal::FileChangedSignature::Delegate delegate ( this, &PropToolFrame::OnGPIMapFileModification );
  m_FileWatcher.Remove( m_GPIMapFilePicker->GetPath().c_str(), delegate );

  Nocturnal::Path f ( args.m_Path );
  if ( f.Exists() )
  {
    m_FileWatcher.Add( args.m_Path, delegate );
    OnGPIMapFileModification( Nocturnal::FileChangedArgs (args.m_Path) );
  }
  else
  {
    m_GlossMapPreviewPanel->ClearImage();
    m_ParallaxMapPreviewPanel->ClearImage();
    m_IncanMapPreviewPanel->ClearImage();
  }
}

void PropToolFrame::OnGPIMapFileChanged( wxFileDirPickerEvent& event )
{
  GPIMapChanged( FileDroppedArgs ( m_GPIMapFilePicker->GetPath().c_str() ) );
}

void PropToolFrame::OnGPIMapFileModification( const Nocturnal::FileChangedArgs& args )
{
  m_GlossMapPreviewPanel->SetImage( args.m_Path, Nocturnal::ImageViewPanelFlags::RenderChannelRed );
  m_ParallaxMapPreviewPanel->SetImage( args.m_Path, Nocturnal::ImageViewPanelFlags::RenderChannelGreen );
  m_IncanMapPreviewPanel->SetImage( args.m_Path, Nocturnal::ImageViewPanelFlags::RenderChannelBlue );

  int selection = m_ShaderListBox->GetSelection();
  if (selection != wxNOT_FOUND)
  {
    wxString str = m_ShaderListBox->GetString(selection);
    if ( !str.empty() )
    {
      igDXRender::Scene* scene = m_Workspace->GetScene();
      if ( scene )
      {
        UpdateShaderTexture( scene, str.c_str(), args.m_Path.c_str(), igDXRender::Texture::SAMPLER_GPI_MAP );
        m_Workspace->Refresh();
      }
    }
  }
}

void PropToolFrame::Open( const std::string& filename )
{
  Nocturnal::Path file ( filename );

  if ( !file.Exists() )
  {
    std::stringstream str;
    str << "Unable to open '" << filename << "', the file does not exist";
    wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );
    return;
  }

  igDXRender::OBJObjectLoader objLoader;
  objLoader.IncrRefCount();

  m_Workspace->ClearScene();
  if ( !m_Workspace->LoadScene( file.c_str(), &objLoader ) )
  {
    std::stringstream str;
    str << "Unable to load '" << filename << "', failed to load scene";
    wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );
    return;
  }

  m_ShaderListBox->Freeze();
  m_ShaderListBox->Clear();

  bool hasShader = false;
  
  for( u32 i = 0; i < objLoader.m_fragments.size(); ++i )
  {
    if ( !objLoader.m_fragments[ i ].m_shader.empty() )
    {
      m_ShaderListBox->Append( objLoader.m_fragments[ i ].m_shader );
      hasShader = true;
    }
  }

  m_ShaderListBox->Thaw();

  if ( hasShader )
  {
    m_ShaderListBox->SetSelection( 0 );
    OnShaderSelected( wxCommandEvent () );
  }
}