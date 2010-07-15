#include "Precompile.h"
#include "ViewPreferences.h"

using namespace Luna; 

REFLECT_DEFINE_CLASS( ViewPreferences );

void ViewPreferences::EnumerateClass( Reflect::Compositor<ViewPreferences>& comp )
{
  Reflect::EnumerationField* enumCameraMode = comp.AddEnumerationField( &ViewPreferences::m_CameraMode, "m_CameraMode" );
  Reflect::EnumerationField* enumGeometryMode = comp.AddEnumerationField( &ViewPreferences::m_GeometryMode, "m_GeometryMode" );
  Reflect::Field* fieldCameraPrefs = comp.AddField( &ViewPreferences::m_CameraPrefs, "m_CameraPrefs" );
  Reflect::Field* fieldColorMode = comp.AddEnumerationField( &ViewPreferences::m_ColorMode, "m_ColorMode" );

  Reflect::Field* fieldHighlighting = comp.AddField( &ViewPreferences::m_Highlighting, "m_Highlighting" );
  Reflect::Field* fieldAxesVisible = comp.AddField( &ViewPreferences::m_AxesVisible, "m_AxesVisible" );
  Reflect::Field* fieldGridVisible = comp.AddField( &ViewPreferences::m_GridVisible, "m_GridVisible" );
  Reflect::Field* fieldBoundsVisible = comp.AddField( &ViewPreferences::m_BoundsVisible, "m_BoundsVisible" );
  Reflect::Field* fieldStatisticsVisible = comp.AddField( &ViewPreferences::m_StatisticsVisible, "m_StatisticsVisible" );
}
    
ViewPreferences::ViewPreferences()
: m_CameraMode (CameraModes::Orbit)
, m_GeometryMode (GeometryModes::Render)
, m_ColorMode (ViewColorModes::NodeType)
, m_Highlighting (true)
, m_AxesVisible (true)
, m_GridVisible (true)
, m_BoundsVisible (false)
, m_StatisticsVisible (false)
{
  for(int i = 0; i < CameraModes::Count; ++i)
  {
    m_CameraPrefs.push_back( new CameraPreferences( CameraMode(i) ) ); 
  }
}

void ViewPreferences::ApplyToView(Luna::View* view)
{
  // apply settings for all modes that we have... 
  for(size_t i = 0; i < m_CameraPrefs.size(); ++i)
  {
    LCameraPreferencesPtr prefs = m_CameraPrefs[i]; 
    CameraMode mode = prefs->m_CameraMode; 
    Luna::Camera* camera = view->GetCameraForMode(mode); 

    prefs->ApplyToCamera(camera); 

  }
  
  view->SetCameraMode( m_CameraMode ); 

  view->SetGeometryMode( m_GeometryMode ); 
  view->SetHighlighting( m_Highlighting ); 
  view->SetAxesVisible( m_AxesVisible ); 
  view->SetGridVisible( m_GridVisible ); 
  view->SetBoundsVisible( m_BoundsVisible ); 
  view->SetStatisticsVisible( m_StatisticsVisible ); 
}

void ViewPreferences::LoadFromView(Luna::View* view)
{
  // just blow away the previous preferences
  m_CameraPrefs.clear(); 

  for(int i = 0; i < CameraModes::Count; ++i)
  {
    CameraMode mode = (CameraMode) i; 
    LCameraPreferencesPtr prefs = new CameraPreferences( mode ); 
    Luna::Camera* camera = view->GetCameraForMode( mode ); 

    prefs->LoadFromCamera(camera); 

    m_CameraPrefs.push_back( prefs ); 

  }

  m_CameraMode = view->GetCameraMode(); 
  m_GeometryMode = view->GetGeometryMode(); 

  m_Highlighting = view->IsHighlighting(); 
  m_AxesVisible = view->IsAxesVisible(); 
  m_GridVisible = view->IsGridVisible(); 
  m_BoundsVisible = view->IsBoundsVisible(); 
  m_StatisticsVisible = view->IsStatisticsVisible(); 
}

ViewColorMode ViewPreferences::GetColorMode() const
{
  return m_ColorMode;
}

void ViewPreferences::SetColorMode( ViewColorMode mode )
{
  if ( m_ColorMode != mode )
  {
    m_ColorMode = mode;
    RaiseChanged( ColorModeField() );
  }
}

const Reflect::Field* ViewPreferences::ColorModeField() const
{
  return GetClass()->FindField( &ViewPreferences::m_ColorMode );
}
