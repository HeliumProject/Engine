#pragma once

#include "Luna/API.h"
#include "Foundation/Automation/Event.h"
#include "Object.h"

// Forwards
namespace Undo
{
  class BatchCommand;
  typedef Nocturnal::SmartPtr<BatchCommand> BatchCommandPtr;
}

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Arguments that will be passed into each context menu items callback.
  // 
  class LUNA_EDITOR_API ContextMenuArgs : public Nocturnal::RefCountBase<ContextMenuArgs>
  {
  private:
    Undo::BatchCommandPtr m_Batch;
    ObjectPtr m_ClientData;

  public:
    ContextMenuArgs();
    virtual ~ContextMenuArgs();
    Undo::BatchCommand* GetBatch();
    ObjectPtr GetClientData() const;
    void SetClientData( const ObjectPtr& data );
  };
  typedef Nocturnal::SmartPtr< ContextMenuArgs > ContextMenuArgsPtr;
  typedef std::vector< ContextMenuArgsPtr > V_ContextMenuArgsSmartPtr;
  typedef Nocturnal::Signature< void, const ContextMenuArgsPtr& > ContextMenuSignature;


  /////////////////////////////////////////////////////////////////////////////
  // Combines a callback delegate with an optional Object-derived pointer.
  // 
  struct MenuCallback
  {
    ContextMenuSignature::Delegate m_Delegate;
    ObjectPtr m_ClientData;

    MenuCallback( const ContextMenuSignature::Delegate& callback, const ObjectPtr& clientData )
      : m_Delegate( callback )
      , m_ClientData( clientData )
    {
    }
  };
  typedef std::vector< MenuCallback > V_MenuCallback;


  class ContextMenuItem;
  typedef Nocturnal::SmartPtr< ContextMenuItem > ContextMenuItemPtr;
  typedef std::vector< ContextMenuItemPtr > V_ContextMenuItemSmartPtr;
  /////////////////////////////////////////////////////////////////////////////
  // Class representing a single item in a context menu.
  // 
  class LUNA_EDITOR_API ContextMenuItem : public Nocturnal::RefCountBase<ContextMenuItem>
  {
  private:
    static const tstring s_Separator;
    tstring m_Label;
    V_MenuCallback m_Callbacks;
    tstring m_Help;
    wxBitmap m_Icon;
    bool m_Enabled;

  public:
    ContextMenuItem( const tstring& label, const tstring& help = TXT( "" ), const wxBitmap& icon = wxNullBitmap );
    virtual ~ContextMenuItem();

    static ContextMenuItemPtr Separator();
    static bool IsSeparator( const ContextMenuItemPtr& item );
    bool IsSeparator() const;

    virtual bool IsSubMenu() const;

    const tstring& GetLabel() const;
    void SetLabel( const tstring& label );

    const tstring& GetHelpString() const;
    void SetHelpString( const tstring& help );

    const wxBitmap& GetIcon() const;
    void SetIcon( const wxBitmap& icon );

    bool IsEnabled() const;
    void Enable( bool enable = true );
    void Disable();

    bool ContainsCallback( const ContextMenuSignature::Delegate& callback );
    void AddCallback( const ContextMenuSignature::Delegate& callback, const ObjectPtr clientData = NULL );
    void Callback( const ContextMenuArgsPtr& args );
    bool MergeCallbacks( const ContextMenuItem& item );
  };


  /////////////////////////////////////////////////////////////////////////////
  // Class representing a context menu item that is actually a sub menu.
  // 
  class LUNA_EDITOR_API SubMenu : public ContextMenuItem
  {
  private:
    V_ContextMenuItemSmartPtr m_SubItems;

  public:
    SubMenu( const tstring& label, const tstring& help = TXT( "" ), const wxBitmap& icon = wxNullBitmap );
    virtual ~SubMenu();

    virtual bool IsSubMenu() const NOC_OVERRIDE;

    void AppendItem( const ContextMenuItemPtr& menuItem );
    const V_ContextMenuItemSmartPtr& GetSubItems() const;
  };
  typedef Nocturnal::SmartPtr< SubMenu > SubMenuPtr;


  /////////////////////////////////////////////////////////////////////////////
  // Class representing a context menu.
  // 
  class LUNA_EDITOR_API ContextMenu : public wxEvtHandler, public Nocturnal::RefCountBase<ContextMenu>
  {
  private:
    typedef std::map< i32, ContextMenuItemPtr > M_ContextMenuItemSmartPtr;

  private:
    wxFrame* m_Frame;
    M_ContextMenuItemSmartPtr m_ItemsById;
    wxMenu m_Menu;
    ContextMenuArgsPtr m_Args;

  public:
    ContextMenu( wxFrame* frame = NULL );
    virtual ~ContextMenu();

    ContextMenuItemPtr AddItem( const ContextMenuItemPtr& menuItem );
    ContextMenuItemPtr AddItem( const tstring& label );
    void AddSeparator();
    void Popup( wxWindow* parent, const ContextMenuArgsPtr& args );
    bool IsEmpty() const;
    size_t GetNumMenuItems() const;
    ContextMenuItemPtr GetMenuItem( const size_t index ) const;

  private:
    void Append( const ContextMenuItemPtr& menuItem, wxMenu* menu );
    void Hookup( const ContextMenuItemPtr& menuItem, wxMenuItem* wxItem, wxMenu* menu );

  protected:
    void OnMenuOpen( wxMenuEvent& args );
    void OnMenuClose( wxMenuEvent& args );
    void OnMenuHighlightItem( wxMenuEvent& args );
    void OnMenuItem( wxCommandEvent& args );
  };
  typedef Nocturnal::SmartPtr< ContextMenu > ContextMenuPtr;
}
