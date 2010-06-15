#pragma once

#include "Drawer.h"

namespace Luna { class Button; }

namespace Luna
{
  class MainFrame;

  class DrawerPanel : public wxPanel
  {
  public:
    DrawerPanel( wxWindow* parent );
    virtual ~DrawerPanel();

    void AddDrawer( Drawer* drawer );

    Drawer* GetCurrentDrawer() const;

    bool IsFixedSizeButtons() const;
    void SetFixedSizeButtons( bool fixedSize );

    bool GetClickToOpen() const;
    void SetClickToOpen( bool value );

  private:
    void DestroyDrawers();
    Drawer* FindDrawer( i32 drawerID );
    Luna::Button* GetButtonForDrawer( Drawer* drawer );
    void ToggleDrawerButton( Drawer* drawer, bool pressed );

    void OnDrawerButtonClicked( wxCommandEvent& args );
    void OnDrawerOpened( const DrawerArgs& args );
    void OnDrawerClosed( const DrawerArgs& args );
    void OnMouseEnterButton( wxMouseEvent& args );

  private:
    S_Drawer m_Drawers;
    Drawer* m_CurrentDrawer;
    bool m_IsFixedSizeButtons;;
    bool m_ClickToOpen;

    typedef std::map< i32, Luna::Button* > M_Button;
    M_Button m_Buttons;
  };
}
