#include "Precompile.h"
#include "AnimGroupDlg.h"

#include "AssetEditorGenerated.h"
#include "AssetPreferences.h"

#include "Asset/AnimationConfigAsset.h"
#include "AnimToolLib/AnimConfig.h"
#include "CodeGen/Namespace.h"
#include "Editor/WindowSettings.h"

using namespace Luna;

static const std::string s_Delim = ": ";

///////////////////////////////////////////////////////////////////////////////
// Generic class for attaching string data to a list item.
// 
class StringIdentifier : public wxClientData
{
private:
  std::string m_ID;

public:
  StringIdentifier( const std::string id )
    : m_ID( id )
  {
  }

  inline const std::string& GetID() const
  {
    return m_ID;
  }
};

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AnimGroupDlg::AnimGroupDlg( wxWindow* parent, Asset::AnimationConfigAsset* animConfig, const std::string& title )
: wxDialog(  parent, wxID_ANY, wxT( title.c_str() ), wxDefaultPosition, wxSize( 445,300 ), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
, m_ButtonSizer( NULL )
, m_Panel( NULL )
, m_ButtonOK( NULL )
, m_AnimConfig( animConfig )
{
	m_Panel = new AnimGroupPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	SetSizeHints( m_Panel->GetMinSize(), wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	mainSizer->Add( m_Panel, 1, wxEXPAND | wxALL, 5 );
	
	m_ButtonSizer = new wxStdDialogButtonSizer();
	m_ButtonOK = new wxButton( this, wxID_OK );
	m_ButtonSizer->AddButton( m_ButtonOK );
	m_ButtonSizer->AddButton( new wxButton( this, wxID_CANCEL ) );
	m_ButtonSizer->Realize();
	mainSizer->Add( m_ButtonSizer, 0, wxBOTTOM|wxLEFT|wxEXPAND, 5 );

  // Populate the controls.
  PopulateCategoryChoice();
  PopulateModifierGrid();

  // Disable the property grid.  It will be enabled when the user chooses a category.
  m_Panel->m_PropertyGrid->Enable( false );
  m_ButtonOK->Enable( false );

  // Add listeners
  Connect( m_Panel->m_Category->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AnimGroupDlg::OnCategoryChoice ) );

	SetSizer( mainSizer );
	Layout();

  // Default position and size
  GetAssetEditorPreferences()->GetAnimGroupDlgWindowSettings()->ApplyToWindow( this );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AnimGroupDlg::~AnimGroupDlg()
{
  m_Panel->m_PropertyGrid->Clear();
}

///////////////////////////////////////////////////////////////////////////////
// Shows the dialog.  Overridden to store the window position and size when 
// finished.
// 
int AnimGroupDlg::ShowModal()
{
  int result = __super::ShowModal();
  GetAssetEditorPreferences()->GetAnimGroupDlgWindowSettings()->SetFromWindow( this );
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the category setting (only valid if the OK button was pressed).
// 
const std::string& AnimGroupDlg::GetCategory() const
{
  return m_Category;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the category displayed on this dialog.
// 
void AnimGroupDlg::SetCategory( const std::string& category )
{
  if ( m_Category != category )
  {
    if ( !category.empty() )
    {
      const CodeGen::Namespace* categoryNamespace = AnimToolLib::AnimConfig::GetInstance()->GetCategories();
      const CodeGen::EnumValue* foundVal = categoryNamespace->FindEnumValue( category );

      if ( !foundVal )
      {
        NOC_BREAK();
        return;
      }

      std::string categorySetting = foundVal->GetParent()->GetShortName() + s_Delim + foundVal->GetShortName();
      m_Panel->m_Category->SetStringSelection( categorySetting );
    }
    else
    {
      m_Panel->m_Category->SetSelection( wxNOT_FOUND );
    }

    OnCategoryChoice( wxCommandEvent() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Fills out the modifiers map that is passed in with the settings from the
// dialog (only valid if the OK button was pressed).  Format of the string map
// is (key, value) = (full modifier enum name, full enum value).
// 
void AnimGroupDlg::GetModifiers( M_string& modifiers ) const
{
  if ( GetCategory().empty() )
  {
    // Modifiers cannot be set if there is no category specifed.
    return;
  }

  const CodeGen::Namespace* modNamespace = AnimToolLib::AnimConfig::GetInstance()->GetModifiers();
  const M_i32str::const_iterator lookupEnd = m_ModifierLookup.end();
  wxPropertyGridIterator gridItr = m_Panel->m_PropertyGrid->GetIterator( wxPG_ITERATE_PROPERTIES );
  for ( ; !gridItr.AtEnd(); ++gridItr )
  {
    wxPGProperty* current = *gridItr;
    const CodeGen::Enum* modEnum = modNamespace->FindEnum( current->GetName().c_str() );
    NOC_ASSERT( modEnum );

    wxVariant variant = current->GetValue();
    i32 index = variant.GetInteger();
    M_i32str::const_iterator found = m_ModifierLookup.find( index );
    if ( found != lookupEnd )
    {
      if ( !modifiers.insert( M_string::value_type( modEnum->GetFullName(), found->second ) ).second )
      {
        // Shouldn't happen: attempted to insert the same modifier enumeration more than once.
        NOC_BREAK();
      }
    }
    else
    {
      // Shouldn't happen: unable to lookup the item selection in the table we built earlier.
      NOC_BREAK();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Sets all the modifier properties that are displayed on this dialog.  Each 
// entry on the modifiers string map should be:
//   (key, value) = (full modifier enum name, full enum value)
// 
void AnimGroupDlg::SetModifiers( const M_string& modifiers )
{
  const M_string::const_iterator modifiersEnd = modifiers.end();
  const CodeGen::Namespace* modifierNamespace = AnimToolLib::AnimConfig::GetInstance()->GetModifiers();
  const CodeGen::EnumValue* any = AnimToolLib::AnimConfig::GetInstance()->GetConstant( AnimToolLib::s_ModifierAny );
  wxPropertyGridIterator gridItr = m_Panel->m_PropertyGrid->GetIterator( wxPG_ITERATE_PROPERTIES );
  for ( ; !gridItr.AtEnd(); gridItr++ )
  {
    wxPGProperty* current = *gridItr;
    const CodeGen::Enum* modEnum = modifierNamespace->FindEnum( current->GetName().c_str() );
    if ( modEnum )
    {
      M_string::const_iterator found = modifiers.find( modEnum->GetFullName() );
      if ( found != modifiersEnd )
      {
        const std::string currentSetting = current->GetValueAsString().c_str();
        const CodeGen::EnumValue* enumVal = any;
        if ( found->second != any->GetFullName() )
        {
          enumVal = modEnum->FindValue( found->second );
        }

        if ( enumVal )
        {
          const std::string label = enumVal->GetShortName();
          if ( label != currentSetting )
          {
            current->SetValueFromString( label );
          }
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Fill out the category drop down.
// 
void AnimGroupDlg::PopulateCategoryChoice()
{
  m_Panel->m_Category->Append( "NO_CATEGORY", new StringIdentifier( "" ) );

  M_strbool::const_iterator itr = m_AnimConfig->m_Categories.begin();
  M_strbool::const_iterator end = m_AnimConfig->m_Categories.end();
  for ( ; itr != end; ++itr )
  {
    if ( itr->second )
    {
      const std::string& category = itr->first;
      const CodeGen::Enum* categoryEnum = AnimToolLib::AnimConfig::GetInstance()->GetRootNamespace()->FindEnum( category );
      if ( categoryEnum )
      {
        CodeGen::OS_EnumValueSmartPtr::Iterator valItr = categoryEnum->GetValues().Begin();
        CodeGen::OS_EnumValueSmartPtr::Iterator valEnd = categoryEnum->GetValues().End();
        for ( ; valItr != valEnd; ++valItr )
        {
          const CodeGen::EnumValuePtr& val = *valItr;
          std::string label = val->GetParent()->GetShortName() + s_Delim + val->GetShortName();
          m_Panel->m_Category->Append( label.c_str(), new StringIdentifier( val->GetFullName() ) );
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Fill out the grid control with modifier options.
// 
void AnimGroupDlg::PopulateModifierGrid()
{
  wxPGId modifiersCategory = m_Panel->m_PropertyGrid->AppendCategory( "Modifiers" );

  const CodeGen::EnumValue* any = AnimToolLib::AnimConfig::GetInstance()->GetConstant( AnimToolLib::s_ModifierAny );
  M_strbool::const_iterator itr = m_AnimConfig->m_Modifiers.begin();
  M_strbool::const_iterator end = m_AnimConfig->m_Modifiers.end();
  i32 id = 0;
  for ( ; itr != end; ++itr )
  {
    if ( itr->second )
    {
      const CodeGen::Enum* modifiers = AnimToolLib::AnimConfig::GetInstance()->GetRootNamespace()->FindEnum( itr->first );
      if ( modifiers )
      {
        wxArrayString choices;
        wxArrayInt choiceIDs;
        choices.Add( any->GetShortName() );
        choiceIDs.Add( any->GetValue() );

        CodeGen::OS_EnumValueSmartPtr::Iterator valItr = modifiers->GetValues().Begin();
        CodeGen::OS_EnumValueSmartPtr::Iterator valEnd = modifiers->GetValues().End();
        for ( ; valItr != valEnd; ++valItr )
        {
          const CodeGen::EnumValuePtr& val = *valItr;
          i32 currentID = ++id;

          choices.Add( val->GetShortName() );
          choiceIDs.Add( currentID );

          m_ModifierLookup.insert( M_i32str::value_type( currentID, val->GetFullName() ) );
        }
        m_Panel->m_PropertyGrid->AppendIn( modifiersCategory, new wxEnumProperty( modifiers->GetShortName().c_str(), wxPG_LABEL, choices, choiceIDs, any->GetValue() ) );
      }
    }
  }
  m_ModifierLookup.insert( M_i32str::value_type( any->GetValue(), any->GetFullName() ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the category drop down is changed.  Enables the OK button
// and the modifier grid.
// 
void AnimGroupDlg::OnCategoryChoice( wxCommandEvent& args )
{
  // Read the selection
  bool enableButton = false;
  m_Category.clear();
  StringIdentifier* data = static_cast< StringIdentifier* >( m_Panel->m_Category->GetClientObject( m_Panel->m_Category->GetSelection() ) );
  if ( data )
  {
    m_Category = data->GetID();
    enableButton = true;
  }

  // Enable the UI
  m_Panel->m_PropertyGrid->Enable( !m_Category.empty() );
  m_ButtonOK->Enable( enableButton );

  // Apparently property grid does not redraw when the enable state changes.
  m_Panel->m_PropertyGrid->Refresh();
}
