#pragma once

#include <set>

#include "Foundation/Automation/Event.h"

namespace Luna
{
  class Drawer;
  class FloatingWindow;

  struct DrawerArgs
  {
    DrawerArgs( Drawer* drawer )
      : m_Drawer( drawer )
    {
    }
  
    Drawer* m_Drawer;
  };
  typedef Nocturnal::Signature< void, const DrawerArgs& > DrawerSignature;

  class Drawer : public wxEvtHandler
  {
  public:
    Drawer( wxAuiPaneInfo& pane, const wxBitmap& icon );
    virtual ~Drawer();

    void OpenDrawer( const wxPoint& loc );
    void CloseDrawer();
    bool IsOpen() const;

    wxAuiPaneInfo* GetPane();

    i32 GetDrawerID() const;
    void SetDrawerID( i32 id );

    const wxString& GetTitle() const;
    const wxBitmap& GetIcon() const;

  private:
    void OnCloseFloatingWindow( wxCloseEvent& args );

  private:
    wxAuiPaneInfo* m_Pane;
    wxBitmap m_Icon;
    i32 m_ID;
    wxRect m_PopupRect;
    FloatingWindow* m_FloatingWindow;

  private:
    DrawerSignature::Event m_OpenEvent;
  public:
    void AddOpenListener( const DrawerSignature::Delegate& listener )
    {
      m_OpenEvent.Add( listener );
    }
    void RemoveOpenListener( const DrawerSignature::Delegate& listener )
    {
      m_OpenEvent.Remove( listener );
    }

  private:
    DrawerSignature::Event m_CloseEvent;
  public:
    void AddCloseListener( const DrawerSignature::Delegate& listener )
    {
      m_CloseEvent.Add( listener );
    }
    void RemoveCloseListener( const DrawerSignature::Delegate& listener )
    {
      m_CloseEvent.Remove( listener );
    }

  private:
    DrawerSignature::Event m_PaneModifiedEvent;
  public:
    void AddPaneModifiedListener( const DrawerSignature::Delegate& listener )
    {
      m_PaneModifiedEvent.Add( listener );
    }
    void RemovePaneModifiedListener( const DrawerSignature::Delegate& listener )
    {
      m_PaneModifiedEvent.Remove( listener );
    }
  };

  class DrawerIDSort
  {
  public:
    bool operator()( Drawer* first, Drawer* second ) const
    {
      return first->GetDrawerID() < second->GetDrawerID();
    }
  };
  typedef std::set< Drawer*, DrawerIDSort > S_Drawer;
}
