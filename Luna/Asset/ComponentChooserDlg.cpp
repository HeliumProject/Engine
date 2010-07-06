#include "Precompile.h"
#include "ComponentChooserDlg.h"

#include "AssetEditor.h"
#include "AssetEditorIDs.h"
#include "AssetEditorGenerated.h"
#include "AssetPreferences.h"
#include "ComponentExistenceCommand.h"
#include "PersistentDataFactory.h"

#include "Pipeline/Asset/AssetFlags.h"
#include "Pipeline/Component/ComponentCategories.h"
#include "Foundation/Container/Insert.h" 
#include "Application/UI/ArtProvider.h"
#include "Application/UI/SortableListView.h"

// Using
using namespace Luna;

// Alphabetically sorts attributes based upon UI name.
class ComponentSort
{
public:
  bool operator()( const Component::ComponentBase* first, const Component::ComponentBase* second ) const
  {
    // Standard definition of set decrees that this be the case.
    if ( first == second )
    {
      return false;
    }
    // If we make it here, just do a normal string comparison on the names.
    return first->GetClass()->m_UIName < second->GetClass()->m_UIName;
  }
};
typedef std::set< Component::ComponentBase*, ComponentSort > S_SortedComponent;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ComponentChooserDlg::ComponentChooserDlg( AssetEditor* editor, const wxPoint& pos, const wxSize& size )
: wxDialog( editor, -1, wxT( "Add Component" ), pos, size, wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER )
, m_Editor( editor )
, m_Panel( new ComponentCategoriesPanel( this ) )
, m_Toolbar( NULL )
, m_MenuView( NULL )
, m_CurrentList( NULL )
{
  SetMinSize( wxSize( 300, 300 ) );
  wxBoxSizer* mainSizer = new wxBoxSizer( wxVERTICAL );

  m_Toolbar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_HORIZONTAL, wxT( "AddComponentToolbar" ) );
  m_Toolbar->AddTool( AssetEditorIDs::AddComponent, wxT("Add"), wxArtProvider::GetBitmap( NOCTURNAL_UNKNOWN_ART_ID ), wxNullBitmap, wxITEM_NORMAL, wxT("Add the selected attribute to the current asset."), wxT("") );
  //TODO: Implement Help button
	//m_Toolbar->AddTool( AssetEditorIDs::ComponentHelp, wxT("Help"), wxArtProvider::GetBitmap( wxART_HELP_BROWSER ), wxNullBitmap, wxITEM_NORMAL, wxT("Display additional information in the Browser."), wxT("") );
	m_Toolbar->AddTool( AssetEditorIDs::ChangeComponentView, wxT("View"), wxArtProvider::GetBitmap( NOCTURNAL_UNKNOWN_ART_ID ), wxNullBitmap, wxITEM_NORMAL, wxT("Change the attribute view."), wxT("") );
	m_Toolbar->Realize();
  mainSizer->Add( m_Toolbar, 0, wxALL|wxEXPAND, 2 );
	
	m_MenuView = new wxMenu();
	wxMenuItem* viewIcon = new wxMenuItem( m_MenuView, ID_DEFAULT, wxString( wxT("Icon") ) , wxT(""), wxITEM_NORMAL );
	m_MenuView->Append( viewIcon );
	wxMenuItem* viewList = new wxMenuItem( m_MenuView, ID_DEFAULT, wxString( wxT("List") ) , wxT(""), wxITEM_NORMAL );
	m_MenuView->Append( viewList );
	wxMenuItem* viewReport = new wxMenuItem( m_MenuView, ID_DEFAULT, wxString( wxT("Report") ) , wxT(""), wxITEM_NORMAL );
	m_MenuView->Append( viewReport );
	wxMenuItem* viewSmallIcon = new wxMenuItem( m_MenuView, ID_DEFAULT, wxString( wxT("Small Icon") ) , wxT(""), wxITEM_NORMAL );
	m_MenuView->Append( viewSmallIcon );

  mainSizer->Add( m_Panel, 1, wxALL | wxEXPAND, 5 );

  Luna::AssetPreferences* preferences = GetAssetEditorPreferences();
  tstring selectedPageTitle;
  preferences->Get( preferences->ComponentChooserTab(), selectedPageTitle );

  // Add all the category panels
  Component::M_ComponentCategories::const_iterator categoryItr = Component::ComponentCategories::GetInstance()->GetCategories().begin();
  Component::M_ComponentCategories::const_iterator categoryEnd = Component::ComponentCategories::GetInstance()->GetCategories().end();
  for ( ; categoryItr != categoryEnd; ++categoryItr )
  {
    const Component::ComponentCategoryPtr& category = categoryItr->second;
    m_Categories.insert( M_TabToCategory::value_type( m_Panel->m_Tabs->GetPageCount(), category.Ptr() ) );
    Nocturnal::Insert<M_TabToAttribList>::Result inserted = m_ComponentLists.insert( M_TabToAttribList::value_type( m_Panel->m_Tabs->GetPageCount(), M_ListIdToAttrib() ) );
    NOC_ASSERT( inserted.second );
    tstring title = category->Name().c_str();
    m_Panel->m_Tabs->AddPage( GetCategoryPanel( category, inserted.first->second ), title, title == selectedPageTitle );
  }

  mainSizer->Layout();
  SetSizer( mainSizer );

  // Connect UI callbacks
  Connect( AssetEditorIDs::AddComponent, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( ComponentChooserDlg::OnAddComponent ), NULL, this );
  Connect( AssetEditorIDs::ChangeComponentView, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( ComponentChooserDlg::OnChangeView ), NULL, this );
  m_Panel->m_Tabs->Connect( m_Panel->m_Tabs->GetId(), wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( ComponentChooserDlg::OnNotebookPageChanged ), NULL, this );
  ComponentCategoryPanel* categoryPanel = reinterpret_cast< ComponentCategoryPanel* >( m_Panel->m_Tabs->GetCurrentPage() );
  m_CurrentList = categoryPanel->m_ListCtrl;
  ConnectListCallbacks();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ComponentChooserDlg::~ComponentChooserDlg()
{
  // Disconnect UI callbacks
  Disconnect( AssetEditorIDs::ChangeComponentView, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( ComponentChooserDlg::OnChangeView ), NULL, this );
  m_Panel->m_Tabs->Disconnect( m_Panel->m_Tabs->GetId(), wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( ComponentChooserDlg::OnNotebookPageChanged ), NULL, this );
  DisconnectListCallbacks();
 
  // Disconnect callbacks from inner controls (list events on each tab for example)?

  // Delete
  delete m_MenuView;
}

///////////////////////////////////////////////////////////////////////////////
// Helper function that creates a panel for the specified category, and populates
// the panel with the appropriate controls for showing the attributes that belong
// to this category.
// 
ComponentCategoryPanel* ComponentChooserDlg::GetCategoryPanel( const Component::ComponentCategoryPtr& category, M_ListIdToAttrib& attribList )
{
  ComponentCategoryPanel* panel = new ComponentCategoryPanel( m_Panel->m_Tabs );
  panel->m_ShortDescription->SetLabel( category->ShortDescription().c_str() );
  panel->m_ListCtrl->InsertColumn( ColumnName, TXT( "Name" ) );
  panel->m_ListCtrl->InsertColumn( ColumnError, TXT( "Error" ) );
  panel->m_ListCtrl->SetImageList( Nocturnal::GlobalFileIconsTable().GetSmallImageList(), wxIMAGE_LIST_NORMAL );
  panel->m_ListCtrl->SetImageList( Nocturnal::GlobalFileIconsTable().GetSmallImageList(), wxIMAGE_LIST_SMALL );
  
  // Sort the attributes alphabetically
  S_SortedComponent sorted;
  for ( Component::M_Component::const_iterator attrItr = category->Components().begin(),
    attrEnd = category->Components().end(); attrItr != attrEnd; ++attrItr )
  {
    sorted.insert( attrItr->second );
  }

  // Add the attributes to the list in order
  i32 id = 0; // this is the zero based index of the item within the list
  for ( S_SortedComponent::const_iterator attrItr = sorted.begin(),
    attrEnd = sorted.end(); attrItr != attrEnd; ++attrItr, ++id )
  {
    Component::ComponentBase* attribute = *attrItr;
    tstring errorMsg;
    bool isValid = ValidateComponent( attribute, errorMsg );

    // Add regular item
    wxListItem normalItem;
    normalItem.SetMask( wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE );
    normalItem.SetText( attribute->GetClass()->m_UIName.c_str() );
    if ( !isValid )
    {
      normalItem.SetTextColour( wxColour( *wxRED ) );
    }
    const tstring icon = Luna::ComponentWrapper::GetComponentIcon( attribute ); 
    normalItem.SetImage( Nocturnal::GlobalFileIconsTable().GetIconID( icon ) );
    normalItem.SetId( id );
    normalItem.SetColumn( ColumnName );
    panel->m_ListCtrl->InsertItem( normalItem );

    // Optionally add error item
    wxListItem errorItem;
    errorItem.SetMask( wxLIST_MASK_TEXT );
    errorItem.SetText( errorMsg.c_str() );
    errorItem.SetId( id );
    errorItem.SetColumn( ColumnError );
    panel->m_ListCtrl->SetItem( errorItem );

    AutoSizeNameColumn( panel->m_ListCtrl );

    // Keep a map that allows us to look up an attribute from the item id and current
    // tab page.  This way we don't have to use wxListItem::SetData with some nasty
    // casts.
    attribList.insert( M_ListIdToAttrib::value_type( id, attribute ) );
  }

  return panel;
}

///////////////////////////////////////////////////////////////////////////////
// Cycles through the different views for the specified list.
// 
void ComponentChooserDlg::NextView( wxListCtrl* list )
{
  i32 next = wxLC_ICON;
  i32 style = list->GetWindowStyle();
  
  if ( style & wxLC_REPORT )
  {
    next = wxLC_SMALL_ICON;
  }
  else if ( style & wxLC_SMALL_ICON )
  {
    next = wxLC_ICON;
  }
  else if ( style & wxLC_ICON )
  {
    next = wxLC_REPORT;
  }

  list->SetSingleStyle( next, true );
  AutoSizeNameColumn( list );
}

///////////////////////////////////////////////////////////////////////////////
// Displays a string and icon in the bottom panel of the dialog.
// 
void ComponentChooserDlg::SetDescription( const tstring& desc, ComponentChooserDlg::Icon icon )
{
  switch ( icon )
  {
  case IconNone:
    m_Panel->m_Icon->Hide();
    break;

  case IconError:
    m_Panel->m_Icon->SetBitmap( wxArtProvider::GetBitmap( NOCTURNAL_UNKNOWN_ART_ID, wxART_OTHER, wxSize( 32, 32 ) ) );
    m_Panel->m_Icon->Show();
    break;
  }
  m_Panel->m_Message->SetLabel( desc.c_str() );

  // Force the panel sizer to layout again so that the icon and message don't overlap.
  m_Panel->m_MessagePanel->GetSizer()->Layout();
}

///////////////////////////////////////////////////////////////////////////////
// Set the message panel based upon the specified attribute.
// 
void ComponentChooserDlg::SetDescription( Component::ComponentBase* attribute )
{
  SetDescription( attribute->GetClass()->m_UIName, IconNone );
}

///////////////////////////////////////////////////////////////////////////////
// Set the message panel based upon the specified category.
// 
void ComponentChooserDlg::SetDescription( Component::ComponentCategory* category )
{
  SetDescription( category->LongDescription(), IconNone );
}

///////////////////////////////////////////////////////////////////////////////
// Attempts to add the specified attribute to any asset classes that are 
// currently selected.
// 
void ComponentChooserDlg::AddComponent( const Component::ComponentPtr& attributeToClone )
{
  S_AssetClassDumbPtr selection;
  m_Editor->GetAssetManager()->GetSelectedAssets( selection );

  tostringstream errorMsg;

  // Make sure that something is selected
  if ( !selection.empty() )
  {
    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    // Iterate over selection list looking for asset classes
    S_AssetClassDumbPtr::const_iterator selItr = selection.begin();
    S_AssetClassDumbPtr::const_iterator selEnd = selection.end();
    for ( ; selItr != selEnd; ++selItr )
    {
      Luna::AssetClass* assetClass = *selItr;
      
      Component::ComponentPtr clone = Reflect::ObjectCast< Component::ComponentBase >( Reflect::Registry::GetInstance()->CreateInstance( attributeToClone->GetType() ) );
      tstring error;
      if ( assetClass->GetPackage< Asset::AssetClass >()->ValidateComponent( clone, error ) )
      {
        Luna::ComponentWrapperPtr lunaAttrib = PersistentDataFactory::GetInstance()->CreateTyped< Luna::ComponentWrapper >( clone, m_Editor->GetAssetManager() );
        batch->Push( new ComponentExistenceCommand( Undo::ExistenceActions::Add, assetClass, lunaAttrib ) );
      }
      else
      {
        errorMsg << "Unable to add attribute " << attributeToClone->GetClass()->m_UIName << " to " << assetClass->GetName() << ".  ";
        errorMsg << error << "\n";
      }
    }

    if ( !batch->IsEmpty() )
    {
      m_Editor->GetAssetManager()->Push( batch );
    }
  }

  const tstring& errorStr( errorMsg.str() );
  if ( !errorStr.empty() )
  {
    SetDescription( errorStr, IconError );
  }
  else
  {
    tostringstream msg;
    msg << TXT( "Added " ) << attributeToClone->GetClass()->m_UIName << TXT( "." );
    SetDescription( msg.str(), IconNone );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Find the attribute pointer from UI information about the tab index and the 
// list item index.
// 
Component::ComponentBase* ComponentChooserDlg::FindComponent( size_t tabNumber, i32 itemId )
{
  Component::ComponentBase* attribute = NULL;
  M_TabToAttribList::const_iterator foundList = m_ComponentLists.find( tabNumber );
  if ( foundList != m_ComponentLists.end() )
  {
    const M_ListIdToAttrib& list = foundList->second;
    M_ListIdToAttrib::const_iterator foundAttrib = list.find( itemId );
    if ( foundAttrib != list.end() )
    {
      attribute = foundAttrib->second;
    }
  }

  return attribute;
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over the current selection in the asset manager and checks to see
// if the specified attribute can be added to all the selected items.  If the
// attribute cannot be added to every selected item, this function returns
// false and the message parameter will contain information about what went
// wrong.
// 
bool ComponentChooserDlg::ValidateComponent( const Component::ComponentPtr& attribute, tstring& message )
{
  message.clear();
  bool isValid = false;

  Selection& selection = m_Editor->GetAssetManager()->GetSelection();

  // Make sure that something is selected
  if ( !selection.GetItems().Empty() )
  {
    try
    {
      // Iterate over selection list looking for asset classes
      OS_SelectableDumbPtr::Iterator selItr = selection.GetItems().Begin();
      OS_SelectableDumbPtr::Iterator selEnd = selection.GetItems().End();
      for ( ; selItr != selEnd; ++selItr )
      {
        Luna::AssetClass* assetClass = NULL;
        Luna::AssetNode* node = Reflect::ObjectCast< Luna::AssetNode >( *selItr );
        if ( node )
        {
          assetClass = node->GetAssetClass();
        }

        if ( assetClass )
        {
          // Make sure the attribute can be added to each asset class.
          Asset::AssetClass* assetClassData = assetClass->GetPackage< Asset::AssetClass >();
          isValid = assetClassData->ValidateComponent( attribute, message );
          if ( !isValid )
          {
            break;
          }
        }
      }
    }
    catch ( const Nocturnal::Exception& exception )
    {
      isValid = false;
      message = exception.What();
    }
  }
  else
  {
    isValid = false;
    message = TXT( "No asset class selected." );
  }

  return isValid;
}

///////////////////////////////////////////////////////////////////////////////
// Automatically sizes the name column for the specified list control so that
// the column fits the longest item in the list.
// 
void ComponentChooserDlg::AutoSizeNameColumn( wxListCtrl* list )
{
  // Make the name column large enough to show the name.
  i32 columnToAutoSize = ColumnName;
  i32 style = list->GetWindowStyle();
  if ( style & wxLC_ICON || style & wxLC_SMALL_ICON )
  {
    // From the wxListCtrl documentation:
    // In small or normal icon view, col must be -1, and the column width is set for all columns.
    columnToAutoSize = -1;
  }
  list->SetColumnWidth( columnToAutoSize, wxLIST_AUTOSIZE );
}

///////////////////////////////////////////////////////////////////////////////
// Connect needed listeners to the current list control (we only bother listening
// for callbacks on the currently visible list).
// 
void ComponentChooserDlg::ConnectListCallbacks()
{
  if ( m_CurrentList )
  {
    m_CurrentList->Connect( m_CurrentList->GetId(), wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ComponentChooserDlg::OnListItemDoubleClick ), NULL, this );
    m_CurrentList->Connect( m_CurrentList->GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ComponentChooserDlg::OnListItemSelected ), NULL, this );
    m_CurrentList->Connect( m_CurrentList->GetId(), wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( ComponentChooserDlg::OnListItemDeselected ), NULL, this );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Disconnect listeners from the currently list control.
// 
void ComponentChooserDlg::DisconnectListCallbacks()
{
  if ( m_CurrentList )
  {
    m_CurrentList->Disconnect( m_CurrentList->GetId(), wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ComponentChooserDlg::OnListItemDoubleClick ), NULL, this );
    m_CurrentList->Disconnect( m_CurrentList->GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ComponentChooserDlg::OnListItemSelected ), NULL, this );
    m_CurrentList->Disconnect( m_CurrentList->GetId(), wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( ComponentChooserDlg::OnListItemDeselected ), NULL, this );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the button is pressed to add an attribute.  Attempts to add the
// attribute.
// 
void ComponentChooserDlg::OnAddComponent( wxCommandEvent& args )
{
  Component::ComponentBase* attribute = FindComponent( m_Panel->m_Tabs->GetSelection(), m_CurrentList->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED ) );
  if ( attribute )
  {
    AddComponent( attribute );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when an item is double-clicked in one of the lists.  Shortcut for
// adding the attribute.
// 
void ComponentChooserDlg::OnListItemDoubleClick( wxListEvent& args )
{
  Component::ComponentBase* attribute = FindComponent( m_Panel->m_Tabs->GetSelection(), args.GetIndex() );
  if ( attribute )
  {
    AddComponent( attribute );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when a list item is selected.  Updates the message panel.
// 
void ComponentChooserDlg::OnListItemSelected( wxListEvent& args )
{
  Component::ComponentBase* attribute = FindComponent( m_Panel->m_Tabs->GetSelection(), args.GetIndex() );
  if ( attribute )
  {
    SetDescription( attribute );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when a list item is deselected.  Updates the message panel.
// 
void ComponentChooserDlg::OnListItemDeselected( wxListEvent& args )
{
  M_TabToCategory::const_iterator found = m_Categories.find( m_Panel->m_Tabs->GetSelection() );
  if ( found != m_Categories.end() )
  {
    SetDescription( found->second );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the change view button is pressed.  Cycles to the next view for
// all lists on all tabs.
// 
void ComponentChooserDlg::OnChangeView( wxCommandEvent& args )
{
  size_t pageCount = m_Panel->m_Tabs->GetPageCount();
  for ( size_t pageIndex = 0; pageIndex < pageCount; ++pageIndex )
  {
    ComponentCategoryPanel* categoryPanel = reinterpret_cast< ComponentCategoryPanel* >( m_Panel->m_Tabs->GetPage( pageIndex ) );
    if ( categoryPanel )
    {
      NextView( categoryPanel->m_ListCtrl );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when a new tab is selected.  When this function is called, a new list
// control will become the active one.  Listeners are disconnected from the 
// previous list and added to the new one.  The message panel is also updated.
// 
void ComponentChooserDlg::OnNotebookPageChanged( wxNotebookEvent& args )
{
  // Store the selected page in the preferences, we'll open back to this page next time.
  Luna::AssetPreferences* preferences = GetAssetEditorPreferences();
  preferences->Set( preferences->ComponentChooserTab(), tstring( m_Panel->m_Tabs->GetPageText( args.GetSelection() ) ) );

  ComponentCategoryPanel* categoryPanel = reinterpret_cast< ComponentCategoryPanel* >( m_Panel->m_Tabs->GetPage( args.GetSelection() ) );
  if ( m_CurrentList != categoryPanel->m_ListCtrl )
  {
    DisconnectListCallbacks();
    m_CurrentList = categoryPanel->m_ListCtrl;
    ConnectListCallbacks();

    long itemId = m_CurrentList->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
    if ( itemId >= 0 )
    {
      Component::ComponentBase* attribute = FindComponent( args.GetSelection(), itemId );
      if ( attribute )
      {
        SetDescription( attribute );
      }
    }
    else
    {
      M_TabToCategory::const_iterator foundCategory = m_Categories.find( args.GetSelection() );
      if ( foundCategory != m_Categories.end() )
      {
        SetDescription( foundCategory->second );
      }
    }
  }

  args.Skip();
}
