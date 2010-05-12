#include "Precompile.h"
#include "ContextMenu.h"

#include "Console/Console.h"
#include "Undo/BatchCommand.h"

using namespace Luna;


///////////////////////////////////////////////////////////////////////////////
// ContextMenuArgs
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
// Makes a new batch command that will be passed to all the context menu item
// callbacks.
// 
ContextMenuArgs::ContextMenuArgs()
: m_Batch( new Undo::BatchCommand() )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ContextMenuArgs::~ContextMenuArgs()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the command batch.
// 
Undo::BatchCommand* ContextMenuArgs::GetBatch()
{
  return m_Batch.Ptr();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the client data associated with this context menu argument.
// 
ObjectPtr ContextMenuArgs::GetClientData() const
{
  return m_ClientData;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the client data associated with this argument.
// 
void ContextMenuArgs::SetClientData( const ObjectPtr& data )
{
  NOC_ASSERT( !m_ClientData.ReferencesObject() );
  m_ClientData = data;
}



///////////////////////////////////////////////////////////////////////////////
// ContextMenuItem
///////////////////////////////////////////////////////////////////////////////

// Special label for items that are separators in the context menu.
const std::string ContextMenuItem::s_Separator = "-";

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ContextMenuItem::ContextMenuItem( const std::string& label, const std::string& help, const wxBitmap& icon )
: m_Label( label )
, m_Help( help )
, m_Icon( icon )
, m_Enabled( true )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ContextMenuItem::~ContextMenuItem()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns a new context menu item separator.
// 
ContextMenuItemPtr ContextMenuItem::Separator()
{
  ContextMenuItemPtr separator = new ContextMenuItem( s_Separator );
  return separator;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified context menu item is a separator.
// 
bool ContextMenuItem::IsSeparator( const ContextMenuItemPtr& item )
{
  bool isSeparator = item->GetLabel() == ContextMenuItem::s_Separator;
  return isSeparator;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this instance is a separator.
// 
bool ContextMenuItem::IsSeparator() const
{
  return ContextMenuItem::IsSeparator( this );
}

///////////////////////////////////////////////////////////////////////////////
// Returns false.  Override in a derived class if this is a sub menu.
// 
bool ContextMenuItem::IsSubMenu() const
{
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the label for the menu item.
// 
const std::string& ContextMenuItem::GetLabel() const
{
  return m_Label;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the label for this item.
// 
void ContextMenuItem::SetLabel( const std::string& label )
{
  m_Label = label;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the help string for this menu item (usually for showing in the
// status bar of the application).
// 
const std::string& ContextMenuItem::GetHelpString() const
{
  return m_Help;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the help string.
// 
void ContextMenuItem::SetHelpString( const std::string& help )
{
  m_Help = help;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the icon that will be used with this menu item (displayed to the left
// of the item).
// 
const wxBitmap& ContextMenuItem::GetIcon() const
{
  return m_Icon;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the icon to use with this menu item.
// 
void ContextMenuItem::SetIcon( const wxBitmap& icon )
{
  m_Icon = icon;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this menu item is enabled.
// 
bool ContextMenuItem::IsEnabled() const
{
  return m_Enabled;
}

///////////////////////////////////////////////////////////////////////////////
// Allows you to enable or disable this menu item.
// 
void ContextMenuItem::Enable( bool enable )
{
  m_Enabled = enable;
}

///////////////////////////////////////////////////////////////////////////////
// Disables this menu item; same as calling Enable( false ).
// 
void ContextMenuItem::Disable()
{
  Enable( false );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified callback is already part of this context menu.
// 
bool ContextMenuItem::ContainsCallback( const ContextMenuSignature::Delegate& callback )
{
  bool found = false;

  // TODO: Make this faster by using a set
  V_MenuCallback::const_iterator itr = m_Callbacks.begin();
  V_MenuCallback::const_iterator end = m_Callbacks.end();
  for ( ; itr != end && !found; ++itr )
  {
    // Breaks out of the loop if found
    found = (*itr).m_Delegate.Equals( callback );
  }

  return found;
}

///////////////////////////////////////////////////////////////////////////////
// When this context menu item is selected, all of its callbacks will be invoked.
// Use this function to add callbacks for this menu item.  A menu item may have
// zero or more callbacks.
// 
void ContextMenuItem::AddCallback( const ContextMenuSignature::Delegate& callback, const ObjectPtr clientData )
{
  if ( !ContainsCallback( callback ) )
  {
    m_Callbacks.push_back( LMenuCallback( callback, clientData ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Invokes all registered callbacks for this menu item, passing in the specified
// arguments to each one.
// 
void ContextMenuItem::Callback( const ContextMenuArgsPtr& args )
{
  V_MenuCallback::const_iterator itr = m_Callbacks.begin();
  V_MenuCallback::const_iterator end = m_Callbacks.end();
  for ( ; itr != end; ++itr )
  {
    const LMenuCallback& callback = *itr;
    if ( callback.m_Delegate.Valid() )
    {
      args->SetClientData( callback.m_ClientData );
      callback.m_Delegate.Invoke( args );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// If the specified menu item has the same label as this menu item, this object
// has the specified item's callbacks added to its list.  Returns true if there
// were any callbacks added to this object.
// 
bool ContextMenuItem::MergeCallbacks( const ContextMenuItem& item )
{
  bool merged = false;

  if ( item.GetLabel() == m_Label ) // Should help and icon also have to match??
  {
    if ( item.m_Callbacks.size() > 0 )
    {
      // NOTE: AddCallback will not always add the item any more so this reserve
      // might be holding more memory than is needed.
      m_Callbacks.reserve( m_Callbacks.size() + item.m_Callbacks.size() );
      V_MenuCallback::const_iterator itr = item.m_Callbacks.begin();
      V_MenuCallback::const_iterator end = item.m_Callbacks.end();
      for ( ; itr != end; ++itr )
      {
        AddCallback( (*itr).m_Delegate, (*itr).m_ClientData );
      }
    }
    merged = true;
  }
  
  return merged;
}


///////////////////////////////////////////////////////////////////////////////
// SubMenu
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
SubMenu::SubMenu( const std::string& label, const std::string& help, const wxBitmap& icon )
: ContextMenuItem( label, help, icon )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SubMenu::~SubMenu()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns true (this is a sub menu).
// 
bool SubMenu::IsSubMenu() const
{
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Appends an item to the end of this sub menu.
// 
void SubMenu::AppendItem( const ContextMenuItemPtr& menuItem )
{
  m_SubItems.push_back( menuItem );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of sub items maintained by this sub menu.
// 
const V_ContextMenuItemSmartPtr& SubMenu::GetSubItems() const
{
  return m_SubItems;
}



///////////////////////////////////////////////////////////////////////////////
// ContextMenu
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
// You can optionally specify a frame which will be used to display help text
// on its status bar as the mouse moves over various items in the context menu.
// 
ContextMenu::ContextMenu( wxFrame* frame )
: m_Frame( frame )
{
  Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( ContextMenu::OnMenuOpen ), NULL, this );
  Connect( wxEVT_MENU_CLOSE, wxMenuEventHandler( ContextMenu::OnMenuClose ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ContextMenu::~ContextMenu()
{
  Disconnect( wxEVT_MENU_OPEN, wxMenuEventHandler( ContextMenu::OnMenuOpen ), NULL, this );
  Disconnect( wxEVT_MENU_CLOSE, wxMenuEventHandler( ContextMenu::OnMenuClose ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified menu item to the end of the menu.
// 
ContextMenuItemPtr ContextMenu::AddItem( const ContextMenuItemPtr& menuItem )
{
  Append( menuItem, &m_Menu );
  return menuItem;
}

///////////////////////////////////////////////////////////////////////////////
// Adds a new menu item with the specified label to the end of this menu.  Returns
// the new menu item.
// 
ContextMenuItemPtr ContextMenu::AddItem( const std::string& label )
{
  ContextMenuItemPtr menuItem = new ContextMenuItem( label );
  return AddItem( menuItem );
}

///////////////////////////////////////////////////////////////////////////////
// Adds a separator to the end of this menu.
// 
void ContextMenu::AddSeparator()
{
  m_Menu.AppendSeparator();
}

///////////////////////////////////////////////////////////////////////////////
// Displays this context menu as a popup to the specified parent.  If a context
// menu item is selected, the args that are passed into this function will be
// passed into the callbacks for the menu item.
// 
void ContextMenu::Popup( wxWindow* parent, const ContextMenuArgsPtr& args )
{
  m_Args = args;
  parent->PushEventHandler( this );
  parent->PopupMenu( &m_Menu );
  parent->PopEventHandler();
  m_Args = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this context menu is empty.
// 
bool ContextMenu::IsEmpty() const
{
  return m_ItemsById.empty();
}

///////////////////////////////////////////////////////////////////////////////
// Retuns the number of items in the menu (includes sub menu items).
// 
size_t ContextMenu::GetNumMenuItems() const
{
  return m_ItemsById.size();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the menu item at the specified index.
// 
ContextMenuItemPtr ContextMenu::GetMenuItem( const size_t index ) const
{
  ContextMenuItemPtr menuItem;

  if ( index < m_ItemsById.size() )
  {
    wxMenuItem* wxItem = m_Menu.FindItemByPosition( index );
    M_ContextMenuItemSmartPtr::const_iterator found = m_ItemsById.find( wxItem->GetId() );
    if ( found != m_ItemsById.end() )
    {
      menuItem = found->second;
    }
  }

  return menuItem;
}

///////////////////////////////////////////////////////////////////////////////
// Helper function that actually converts the application menu item into a 
// wxMenuItem.  This function may be called recursively to fill out sub menus.
// 
void ContextMenu::Append( const ContextMenuItemPtr& menuItem, wxMenu* menu )
{
  if ( menuItem->IsSubMenu() )
  {
    SubMenu* subMenu = static_cast< SubMenu* >( menuItem.Ptr() );
    wxMenu* wxSubMenu = new wxMenu();
    wxMenuItem* wxItem = menu->AppendSubMenu( wxSubMenu, subMenu->GetLabel(), subMenu->GetHelpString() );
    Hookup( subMenu, wxItem, wxSubMenu );

    V_ContextMenuItemSmartPtr::const_iterator itr = subMenu->GetSubItems().begin();
    V_ContextMenuItemSmartPtr::const_iterator end = subMenu->GetSubItems().end();
    for ( ; itr != end; ++itr )
    {
      Append( *itr, wxSubMenu );
    }
  }
  else if ( menuItem->IsSeparator() )
  {
    menu->AppendSeparator();
  }
  else
  {
    wxMenuItem* wxItem = new wxMenuItem( &m_Menu, wxID_ANY, menuItem->GetLabel().c_str(), menuItem->GetHelpString().c_str(), false ); 
    wxItem->SetBitmap( menuItem->GetIcon() );
    menu->Append( wxItem );
    menu->Enable( wxItem->GetId(), menuItem->IsEnabled() );
    Hookup( menuItem, wxItem, menu );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to connect UI callbacks to menu items.
// 
void ContextMenu::Hookup( const ContextMenuItemPtr& menuItem, wxMenuItem* wxItem, wxMenu* menu )
{
  Connect( wxItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ContextMenu::OnMenuItem ), NULL, this );
  Connect( wxItem->GetId(), wxEVT_MENU_HIGHLIGHT, wxMenuEventHandler( ContextMenu::OnMenuHighlightItem ), NULL, this );
  
  m_ItemsById.insert( M_ContextMenuItemSmartPtr::value_type( wxItem->GetId(), menuItem ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the context menu is opening.  Clears the status bar help
// text on the frame.
// 
void ContextMenu::OnMenuOpen( wxMenuEvent& args )
{
  if ( m_Frame )
  {
    m_Frame->DoGiveHelp( "", false );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the context menu is closing.  Clears the status bar help
// text on the frame.
// 
void ContextMenu::OnMenuClose( wxMenuEvent& args )
{
  if ( m_Frame )
  {
    m_Frame->DoGiveHelp( "", false );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the mouse moves over an item in the menu.  Displays the
// help text for the highlighted menu item on the frame's status bar.
// 
void ContextMenu::OnMenuHighlightItem( wxMenuEvent& args )
{
  if ( m_Frame )
  {
    M_ContextMenuItemSmartPtr::const_iterator found = m_ItemsById.find( args.GetId() );
    if ( found != m_ItemsById.end() )
    {
      const ContextMenuItemPtr& menuItem = found->second;
      m_Frame->DoGiveHelp( menuItem->GetHelpString().c_str(), true );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an item in the menu is clicked on.  Invokes all the callbacks
// registered to that menu item, passing in the context menu event arguments
// that were passed into the Popup() function.
// 
void ContextMenu::OnMenuItem( wxCommandEvent& args )
{
  M_ContextMenuItemSmartPtr::const_iterator found = m_ItemsById.find( args.GetId() );
  if ( found != m_ItemsById.end() )
  {
    const ContextMenuItemPtr& menuItem = found->second;
    menuItem->Callback( m_Args );
  }
}
