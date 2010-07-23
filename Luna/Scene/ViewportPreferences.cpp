#include "Precompile.h"
#include "ViewportPreferences.h"

using namespace Luna; 
   
ViewportPreferences::ViewportPreferences()
: m_CameraMode (CameraModes::Orbit)
, m_GeometryMode (GeometryModes::Render)
, m_ColorMode (ViewColorModes::Type)
, m_Highlighting (true)
, m_AxesVisible (true)
, m_GridVisible (true)
, m_BoundsVisible (false)
, m_StatisticsVisible (false)
{
  for(int i = 0; i < CameraModes::Count; ++i)
  {
    m_CameraPrefs.push_back( new CameraPreferences() ); 
    m_CameraPrefs.back()->m_CameraMode = CameraMode(i);
  }
}

void ViewportPreferences::ApplyToViewport(Luna::Viewport* view)
{
  // apply settings for all modes that we have... 
  for(size_t i = 0; i < m_CameraPrefs.size(); ++i)
  {
    CameraPreferencesPtr prefs = m_CameraPrefs[i]; 
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

void ViewportPreferences::LoadFromViewport(Luna::Viewport* view)
{
  // just blow away the previous preferences
  m_CameraPrefs.clear(); 

  for(int i = 0; i < CameraModes::Count; ++i)
  {
    CameraMode mode = (CameraMode) i; 
    CameraPreferencesPtr prefs = new CameraPreferences(); 
    prefs->m_CameraMode = mode;
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

ViewColorMode ViewportPreferences::GetColorMode() const
{
  return m_ColorMode;
}

void ViewportPreferences::SetColorMode( ViewColorMode mode )
{
  if ( m_ColorMode != mode )
  {
    m_ColorMode = mode;
    RaiseChanged( ColorModeField() );
  }
}

const Reflect::Field* ViewportPreferences::ColorModeField() const
{
  return GetClass()->FindField( &ViewportPreferences::m_ColorMode );
}
