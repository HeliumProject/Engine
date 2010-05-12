#include "stdafx.h"

#include "CreateAssetFileLocationPage.h"

#include "Wizard.h"
#include "AttributesPage.h"
#include "SummaryPage.h"
#include "WizardPanels.h"

#include "Asset/AssetClass.h"
#include "Asset/AssetInit.h"
#include "Asset/AssetTemplate.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"

#include <boost/regex.hpp> 


using namespace AssetManager;

///////////////////////////////////////////////////////////////////////////////
// Private Constructor - Use Create function to make a new instance of this
// class.
// 
CreateAssetFileLocationPage::CreateAssetFileLocationPage( CreateAssetWizard* wizard )
: FileLocationPage( wizard )
, m_RadioBox( NULL )
{
  DisconnectListeners();

  //const size_t numTemplates = ConfigParser::GetInstance()->GetTemplates().count( GetCreateAssetWizard()->GetAssetClass()->GetType() );
  const size_t numTemplates = GetCreateAssetWizard()->GetAssetTemplates().size();

  // If there are templates available for this asset class, set up a radio box of choices.
  if ( numTemplates > 0 )
  {
    Reflect::V_Element::const_iterator itr = GetCreateAssetWizard()->GetAssetTemplates().begin();
    Reflect::V_Element::const_iterator end = GetCreateAssetWizard()->GetAssetTemplates().end();

    // Default template will be the first one
    GetCreateAssetWizard()->SetCurrentTemplate( (Asset::AssetTemplate*)itr->Ptr() );

    wxArrayString choices;
    wxArrayString tooltips;

    choices.Alloc( numTemplates );
    tooltips.Alloc( numTemplates );

    // Build the radio box options from the list of available templates.
    for ( i32 index = 0; itr != end; ++itr, ++index )
    {
      const Asset::AssetTemplate* assetTemplate = (Asset::AssetTemplate*)itr->Ptr();
      choices.Add( assetTemplate->m_Name.c_str() );
      tooltips.Add( assetTemplate->m_Description.c_str() );
      m_RadioAssetTemplates.insert( M_RadioAssetTemplate::value_type( index, assetTemplate ) );
    }

    // Create the radio box.
    wxBoxSizer* radioSizer = new wxBoxSizer( wxVERTICAL );
    m_RadioBox = new wxRadioBox( m_Panel->m_ScrollWindow, wxID_ANY, "Choose a template", wxDefaultPosition, wxDefaultSize, choices, 0, wxRA_SPECIFY_ROWS );
    radioSizer->Add( m_RadioBox, 1, wxALL|wxEXPAND, 5 );

    // Set up the tooltips
    const i32 numTips = static_cast< i32 >( tooltips.Count() );
    for ( i32 tipIndex = 0; tipIndex < numTips; ++tipIndex )
    {
      m_RadioBox->SetItemToolTip( tipIndex, tooltips.Item( tipIndex ) );
    }

    // Add the radio box to the appropriate place on the panel.
    m_Panel->m_ScrollWindow->GetSizer()->Add( radioSizer, 0, wxEXPAND, 5 );
    m_Panel->m_ScrollWindow->Layout();
  }
  else
  {
    // No templates for this type, just set up some default attributes and continue
    GetCreateAssetWizard()->GetAssetClass()->MakeDefault();
  }

  // If there's only one template, no use even giving the user a choice.
  if ( numTemplates == 1 )
  {
    m_RadioBox->Hide();
  }

  // Create next page
  UIToolKit::WizardPage* nextPage = NULL;
  if ( numTemplates > 0 )
  {
    nextPage = AttributesPage::Create( GetCreateAssetWizard() );
    GetCreateAssetWizard()->SetAttributesPage( wxDynamicCast( nextPage, AttributesPage ) );
  }
  else
  {
    nextPage = SummaryPage::Create( GetCreateAssetWizard() );
  }
  SetNext( nextPage );

  // Create subfolder default value
  if ( GetCreateAssetWizard()->GetCurrentTemplate() != NULL )
  {
    m_AddDirectory = GetCreateAssetWizard()->GetCurrentTemplate()->m_DefaultAddSubDir;
    m_Panel->m_checkBoxCreateSubfolder->Enable( GetCreateAssetWizard()->GetCurrentTemplate()->m_ShowSubDirCheckbox );
  }

  ConnectListeners();
  if ( m_RadioBox )
  {
    m_RadioBox->Connect( m_RadioBox->GetId(), wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( CreateAssetFileLocationPage::OnTemplateChanged ), NULL, this );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
CreateAssetFileLocationPage::~CreateAssetFileLocationPage()
{
  if ( m_RadioBox )
  {
    m_RadioBox->Disconnect( m_RadioBox->GetId(), wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( CreateAssetFileLocationPage::OnTemplateChanged ), NULL, this );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Static creation function.  Returns a new (heap-allocated) instance of this 
// class.
// 
UIToolKit::WizardPage* CreateAssetFileLocationPage::Create( CreateAssetWizard* wizard )
{
  return new CreateAssetFileLocationPage( wizard );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when one of templates on this page is changed by the user.
// Updates the full path to the asset.
// 
void CreateAssetFileLocationPage::OnTemplateChanged( wxCommandEvent& args )
{
#pragma TODO("DefaultPath should be updated on template change.") 

  M_RadioAssetTemplate::const_iterator found = m_RadioAssetTemplates.find( m_RadioBox->GetSelection() );
  GetCreateAssetWizard()->SetCurrentTemplate( found->second );

  m_AddDirectory = GetCreateAssetWizard()->GetCurrentTemplate()->m_DefaultAddSubDir;
  m_Panel->m_checkBoxCreateSubfolder->Enable( GetCreateAssetWizard()->GetCurrentTemplate()->m_ShowSubDirCheckbox );

  TransferDataToWindow();
}

///////////////////////////////////////////////////////////////////////////////
// Builds a default path for the asset class from the name and current template.
// 
std::string CreateAssetFileLocationPage::GetDefaultDirectory() const
{
  std::string defaultDirectory = Finder::ProjectAssets();
  FileSystem::AppendPath( defaultDirectory, GetCreateAssetWizard()->GetCurrentTemplate()->m_DefaultRoot );

  if ( GetCreateAssetWizard()->GetCurrentTemplate()
    && !GetCreateAssetWizard()->GetCurrentTemplate()->m_DefaultFormat.empty() )
  {
    FileSystem::AppendPath( defaultDirectory, GetCreateAssetWizard()->GetCurrentTemplate()->m_DefaultFormat );

    // try to replace any keys in the path, sorry, pretty ugly first pass :(
    const boost::regex currentlySelectedAssetDirPattern( "\\%CurSelectedAssetDir\\%" );
    boost::smatch results; 

    if ( boost::regex_search( defaultDirectory, results, currentlySelectedAssetDirPattern ) 
      && !GetCreateAssetWizard()->GetSelectedAssetPath().empty() )
    {
      // we will always do a replacement now that we are inside this block
      // we will set this to a non-empty value if we can.
      std::string assetDir = GetCreateAssetWizard()->GetSelectedAssetPath();
      FileSystem::StripLeaf( assetDir );
      FileSystem::StripPrefix( Finder::ProjectAssets(), assetDir );

      // do the replacement here. 
      defaultDirectory = boost::regex_replace(defaultDirectory, currentlySelectedAssetDirPattern, assetDir.c_str()); 

      // just make sure it's clean
      FileSystem::CleanName( defaultDirectory );
    }
  }

  return defaultDirectory;
}


///////////////////////////////////////////////////////////////////////////////
// Called when switching pages.  Transfers data from the UI to the data managed
// by the wizard class.
// 
bool CreateAssetFileLocationPage::TransferDataFromWindow()
{
  if ( m_RadioBox )
  {
    M_RadioAssetTemplate::const_iterator found = m_RadioAssetTemplates.find( m_RadioBox->GetSelection() );
    if ( found == m_RadioAssetTemplates.end() )
    {
      wxMessageBox( "The specified template is not valid for some reason.\n Choose another template.", "Error", wxCENTER | wxICON_ERROR | wxOK, GetCreateAssetWizard() );
      return false;
    }
    else
    {
      GetCreateAssetWizard()->SetCurrentTemplate( found->second );
    }
  }

  return __super::TransferDataFromWindow();
}
