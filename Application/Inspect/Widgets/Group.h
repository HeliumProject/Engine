#pragma once

#include "Application/API.h"
#include "Application/Inspect/Widgets/Container.h"

#include <wx/panel.h>

namespace Inspect
{
  class PanelOutline : public wxPanel
  {
  public:
    Group* m_Group;

    PanelOutline( wxWindow* parent, Group* group, const tchar* name );
    void OnPaint( wxPaintEvent& args );
    
    DECLARE_EVENT_TABLE();
  };

  //
  // Contains other controls and arranges them vertically.
  // Can optionally draw a square border around all contained controls.
  //

  class APPLICATION_API Group : public Reflect::ConcreteInheritor<Group, Container>
  {
  private:
    bool m_DrawBorder;

  public:
    Group();

    virtual void Realize(Container* parent);

    bool IsBorderEnabled() const;
    void EnableBorder( bool enable = true );
    void DisableBorder() { EnableBorder( false ); }

    virtual bool IsSelected() { return false; }

    // helper
    static GroupPtr CreatePanel(Control* control);

    // helper
    static GroupPtr CreatePanel(const V_Control& controls);

    friend class APPLICATION_API Canvas;
  };

  typedef Nocturnal::SmartPtr<Group> GroupPtr;
  typedef std::vector<GroupPtr> V_Group;
}