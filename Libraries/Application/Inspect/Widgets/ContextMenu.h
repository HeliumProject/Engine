#pragma once

#include "Application/API.h"

#include "Platform/Types.h"
#include "Foundation/Automation/Event.h"

#include <wx/event.h>

namespace Inspect
{
  class APPLICATION_API Control;


  //
  // Static context menu support
  //

  struct ContextMenuEventArgs
  {
    Control* m_Control;

    const std::string& m_Item;

    ContextMenuEventArgs(Control* control, const std::string& item)
      : m_Control (control)
      , m_Item (item)
    {

    }
  };

  // the delegate for a per-item callback to be called upon activation
  typedef Nocturnal::Signature<void, const ContextMenuEventArgs&> ContextMenuSignature;

  // container for each delegate of each context menu item
  typedef std::map<std::string, ContextMenuSignature::Delegate> M_ContextMenuDelegate;


  //
  // Dynamic context menu support
  //

  class APPLICATION_API ContextMenu;
  typedef Nocturnal::SmartPtr<ContextMenu> ContextMenuPtr;

  // popup-time context menu setup delegate
  typedef Nocturnal::Signature<void, ContextMenuPtr> LSetupContextMenuSignature;


  //
  // The menu class
  //

  class APPLICATION_API ContextMenu : public wxEvtHandler, public Nocturnal::RefCountBase<ContextMenu>
  {
  protected:
    Control* m_Control;

    std::vector< std::string > m_Items;
    M_ContextMenuDelegate m_Delegates;

  public:
    ContextMenu(Control* control);
    ~ContextMenu();

  protected:
    void ControlRealized( Control* control );
    void OnShow( wxContextMenuEvent& event );
    void OnItem( wxCommandEvent& event );

  public:
    virtual const std::vector< std::string >& GetItems()
    {
      return m_Items;
    }

    virtual void Resize(size_t size)
    {
      m_Items.resize(size);
    }

    virtual void Clear()
    {
      m_Items.clear();
    }

    // event to add dynamic context menus items to the menu instance
    LSetupContextMenuSignature::Event m_SetupContextMenuEvent;

    virtual void AddItem(const std::string& item, ContextMenuSignature::Delegate delegate);
    virtual void AddSeperator();
  };

  typedef Nocturnal::SmartPtr<ContextMenu> ContextMenuPtr;
}