#include "Precompile.h"
#include "ClassChooserDlg.h"
#include "EditorGenerated.h"
#include "Symbol/Inheritance.h"

// Using
using namespace Luna;

// Static event table
BEGIN_EVENT_TABLE( ClassChooserDlg, wxDialog )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// Constructor
// Pass in the parent window and the list of base classes that you want to 
// populate the dialog with.
// 
ClassChooserDlg::ClassChooserDlg( wxWindow* parent, const S_string& baseClasses )
: wxDialog( parent, wxID_ANY, "Class Chooser", wxDefaultPosition, wxSize( 460, 500 ), wxDEFAULT_DIALOG_STYLE |wxRESIZE_BORDER )
, m_Panel( new ClassChooserPanel( this ) )
{
  // UI initialization
	SetMinSize( wxSize( 460, 500 ) );
	wxBoxSizer* outerSizer;
	outerSizer = new wxBoxSizer( wxVERTICAL );
  outerSizer->Add( m_Panel, 1, wxEXPAND, 5 );
	SetSizer( outerSizer );
	Layout();

  // Populate the tree control with the class list
  const wxTreeItemId invisibleRoot = m_Panel->m_Tree->AddRoot( "INVISIBLE_ROOT" );
  S_string::const_iterator baseItr = baseClasses.begin();
  S_string::const_iterator baseEnd = baseClasses.end();
  for ( ; baseItr != baseEnd; ++baseItr )
  {
    const std::string& baseClass = *baseItr;
    RecurseDerived( invisibleRoot, baseClass );
  }
  m_Panel->m_Tree->SortChildren( invisibleRoot );

  m_Panel->m_Tree->SetFocus();

  PopulateDropdown( baseClasses ); 

  // clear this out now to force selection to be what we want
  //
  m_SelectedClass = ""; 

  SetSelectedClass( *baseClasses.begin() ); 

  // Conenct Events
  Connect( m_Panel->m_Tree->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( ClassChooserDlg::OnTreeItemSelected ), NULL, this );
  Connect( m_Panel->m_Tree->GetId(), wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( ClassChooserDlg::OnTreeItemActivated ), NULL, this );
  Connect( m_Panel->m_Dropdown->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ClassChooserDlg::OnChoice ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ClassChooserDlg::~ClassChooserDlg()
{
  // Disconnect Events
  Disconnect( m_Panel->m_Tree->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( ClassChooserDlg::OnTreeItemSelected ), NULL, this );
  Disconnect( m_Panel->m_Tree->GetId(), wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( ClassChooserDlg::OnTreeItemActivated ), NULL, this );
  Disconnect( m_Panel->m_Dropdown->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ClassChooserDlg::OnChoice ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Static helper function to recursively populate classList with baseClass and
// all of the derived classes of baseClass.
// 
void RecurseAddDerivedClasses( const std::string& baseClass, S_string& classList )
{
  classList.insert( baseClass );

  V_string derivedClasses;
  Symbol::Inheritance::GetDerived( baseClass, derivedClasses );
  V_string::const_iterator derivedItr = derivedClasses.begin();
  V_string::const_iterator derivedEnd = derivedClasses.end();
  for ( ; derivedItr != derivedEnd; ++derivedItr )
  {
    const std::string& derivedClass = *derivedItr;
    RecurseAddDerivedClasses( derivedClass, classList );
  }
}


void ClassChooserDlg::PopulateDropdown(const S_string& baseClasses)
{  
  S_string runtimeClassNames; 
  
  {
    S_string::const_iterator itr = baseClasses.begin(); 
    S_string::const_iterator end = baseClasses.end(); 

    for(itr; itr != end; ++itr)
    {
      RecurseAddDerivedClasses( *itr, runtimeClassNames); 
    }
  }

  {
    S_string::const_iterator itr = runtimeClassNames.begin(); 
    S_string::const_iterator end = runtimeClassNames.end(); 

    for(itr; itr != end; ++itr)
    {
      m_Panel->m_Dropdown->Append( *itr ); 
    }
  }

}

///////////////////////////////////////////////////////////////////////////////
// Returns the class that is selected in the UI.  Only valid if the user has
// clicked the OK button on the dialog.
// 
const std::string& ClassChooserDlg::GetSelectedClass() const
{
  return m_SelectedClass;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the class that is selected in the dialog.  This function will also
// update the tree control to Reflect the selected item.
// 
void ClassChooserDlg::SetSelectedClass( const std::string& className )
{
  if ( className == m_SelectedClass )
  {
    return; 
  }

  m_SelectedClass = className;
  M_TreeItems::iterator found = m_TreeItems.find( className );
  if ( found != m_TreeItems.end() )
  {
    const wxTreeItemId& item = found->second;
    if ( !m_Panel->m_Tree->IsSelected( item ) )
    {
      m_Panel->m_Tree->SelectItem( item );
    }
  }
  else
  {
    m_Panel->m_Tree->Unselect(); 
  }

  int dropdownIndex = m_Panel->m_Dropdown->FindString( className ); 
  if ( dropdownIndex != m_Panel->m_Dropdown->GetSelection() )
  {
    m_Panel->m_Dropdown->Select( m_Panel->m_Dropdown->FindString( className ) ); 
  }
  
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to populate the tree with all the derived classes of the
// specified base.
// 
void ClassChooserDlg::RecurseDerived( const wxTreeItemId& root, const std::string& baseClass )
{
  wxTreeItemId item = m_Panel->m_Tree->AppendItem( root, baseClass.c_str() );
  m_TreeItems.insert( M_TreeItems::value_type( baseClass, item ) );

  V_string derivedClasses;
  Symbol::Inheritance::GetDerived( baseClass, derivedClasses );
  V_string::const_iterator derivedItr = derivedClasses.begin();
  V_string::const_iterator derivedEnd = derivedClasses.end();
  for ( ; derivedItr != derivedEnd; ++derivedItr )
  {
    const std::string& derivedClass = *derivedItr;
    RecurseDerived( item, derivedClass );
  }
  m_Panel->m_Tree->SortChildren( item );
}

///////////////////////////////////////////////////////////////////////////////
// Overridden from wxWindow.  Called automatically when the user presses
// the OK button.  If the user has selected an item in the tree, the dialog
// will cache that item so that it can be fetched after the dialog is no longer
// being shown.  If no item has been selected, this function returns false,
// which prevents the dialog from closing.
// 
bool ClassChooserDlg::TransferDataFromWindow()
{
  bool isOk = false;
  wxTreeItemId item = m_Panel->m_Tree->GetSelection();
  if ( item.IsOk() )
  {
    std::string className = m_Panel->m_Tree->GetItemText( item ).c_str();
    SetSelectedClass( className );
    isOk = __super::TransferDataFromWindow();
  }
  return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Called when the user double-clicks on an item in the tree.  Fires an event
// that simulates the user clicking on the OK button of the dialog.  Allows 
// user to quickly select an item and dismiss the dialog in one action.
// 
void ClassChooserDlg::OnTreeItemActivated( wxTreeEvent& args )
{
  if ( args.GetItem().IsOk() )
  {
    // Simulate an OK button press so that the normal shut down code runs.
    wxCommandEvent okEvent( wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK );
    okEvent.SetEventObject( this );
    GetEventHandler()->AddPendingEvent( okEvent );
  }
}

void ClassChooserDlg::OnTreeItemSelected( wxTreeEvent& args )
{
  wxTreeItemId item = m_Panel->m_Tree->GetSelection();
  if( item.IsOk() )
  {
    std::string className = m_Panel->m_Tree->GetItemText( item ).c_str();
    SetSelectedClass( className );
  }
  else
  {
    SetSelectedClass(""); 
  }
}

void ClassChooserDlg::OnChoice( wxCommandEvent& event )
{
  int selection = event.GetSelection(); 
  SetSelectedClass( m_Panel->m_Dropdown->GetString( selection ).c_str() ); 

  event.StopPropagation();
}

