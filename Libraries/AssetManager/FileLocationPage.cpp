#include "stdafx.h"
#include "FileLocationPage.h"

#include "WizardPanels.h"

#include "Asset/AssetInit.h"
#include "Asset/AssetTemplate.h"
#include "Common/String/Tokenize.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"

#include <boost/regex.hpp> 

// Using
using namespace AssetManager;


static const char* s_PageTitle = "Asset File Location";
static const char* s_PageDescription = "Choose a name and directory where you would like to save your asset. A default location is provided. The full path to the asset is shown underneath the directory.";

static const char* s_DefaultAssetName = "<enter asset name>";
static const u32   s_MaxAssetPathLength = 220;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
FileLocationPage::FileLocationPage( Wizard* wizard )
: WizardPage< LocationPanel >( wizard )
, m_AddDirectory( false )
, m_GotDefaultDirectory ( false )
{
  // Set up description
  m_Panel->m_Title->SetLabel( s_PageTitle );
  m_Panel->m_Description->SetLabel( s_PageDescription );
  m_Panel->m_Description->Wrap( m_Panel->GetMinWidth() - 10 );

  if ( GetWizard()->GetOperationFlag() == OperationFlags::Duplicate )
  {
    m_Panel->m_StaticOrigPath->SetLabel( "Asset being duplicated:" );
  }
  else if ( GetWizard()->GetOperationFlag() == OperationFlags::Rename )
  {
    m_Panel->m_StaticOrigPath->SetLabel( "Asset being renamed:" );
  }
  else
  {
    m_Panel->m_DuplicatePathPanel->Hide();
  }

  m_Panel->Layout();

  // Create subfolder default value
  m_AddDirectory = GetWizard()->GetCurrentTemplate()->m_DefaultAddSubDir; 
  m_Panel->m_checkBoxCreateSubfolder->Enable( GetWizard()->GetCurrentTemplate()->m_ShowSubDirCheckbox );

  ConnectListeners();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
FileLocationPage::~FileLocationPage()
{
  DisconnectListeners();
}


///////////////////////////////////////////////////////////////////////////////
// Static creation function.  Returns a new (heap-allocated) instance of this 
// class.
// 
UIToolKit::WizardPage* FileLocationPage::Create( Wizard* wizard )
{
  return new FileLocationPage( wizard );
}

///////////////////////////////////////////////////////////////////////////////
void FileLocationPage::ConnectListeners()
{
  m_Panel->m_Name->Connect( m_Panel->m_Name->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( FileLocationPage::OnNameTextChanged ), NULL, this );
  m_Panel->m_ButtonDirectoryPicker->Connect( m_Panel->m_ButtonDirectoryPicker->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileLocationPage::OnButtonChooseDirectory ), NULL, this );
  m_Panel->m_Directory->Connect( m_Panel->m_Directory->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( FileLocationPage::OnDirectoryTextChanged ), NULL, this );
  m_Panel->m_checkBoxCreateSubfolder->Connect( m_Panel->m_checkBoxCreateSubfolder->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FileLocationPage::OnCheckCreateAssetSubfolder ), NULL, this );
}

void FileLocationPage::DisconnectListeners()
{
  m_Panel->m_Name->Disconnect( m_Panel->m_Name->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( FileLocationPage::OnNameTextChanged ), NULL, this );
  m_Panel->m_ButtonDirectoryPicker->Disconnect( m_Panel->m_ButtonDirectoryPicker->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileLocationPage::OnButtonChooseDirectory ), NULL, this );
  m_Panel->m_Directory->Disconnect( m_Panel->m_Directory->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( FileLocationPage::OnDirectoryTextChanged ), NULL, this );
  m_Panel->m_checkBoxCreateSubfolder->Disconnect( m_Panel->m_checkBoxCreateSubfolder->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FileLocationPage::OnCheckCreateAssetSubfolder ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
void FileLocationPage::SetDefaultDirectory( const std::string& defaultDir )
{
  m_Directory = defaultDir;
  m_GotDefaultDirectory = true;
}

///////////////////////////////////////////////////////////////////////////////
// Builds a default name for the asset class from the name and current template.
// 
std::string FileLocationPage::GetDefaultName() const
{
  std::string defaultName;

  if ( GetWizard()->GetAssetClass() && !GetWizard()->GetAssetClass()->GetShortName().empty() )
  {
    defaultName = GetWizard()->GetAssetClass()->GetShortName();
    FileSystem::StripExtension( defaultName, 2 );
  }
  
  if ( defaultName.empty() )
  {
    defaultName = s_DefaultAssetName;
  }

  return defaultName;
}


///////////////////////////////////////////////////////////////////////////////
// Builds a default path for the asset class from the name and current template.
// 
std::string FileLocationPage::GetDefaultDirectory() const
{
  if ( GetWizard()->GetAssetClass() && !GetWizard()->GetAssetClass()->GetFilePath().empty() )
  {
    std::string defaultDirectory = GetWizard()->GetAssetClass()->GetFilePath();
    FileSystem::StripLeaf( defaultDirectory );
    FileSystem::StripLeaf( defaultDirectory );
    return defaultDirectory;
  }
  else
  {
    std::string defaultDirectory = Finder::ProjectAssets();
    FileSystem::AppendPath( defaultDirectory, GetWizard()->GetCurrentTemplate()->m_DefaultRoot );
    return defaultDirectory;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the full path to the new asset (automatically ensures that the path
// ends in a directory that matches the file name).
// 
std::string FileLocationPage::ConstructFilePath() const
{
  std::string path = m_Directory;

  // add the asset sub folder
  if ( m_AddDirectory && FileSystem::GetLeaf( path ) != m_Name )
  {
    FileSystem::AppendPath( path, m_Name );
  }

  FileSystem::AppendPath( path, m_Name );

  // add the decoration spec to the file name
  if ( !GetWizard()->GetCurrentTemplate()->m_ModifierSpec.empty() )
  {
    ((const Finder::ModifierSpec*)Finder::GetFinderSpec( GetWizard()->GetCurrentTemplate()->m_ModifierSpec ))->Modify( path );
  }

  FileSystem::CleanName( path );

  return path;
}

///////////////////////////////////////////////////////////////////////////////
// Called just before the window is shown.  Populates the UI.
// 
bool FileLocationPage::TransferDataToWindow()
{
  DisconnectListeners();

  // Asset Name
  if ( m_Name.empty() )
  {
    m_Name = GetDefaultName();
  }
  m_Panel->m_Name->SetValue( m_Name.c_str() );
  
  if ( m_Name.compare( s_DefaultAssetName ) == 0 )
  {
    // highlight/select the default text
    m_Panel->m_Name->SetSelection( -1, -1 );
  }

  // Directory
  if ( !m_GotDefaultDirectory || m_Directory.empty() )
  {
    m_Directory = GetDefaultDirectory();
    m_GotDefaultDirectory = true;
  }
  m_Panel->m_Directory->SetValue( m_Directory.c_str() );

  //check the directory and highlight it if it's not allowed
  if ( !GetWizard()->GetCurrentTemplate()->ValidateDirectory( ConstructFilePath() ) )
  {
    std::string errorString = GetWizard()->GetCurrentTemplate()->m_AboutDirSettings;
    if ( errorString.empty() )
    {
      errorString = "The directory currently specified is not an allowed directory for this asset type.";
    }

    m_Panel->m_NewAssetPath->SetForegroundColour( wxColour( *wxRED ) ); 
    m_Panel->m_NewAssetPath->SetToolTip( errorString );
    m_Panel->m_NewAssetPath->Refresh();
  }
  else
  {
    m_Panel->m_NewAssetPath->SetForegroundColour( wxColour( *wxBLACK ) );  
    m_Panel->m_NewAssetPath->SetToolTip( "" );
    m_Panel->m_NewAssetPath->Refresh();
  }

    // Full Path to New and Old asset
  m_Panel->m_NewAssetPath->SetValue( ConstructFilePath().c_str() );
  m_Panel->m_OriginalAssetPath->SetValue( GetWizard()->GetAssetClass()->GetFilePath().c_str() );

  // Create subfolder
  m_Panel->m_checkBoxCreateSubfolder->SetValue( m_AddDirectory );

  ConnectListeners();
  
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// Called when switching pages.  Transfers data from the UI to the data managed
// by the wizard class.
// 
bool FileLocationPage::TransferDataFromWindow()
{
  wxBusyCursor bc;
  
  // Only do validation if traversing to the next wizard page
  if ( GetWizard()->GetDirection() != UIToolKit::PageDirections::Forward )
  {
    return true;
  }

  if ( m_Name.empty() || ( m_Name.compare( s_DefaultAssetName ) == 0 ) )
  {
    wxMessageBox( "Please fill in a name for the new asset.", "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
    return false;
  }

  if ( *m_Name.begin() == ' ' || *m_Name.rbegin() == ' ' )
  {
    wxMessageBox( "Asset name cannot begin or end with spaces.", "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
    return false;
  }
  
  // validate that the file name only contains alphanumeric characters, spaces and the "_" and "-"
  // boost::match_single_line means that ^ (start of line) can't be embedded after a newline. 
  const boost::regex s_MatchValidAssetName("^[a-zA-Z0-9]($|[\\w\\- ]*?[a-zA-Z0-9]$)", boost::match_single_line); 

  boost::smatch  matchResult; 
  if ( !boost::regex_match( m_Name, matchResult, s_MatchValidAssetName ) )
  {
    std::string error = "An asset name may only contain alphanumeric characters, spaces and the \"_\" and \"-\" characters.";
    wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
    return false;
  }

  if ( m_Directory.empty() )
  {
    wxMessageBox( "Please fill in a directory for the new asset.", "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
    return false;
  }

  std::string filePath = ConstructFilePath();

  if ( !GetWizard()->GetCurrentTemplate()->ValidateDirectory( filePath ) )
  {
    std::string error = GetWizard()->GetCurrentTemplate()->m_AboutDirSettings;
    if ( error.empty() )
    {
      error = "The asset you are creating is not allowed to be placed in the directory you have chosen.  Please place the asset in one of the proper allowed directories.";
    }

    wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
    return false;
  }

  if ( filePath.length() > s_MaxAssetPathLength )
  {
    std::stringstream str;
    str << "The file path that you specified exceeds the maximum allowed length of an asset path (" << s_MaxAssetPathLength << " charachers).";
    wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
    return false;
  }

#pragma TODO( "Add support for spell checking the asset file name" )

  const boost::regex s_MatchValidAssetPath("^[a-zA-Z]\\:(/[a-zA-Z0-9]([\\w\\-\\. ]*?[a-zA-Z0-9])*){1,}[/]{0,1}$", boost::match_single_line); 
  if ( !boost::regex_match( filePath, matchResult, s_MatchValidAssetPath ) )
  {
    std::string error = "An asset path may only contain alphanumeric characters, spaces and the \"_\" and \"-\" characters.";
    wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
    return false;
  }

  // ensure that the new file path is not already in the system
  if ( FileSystem::Exists( filePath ) || File::GlobalManager().GetID( filePath ) != TUID::Null )
  {
    std::string error = "A file already exists at that location.\nPlease specifiy a different file name or directory.";
    wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
    return false;
  }

  GetWizard()->SetNewFileLocation( filePath );

  return true;
}



///////////////////////////////////////////////////////////////////////////////
// Callback for when one of the text fields on this page is changed by the user.
// Updates the full path to the asset.
// 
void FileLocationPage::OnNameTextChanged( wxCommandEvent& args )
{
  m_Name = m_Panel->m_Name->GetValue().c_str();

  TransferDataToWindow();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when one of the text fields on this page is changed by the user.
// Updates the full path to the asset.
// 
void FileLocationPage::OnDirectoryTextChanged( wxCommandEvent& args )
{
  m_Directory = m_Panel->m_Directory->GetValue().c_str();
  
  TransferDataToWindow();
}


///////////////////////////////////////////////////////////////////////////////
// Callback for when the user clicks the button to browse for a directory.
// Opens the directory browser and updates the UI as appropriate.
// 
void FileLocationPage::OnButtonChooseDirectory( wxCommandEvent& args )
{
  std::string directory = m_Directory;

  wxDirDialog directoryChooser( GetWizard(), wxDirSelectorPromptStr, directory.c_str() );
  if ( directoryChooser.ShowModal() == wxID_OK )
  {
    // Update the UI with the new value
    std::string newPath;
    try
    {
      // get and clean user input
      newPath = directoryChooser.GetPath();
      FileSystem::CleanName( newPath );
    }
    catch ( const Nocturnal::Exception& )
    {
      wxMessageBox( "The directory that you specified is not valid.", "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
      return;
    }

    if ( !newPath.empty() )
    {
      m_Directory = newPath;
      TransferDataToWindow();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user checks the box to automatically create a subfolder
// using the asset name.
// 
void FileLocationPage::OnCheckCreateAssetSubfolder( wxCommandEvent& args )
{
  m_AddDirectory = args.IsChecked();
  
  TransferDataToWindow();
}