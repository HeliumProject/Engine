#pragma once

#include "Luna/API.h"
#include "CommandQueue.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Base class for top level windows in Luna.
  // 
  class LUNA_EDITOR_API Frame NOC_ABSTRACT : public wxFrame
  {
    // 
    // Member variables
    //
  private:
    typedef std::map< i32, wxWindow* > M_MenuIdToPanel;
    M_MenuIdToPanel m_Panels;

  protected:
    CommandQueue m_CommandQueue;
    wxAuiManager m_FrameManager;

    // 
    // General member functions
    // 
  public:
    Frame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame" );
    virtual ~Frame();

    virtual void SaveWindowState() = 0;

    void PostCommand( const Undo::CommandPtr& command );

    virtual const std::string& GetPreferencePrefix() const = 0;
    virtual std::string PrefixPreferenceKey( const std::string& key ) const;

  protected:
    u32 CreatePanelsMenu( wxMenu* menu );
    void UpdatePanelsMenu( wxMenu* menu );

    // 
    // UI event handlers
    // 
  private:
    void OnSetFocus( wxFocusEvent& args );
    void OnShowPanel( wxCommandEvent& args );
    void OnExiting( wxCloseEvent& args );

  protected:
    DECLARE_EVENT_TABLE();
  };

}
