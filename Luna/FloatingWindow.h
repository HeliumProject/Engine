#pragma once

namespace Luna
{
  class Drawer;

  class FloatingWindow : public wxAuiFloatingFrame
  {
  public:
    FloatingWindow( wxWindow* parent, wxAuiManager* ownerMgr, const wxAuiPaneInfo& pane );
    virtual ~FloatingWindow();

    void AttachToDrawer( Drawer* drawer );
    void DetachFromDrawer();
    bool IsAttachedToDrawer() const;

  private:
    void PushEventMarshaller( wxWindow* window );
    void PopEventMarshaller( wxWindow* window );
    friend class DrawerEventMarshaller;
    void OnMouseEnter( wxMouseEvent& event );
    void OnMouseLeave( wxMouseEvent& event );
    void OnTimer( wxTimerEvent& event );
    void OnClose( wxCloseEvent& event );

  private:
    Drawer* m_Drawer;
    wxTimer m_Timer; // Tracks when to dismiss this window
  };
}
