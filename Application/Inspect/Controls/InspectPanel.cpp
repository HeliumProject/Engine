#include "InspectPanel.h"
#include "Application/Inspect/Controls/Canvas.h"
#include "Application/Inspect/Controls/Label.h"
#include "Application/UI/ArtProvider.h"

using namespace Reflect;
using namespace Inspect;

Panel::Panel()
: m_ItemData( this )
, m_Expanded( false)
, m_Expandable( true )
, m_Collapsable( true )
, m_Selected( false )
, m_Selectable( true )
, m_ShowTreeNode( true )
, m_RefreshControls( true )
, m_OwnWindow( false )
{

}

Panel::~Panel()
{
  Clear();
}

bool Panel::Process(const tstring& key, const tstring& value)
{
  if ( __super::Process( key, value ) )
    return true;

  if ( key == PANEL_ATTR_TEXT )
  {
    SetText( m_Text = value );
    return true;
  }

  return false;
}

wxWindow* Panel::GetContextWindow()
{
  wxTreeItemId item = m_ItemData.GetId();
  if ( item != Helium::TreeWndCtrlItemIdInvalid )
  {
    Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Window;
    wxWindow* window = treeWndCtrl->GetItemWindow( item );
    if ( window )
    {
      return window;
    }
  }

  return GetWindow();
}

const wxWindow* Panel::GetContextWindow() const
{
  wxTreeItemId item = m_ItemData.GetId();
  if ( item != Helium::TreeWndCtrlItemIdInvalid )
  {
    Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Window;
    wxWindow* window = treeWndCtrl->GetItemWindow( item );
    if ( window )
    {
      return window;
    }
  }

  return GetWindow();
}

void Panel::Clear()
{
  __super::Clear();

  m_RefreshControls = true;
}

void Panel::Realize(Container* parent)
{
  if ( IsRealized() )
  {
    for ( V_Control::iterator itr = m_Controls.begin(), end = m_Controls.end(); itr != end; ++itr )
    {
      (*itr)->Realize( this );
    }
    return;
  }
  
  INSPECT_SCOPE_TIMER( ("") );
  
  Canvas::ExpandState cachedState = m_Canvas->GetPanelExpandState( GetPath() );
  switch ( cachedState )
  {
    // The state was cached to be expanded
  case Canvas::Expanded:
    m_Expanded = true;
    break;

    // The state was cached to be collapsed
  case Canvas::Collapsed:
    m_Expanded = false;
    break;
  }
  
  m_Parent = parent;
  
  RefreshControls();

  // Don't call Group's Realize since we already created m_Window
  Container::Realize( parent );
}

void Panel::UnRealize()
{
  wxTreeItemId item = m_ItemData.GetId();
  if ( item != Helium::TreeWndCtrlItemIdInvalid )
  {
    Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Window;
    treeWndCtrl->Delete( item );
    m_ItemData.SetId( Helium::TreeWndCtrlItemIdInvalid );
  }

  if ( !m_OwnWindow )
  {
    m_Window = NULL;
  }

  __super::UnRealize();
}

void Panel::SetEnabled(bool enabled)
{
  if ( m_OwnWindow )
  {
    __super::SetEnabled( enabled );
  }
  else
  {
    // Do not set m_Window to be enabled/disabled because we do not own m_Window.
    wxWindow* window = m_Window;

    m_Window = NULL;
    __super::SetEnabled( enabled );
    m_Window = window;
  }
}

void Panel::RefreshControls()
{
  if ( !m_RefreshControls )
    return;
    
  Helium::TreeWndCtrl* treeWndCtrl = NULL;
  if ( Reflect::ObjectCast<Panel>( m_Parent ) || Reflect::ObjectCast<Canvas>( m_Parent ) )
  {
    treeWndCtrl = (Helium::TreeWndCtrl*) m_Parent->GetWindow();
    m_OwnWindow = false;
  }
  else
  {
    treeWndCtrl = new Helium::TreeWndCtrl( m_Parent->GetWindow() );
    treeWndCtrl->AddRoot( TXT( "Panel Root (hidden)" ) );
    treeWndCtrl->SetHideRoot( true );
    treeWndCtrl->SetImageList( Helium::GlobalFileIconsTable().GetSmallImageList() );
    treeWndCtrl->SetStateImageList( Helium::GlobalFileIconsTable().GetSmallImageList() );
    m_OwnWindow = true;
  }
  
  m_Window = treeWndCtrl;
  treeWndCtrl->Freeze();
  
  int collapsedIndex = Helium::GlobalFileIconsTable().GetIconID( TXT( "ms_folder_closed" ) );
  int expandedIndex = Helium::GlobalFileIconsTable().GetIconID( TXT( "ms_folder_open" ) );

  wxTreeItemId item = m_ItemData.GetId();
  if ( m_ShowTreeNode )
  {
    if ( item == Helium::TreeWndCtrlItemIdInvalid )
    {
      treeWndCtrl->AppendItem( GetParentTreeNode( m_Parent ), m_Text, collapsedIndex, expandedIndex, &m_ItemData );
      item = m_ItemData.GetId();
    }
  }
  else
  {
    HELIUM_ASSERT( item == Helium::TreeWndCtrlItemIdInvalid );
    item = GetParentTreeNode( m_Parent );
  }

  SetItemExpanded( m_Expanded );

  V_Control::const_iterator beg = m_Controls.begin();
  V_Control::const_iterator end = m_Controls.end();
  for( V_Control::const_iterator itr = beg; itr != end; ++itr )
  {
    Control* c = *itr;
    c->Realize( this );
    
    wxWindow* window = c->GetWindow();
    if ( window != treeWndCtrl )
    {
      treeWndCtrl->AppendItem( item, window );
    }
  }
  
  m_Window->Layout();
  treeWndCtrl->Thaw();
  
  m_RefreshControls = false;
}

tstring Panel::GetPath()
{
  if ( m_Path.empty() )
  {
    BuildPath( m_Path );
  }

  return m_Path;
}

void Panel::BuildPath(tstring& path)
{
  if (m_Parent != NULL && m_Parent->HasType(Reflect::GetType<Panel>()))
  {
    static_cast<Panel*>(m_Parent)->BuildPath(path);
  }

  path += TXT( "|" ) + m_Text;
}

void Panel::GetExpanded(std::map< tstring, tstring >& paths)
{
  tstring path = GetPath();

  if (m_Expanded)
  {
    paths[ path ] = m_Text;
  }

  V_Control::iterator itr = m_Controls.begin();
  V_Control::iterator end = m_Controls.end();
  for ( ; itr != end; ++itr )
  {
    Panel* panel = ObjectCast<Panel>( *itr );
    if ( panel )
    {
      panel->GetExpanded( paths );
    }
  }
}

void Panel::SetExpanded(const std::map< tstring, tstring >& paths)
{
  tstring path = GetPath();

  if ( paths.find(path) != paths.end() )
  {
    m_Expanded = true;
    SetItemExpanded( true );
  }

  V_Control::iterator itr = m_Controls.begin();
  V_Control::iterator end = m_Controls.end();
  for ( ; itr != end; ++itr )
  {
    Panel* panel = ObjectCast<Panel>( *itr );
    if ( panel )
    {
      panel->SetExpanded( paths );
    }
  }
}

bool Panel::IsExpanded()
{
  return m_Expanded;
}

void Panel::SetExpanded(bool expanded, bool force)
{
  Canvas::ExpandState cachedState = Canvas::Default;

  if ( force )
  {
    if ( m_Canvas )
    {
      m_Canvas->SetPanelExpandState( GetPath(), expanded ? Inspect::Canvas::Expanded : Inspect::Canvas::Collapsed );
    }
  }
  else
  {
    cachedState = m_Canvas ? m_Canvas->GetPanelExpandState( GetPath() ) : Canvas::Default;

    switch ( cachedState )
    {
      // The state was cached to be expanded, ignore the expansion change
    case Canvas::Expanded:
      if ( m_Expandable )
      {
        m_Expanded = true;
        SetItemExpanded( true );
      }
      break;

      // The state was cached to be collapsed, ignore the expansion change
    case Canvas::Collapsed:
      if ( m_Collapsable )
      {
        m_Expanded = false;
        SetItemExpanded( false );
      }
      break;
    }
  }

  if ( cachedState == Canvas::Default )
  {
    if ( expanded && m_Expandable )
    {
      m_Expanded = expanded;
      SetItemExpanded( expanded );
    }
    else if ( !expanded && m_Collapsable )
    {
      m_Expanded = expanded;
      SetItemExpanded( expanded );
    }
  }
}

void Panel::SetItemExpanded(bool expanded)
{
  if ( m_Canvas )
  {
    wxTreeItemId item = m_ItemData.GetId();
    if ( item != Helium::TreeWndCtrlItemIdInvalid )
    {
      Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Window;
      if ( treeWndCtrl->IsExpanded( item ) != expanded )
      {
        bool ignoreToggle = m_ItemData.GetIgnoreToggle();
        if ( !ignoreToggle )
        {
          m_ItemData.StartIgnoreToggle();
        }

        treeWndCtrl->SetExpanded( item, expanded );

        if ( !ignoreToggle )
        {
          m_ItemData.EndIgnoreToggle();
        }
      }
    }
  }
}

wxTreeItemId Panel::GetParentTreeNode(Container* parent)
{
  for ( ; parent != NULL; parent = parent->GetParent() )
  {
    Panel* parentPanel = Reflect::ObjectCast<Panel>( m_Parent );
    if ( parentPanel )
    {
      wxTreeItemId parentItem = parentPanel->m_ItemData.GetId();
      if ( parentItem != Helium::TreeWndCtrlItemIdInvalid )
      {
        return parentItem;
      }
    }
  }

  Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Window;
  return treeWndCtrl->GetRootItem();
}

void Panel::SetExpandedRecursive(bool expanded, bool force)
{
  SetExpanded( expanded, force );

  V_Control::iterator itr = m_Controls.begin();
  V_Control::iterator end = m_Controls.end();
  for ( ; itr != end; ++itr )
  {
    Panel* panel = ObjectCast<Panel>( *itr );
    if ( panel )
    {
      panel->SetExpandedRecursive( expanded, force );
    }
  }
}

bool Panel::IsExpandable()
{
  return m_Expandable;
}

void Panel::SetExpandable(bool expandable)
{
  m_Expandable = expandable;
}

bool Panel::IsCollapsable()
{
  return m_Collapsable;
}

void Panel::SetCollapsable(bool collapsable)
{
  m_Collapsable = collapsable;
}

bool Panel::GetShowTreeNode()
{
  return m_ShowTreeNode;
}


void Panel::SetShowTreeNode(bool showTreeNode)
{
  m_ShowTreeNode = showTreeNode;
}

PanelPtr Panel::CreatePanel(Control* control)
{
  if ( control == NULL )
    return NULL;

  // create a new control
  PanelPtr panel = control->GetCanvas()->Create<Panel>();

  panel->AddControl( control );

  return panel;
}

PanelPtr Panel::CreatePanel(const V_Control& controls)
{
  if ( controls.size() == 0 )
    return NULL;

  // create a new control
  PanelPtr panel = controls.front()->GetCanvas()->Create<Panel>();

  // spool controls to a fresh control
  V_Control::const_iterator itr = controls.begin();
  V_Control::const_iterator end = controls.end();
  for ( ; itr != end; ++itr )
    panel->AddControl((*itr));

  return panel;
}

void Panel::Read()
{
  if ( IsBound() )
  {
    ReadData( m_Text );

    if ( m_Canvas )
    {
      wxTreeItemId item = m_ItemData.GetId();
      if ( item != Helium::TreeWndCtrlItemIdInvalid )
      {
        Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Window;
        treeWndCtrl->SetItemText( item, m_Text );
      }
    }
  }

  __super::Read();
}

void Panel::SetText(const tstring& text)
{
  m_Text = text;

  if ( IsBound() )
  {
    WriteData( m_Text );
  }

  if ( m_Canvas )
  {
    wxTreeItemId item = m_ItemData.GetId();
    if ( item != Helium::TreeWndCtrlItemIdInvalid )
    {
      Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Window;
      treeWndCtrl->SetItemText( item, m_Text );
    }
  }
}

const tstring& Panel::GetText() const
{
  return m_Text;
}
