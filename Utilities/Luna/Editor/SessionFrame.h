#pragma once

#include "API.h"
#include "Frame.h"

namespace Luna
{
  class LUNA_EDITOR_API SessionFrame : public Frame
  {
  private:
    wxMenu* m_FileMenu;
    wxToolBar* m_ToolBar;
  
  public:
    SessionFrame();
    virtual ~SessionFrame();

    virtual void SaveWindowState() NOC_OVERRIDE;

    virtual const std::string& GetPreferencePrefix() const NOC_OVERRIDE;

  private:
    void PromptSaveSession();

    // Callbacks for UI elements
  private:
    void OnOpen( wxCommandEvent& args );
    void OnSaveAll( wxCommandEvent& args );
    void OnSaveSession( wxCommandEvent& args );
    void OnSaveSessionAs( wxCommandEvent& args );
    void OnCheckin( wxCommandEvent& args );
    void OnExit( wxCommandEvent& args );

  private:
    DECLARE_EVENT_TABLE();
  };
}
