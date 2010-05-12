#include "stdafx.h"

#include "FileBackedPage.h"

#include "Wizard.h"
#include "WizardPanels.h"
#include "ManagedAsset.h"

#include "Asset/AssetClass.h"
#include "Common/String/Tokenize.h"
#include "File/Manager.h"
#include "FileBrowser/FileBrowser.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "UIToolKit/FileDialog.h"
#include "UIToolKit/ImageManager.h"

// Using
using namespace AssetManager;

static const u32   s_MaxAssetPathLength = 220;

///////////////////////////////////////////////////////////////////////////////
// Constructor - protected; You must derive from this class.
// 
FileBackedPage::FileBackedPage( Wizard* wizard, i32 attribTypeID, const Finder::FolderSpec& defaultFolder, const Finder::ModifierSpec& ext, const std::string& newLabel, const std::string& existingLabel )
: WizardPage< FileBackedAttrPanel >( wizard )
, m_AttributeTypeID( attribTypeID )
, m_DefaultFolder( &defaultFolder )
, m_Extension( &ext )
{
  m_Panel->m_RadioBtnNew->SetLabel( newLabel.c_str() );
  m_Panel->m_RadioBtnExisting->SetLabel( existingLabel.c_str() );

  m_Panel->m_ButtonFindExisting->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "magnify_16.png" ) );

  ConnectListeners();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
FileBackedPage::~FileBackedPage()
{
  DisconnectListeners();
}


///////////////////////////////////////////////////////////////////////////////
void FileBackedPage::ConnectListeners()
{
  m_Panel->m_RadioBtnNew->Connect( m_Panel->m_RadioBtnNew->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( FileBackedPage::OnRadioButtonSelected ), NULL, this );
  m_Panel->m_RadioBtnExisting->Connect( m_Panel->m_RadioBtnExisting->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( FileBackedPage::OnRadioButtonSelected ), NULL, this );
  m_Panel->m_ButtonExisting->Connect( m_Panel->m_ButtonExisting->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileBackedPage::OnButtonExistingClicked ), NULL, this );
  m_Panel->m_ButtonFindExisting->Connect( m_Panel->m_ButtonFindExisting->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileBackedPage::OnButtonFindExistingClicked ), NULL, this );
}

void FileBackedPage::DisconnectListeners()
{
  m_Panel->m_RadioBtnNew->Disconnect( m_Panel->m_RadioBtnNew->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( FileBackedPage::OnRadioButtonSelected ), NULL, this );
  m_Panel->m_RadioBtnExisting->Disconnect( m_Panel->m_RadioBtnExisting->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( FileBackedPage::OnRadioButtonSelected ), NULL, this );
  m_Panel->m_ButtonExisting->Disconnect( m_Panel->m_ButtonExisting->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileBackedPage::OnButtonExistingClicked ), NULL, this );
  m_Panel->m_ButtonFindExisting->Disconnect( m_Panel->m_ButtonFindExisting->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FileBackedPage::OnButtonFindExistingClicked ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Pass true to this function if you want the radio button for the "existing" 
// file to be checked when this dialog is shown.  Pass false if you want the 
// "new" file to be checked when this dialog is shown (this is the default
// behavior if you do not call this function).  Provided so that you can change
// which radio button is selected the first time this page is shown in the
// wizard.
// 
void FileBackedPage::SetOpenExisting( bool openExisting )
{
  m_Panel->m_RadioBtnExisting->SetValue( openExisting );
  m_Panel->m_RadioBtnNew->SetValue( !openExisting );
}

///////////////////////////////////////////////////////////////////////////////
// Allows you to specify the path to an existing file which will be shown when
// this page is displayed.
// 
void FileBackedPage::SetExistingPath( const std::string& path )
{
  m_Panel->m_FilePathExisting->SetValue( path.c_str() );
}

///////////////////////////////////////////////////////////////////////////////
// Called before the UI is shown.
// 
bool FileBackedPage::TransferDataToWindow()
{
  DisconnectListeners();

  const Reflect::Type* attribType = Reflect::Registry::GetInstance()->GetType( m_AttributeTypeID );

  // Set the page title
  std::string title = attribType->m_UIName;
  m_Panel->m_Title->SetLabel( title.c_str() );

  // Set the page description
  const char* desc = attribType->GetProperty( Asset::AssetProperties::ShortDescription ).c_str();
  m_Panel->m_Description->SetLabel( desc );
  m_Panel->m_Description->Wrap( m_Panel->GetMinWidth() - 10 );

  // Create reasonable file paths to start with
  std::string path = GetDefaultNewFilePath();
  m_Panel->m_FilePathNew->SetValue( path.c_str() );
  if ( !m_Panel->m_RadioBtnExisting->GetValue() )
  {
    // Only update the existing path if the user was not editing it when
    // they were last on this page.
    FileSystem::StripLeaf( path );
    m_Panel->m_FilePathExisting->SetValue( path.c_str() );
  }

  // Enable/disable controls
  UpdateEnableState();

  m_Panel->Layout();

  ConnectListeners();

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Called when the page is changed.  Stores the UI data back to the persistent
// data managed by the wizard.
// 
bool FileBackedPage::TransferDataFromWindow()
{
  std::string path;
  if ( m_Panel->m_RadioBtnNew->GetValue() )
  {
    // get and clean user input
    path = m_Panel->m_FilePathNew->GetValue().c_str();
    FileSystem::CleanName( path );

    if ( FileSystem::Exists( path ) )
    {
      if ( wxMessageBox( "The file you specified already exists.  Would you \nlike to create an association with this file?",
                         "Associate with existing file?", wxCENTER | wxYES_NO, GetWizard() ) == wxYES )
      {
        m_Panel->m_RadioBtnExisting->SetValue( true );
        m_Panel->m_FilePathExisting->SetValue( m_Panel->m_FilePathNew->GetValue() );
        m_Panel->m_RadioBtnExisting->SetFocus();
        UpdateEnableState();
        return false;
      }
      else
      {
        return false;
      }
    }
  }
  else if ( m_Panel->m_RadioBtnExisting->GetValue() )
  {
    // Note: if it doesn't exist in the resolver, it will be added later in the call to Finished

    // get and clean user input
    path = m_Panel->m_FilePathExisting->GetValue().c_str();
    FileSystem::CleanName( path );

    // Basic checks that this is a valid file that exists
    try
    {
      if ( !FileSystem::HasExtension( path, m_Extension->GetModifier() ) )
      {
        std::stringstream str;
        str << "The specified file is not valid. The file must end with \"" << m_Extension->GetFilter() + "\".";
        wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
        return false;
      }

      if ( !FileSystem::Exists( path ) )
      {
        wxMessageBox( "The file that you specified does not exist. \nPlease create the file before continuing.", "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
        return false;
      }

      if ( path.length() > s_MaxAssetPathLength )
      {
        std::stringstream str;
        str << "The file path that you specified exceeds the maximum allowed length of an asset path (" << s_MaxAssetPathLength << " charachers).";
        wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
        return false;
      }

      //const boost::regex s_MatchValidAssetFolder("(^[a-zA-Z]\\:$)|(^[a-zA-Z0-9]($|[\\w\\- ]*?[a-zA-Z0-9]$))", boost::match_single_line); 
      //boost::smatch  matchResult;
      //std::string folderPath = path;
      //FileSystem::StripExtension( folderPath );
      //V_string fileNameTokens; 
      //Tokenize( folderPath, fileNameTokens, "\\/" );
      //V_string::iterator pathItr = fileNameTokens.begin();
      //V_string::iterator pathEnd = fileNameTokens.end();
      //for ( ; pathItr != pathEnd; ++pathItr )
      //{
      //  if ( !boost::regex_match( *pathItr, matchResult, s_MatchValidAssetFolder ) )
      //  {
      //    std::string error = "An asset path may only contain alphanumeric characters, spaces and the \"_\" and \"-\" characters.";
      //    wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
      //    return false;
      //  }
      //}
      const boost::regex s_MatchValidAssetPath("^[a-zA-Z]\\:(/[a-zA-Z0-9]([\\w\\-\\. ]*?[a-zA-Z0-9])*){1,}[/]{0,1}$", boost::match_single_line); 
      boost::smatch  matchResult;
      if ( !boost::regex_match( path, matchResult, s_MatchValidAssetPath ) )
      {
        std::string error = "An asset path may only contain alphanumeric characters, spaces and the \"_\" and \"-\" characters.";
        wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
        return false;
      }
    }
    catch ( const Nocturnal::Exception& )
    {
      wxMessageBox( "The file that you specified is not valid.", "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
      return false;
    }
  }

  // Make sure that the path is in the project.
  if ( !Finder::IsInProject( path ) )
  {
    std::stringstream str;
    str << "The file you specified is not in the project root.  Make sure your file is located in:" << std::endl << Finder::ProjectRoot() + ".";
    wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
    return false;
  }

  // Set the summary page info and list of files (if any)
  m_PageSummary.clear();
  m_P4FileList.clear();

  std::string buffer;

  buffer = std::string( Reflect::Registry::GetInstance()->GetType( m_AttributeTypeID )->m_UIName.c_str() ) + ":\n";
  m_PageSummary.push_back( StyledText( &m_TitleTextAttr, buffer ) );

  buffer = std::string( "  o " ) + path + std::string( "\n" );
  m_PageSummary.push_back( StyledText( &m_DefaultTextAttr, buffer ) );

  m_P4FileList.push_back( path );

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the path to use for the "create new" file field.  Can be overridden
// in derived classes.
// 
std::string FileBackedPage::GetDefaultNewFilePath()
{
  std::string path( GetWizard()->GetNewFileLocation() );
  FileSystem::StripPrefix( Finder::ProjectAssets(), path );
  std::string temp = m_DefaultFolder->GetFolder();
  FileSystem::AppendPath( temp, path );
  path = temp;
  FileSystem::StripExtension( path );
  m_Extension->Modify( path );
  return path;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the wizard is finished.  Creates the file and sets up
// the appropriate attribute on the wizard's asset class.
// 
void FileBackedPage::Finished()
{
  // Make sure the asset class has the attribute that we want to edit.
  if ( GetWizard()->GetAssetClass()->ContainsAttribute( m_AttributeTypeID ) )
  {
    std::string path;
    tuid fileID;
    if ( m_Panel->m_RadioBtnNew->GetValue() )
    {
      // Create the file if it doesn't already exist
      // SHOULD WE JUST RECALCULATE THE DEFAULT DIRECTORY HERE???
      path = m_Panel->m_FilePathNew->GetValue().c_str();

      // Let the derived class actually create the file for us.
      fileID = CreateNewFile( path );
    }
    else if ( m_Panel->m_RadioBtnExisting->GetValue() )
    {
      // File already exists, get the tuid
      path = m_Panel->m_FilePathExisting->GetValue().c_str();

      try
      {
        fileID = File::GlobalManager().Open( path );
      }
      catch ( const File::Exception& e )
      {
        std::stringstream str;
        str << "Failed to create file " << path << "." << std::endl << "Reason: " << e.what();
        wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
        
        //Early out and end the transaction
        GetWizard()->ErrorOccurred();
        return;
      }
    }

    // if we dont have a fileId at the point, something bad happened
    if ( fileID == TUID::Null )
    {
      GetWizard()->ErrorOccurred();
      return;
    }

    // Associate the file with the attribute on the asset class.
    // Let the derived class hook this up to the attribute.
    Finished( fileID );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Enables or disables various pieces of the UI based upon the current radio
// button selection.
// 
void FileBackedPage::UpdateEnableState()
{
  m_Panel->m_FilePathNew->Enable( m_Panel->m_RadioBtnNew->GetValue() );
  m_Panel->m_FilePathExisting->Enable( m_Panel->m_RadioBtnExisting->GetValue() );
  m_Panel->m_ButtonExisting->Enable( m_Panel->m_RadioBtnExisting->GetValue() );
  m_Panel->m_ButtonFindExisting->Enable( m_Panel->m_RadioBtnExisting->GetValue() );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a radio button is selected.  Updates the UI.
// 
void FileBackedPage::OnRadioButtonSelected( wxCommandEvent& args )
{
  UpdateEnableState();
  args.Skip();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user clicks the button to browse for an existing file.
// Prompts the user for the file to use.
// 
void FileBackedPage::OnButtonExistingClicked( wxCommandEvent& args )
{
  std::string directory( m_Panel->m_FilePathExisting->GetValue().c_str() );
  std::string file( FileSystem::GetLeaf( directory ) );
  FileSystem::StripLeaf( directory );
  
  UIToolKit::FileDialog browserDlg( GetWizard(), "Open", directory.c_str(), file.c_str(), "", UIToolKit::FileDialogStyles::DefaultOpen );
  
  browserDlg.AddFilter( m_Extension->GetDialogFilter() );
  if ( browserDlg.ShowModal() == wxID_OK )
  {
    m_Panel->m_FilePathExisting->SetValue( browserDlg.GetPath() );
  }
}


///////////////////////////////////////////////////////////////////////////////
void FileBackedPage::OnButtonFindExistingClicked( wxCommandEvent& args )
{
  File::FileBrowser browserDlg( NULL, -1, "Select File" );

  std::string directory( GetDefaultNewFilePath() );
  FileSystem::StripLeaf( directory );
  browserDlg.SetDirectory( directory );

  browserDlg.AddFilter( *m_Extension );
  browserDlg.SetFilterIndex( *m_Extension );

  if ( browserDlg.ShowModal() == wxID_OK )
  {
    m_Panel->m_FilePathExisting->SetValue( browserDlg.GetPath().c_str() );
  }
}