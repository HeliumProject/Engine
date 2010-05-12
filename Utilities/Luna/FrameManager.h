#pragma once

namespace Luna
{
  class FrameManager : public wxAuiManager
  {
  public:
    FrameManager( wxWindow* managedWindow = NULL, unsigned int flags = wxAUI_MGR_DEFAULT );

    virtual wxAuiFloatingFrame* CreateFloatingFrame( wxWindow* parent, const wxAuiPaneInfo& p );
  };
}
