#pragma once

#include "SceneEditorIDs.h"
#include "View.h"
#include "UIToolKit/MenuButton.h"

namespace Luna
{
  class SceneEditor;

  class CameraModeButton : public UIToolKit::MenuButton 
  {
  public:
    CameraModeButton( wxWindow* parent, SceneEditor* sceneEditor, wxWindowID id = wxID_ANY );
    virtual ~CameraModeButton();

    void SetCameraMode( CameraMode mode );
    CameraMode GetCameraMode();

  private:
    void OnChangeMode( wxCommandEvent& args );
    void OnNextCameraMode( wxCommandEvent& args );
    void OnCameraModeChanged( const CameraModeChangeArgs& args );

  private:
    CameraMode m_CurrentMode;
    SceneEditor* m_SceneEditor;
    wxBitmap m_Bitmaps[CameraModes::Count];

  protected:
    DECLARE_EVENT_TABLE();
  };
}
