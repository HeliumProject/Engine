#include "stdafx.h"

#include "ShaderBrowser.h"


#include "File/Manager.h"
#include "File/ManagedFileDialog.h"
#include "FileSystem/FileSystem.h"

#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ShaderSpecs.h"

namespace Maya
{
  /////////////////////////////////////////////////////////////////////////////
  // Events table
  //
  BEGIN_EVENT_TABLE( ShaderBrowser, File::FileBrowser )
    EVT_BUTTON( wxID_OK, ShaderBrowser::OnOK )
    EVT_BUTTON( wxID_CANCEL, ShaderBrowser::OnCancel )
    EVT_CLOSE( ShaderBrowser::OnClose )
    EVT_LIST_ITEM_ACTIVATED( ID_ListCtrlResults, ShaderBrowser::OnItemActivated )     // The item has been double-clicked.
  END_EVENT_TABLE()


  /////////////////////////////////////////////////////////////////////////////
  // 
  // Ctor/Dtor
  //
  ShaderBrowser::ShaderBrowser( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style )
  : File::FileBrowser( parent, id, title, pos, size, style )
  , m_ShaderType( ShaderTypes::Unknown )
  {
    Init();
    Create( parent, id, title, pos, size, style );
  }

  ShaderBrowser::~ShaderBrowser()
  {
  }



  ////////////////////////////////////////////////////////
  // Common part of all ctors
  //
  void ShaderBrowser::Init()
  {
    m_DisplayColumnList.clear();
    m_DisplayColumnList.push_back( File::DisplayColumnTypes::Name );
    //m_DisplayColumnList.push_back( File::DisplayColumnTypes::FileType );
    m_DisplayColumnList.push_back( File::DisplayColumnTypes::Folder );
    //m_DisplayColumnList.push_back( File::DisplayColumnTypes::Size );
    //m_DisplayColumnList.push_back( File::DisplayColumnTypes::Id );

    SetFilter( FinderSpecs::Asset::SHADER_DECORATION );

    SetTuidRequired( true );
    //DisableMultipleSelection();
    EnableMultipleSelection();
  }


  ////////////////////////////////////////////////////////
  // Overload the create to add the lambert buttons
  //
  void ShaderBrowser::Create( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style )
  {
    this->SetSizeHints( wxSize( 800,600 ), wxDefaultSize );

    ////////////////////////////////////////////////////////
    // Add the lamber buttons
    m_panelDialogButtons->Destroy();

    m_panelDialogButtons = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

    wxSizer* panelSizer = m_panelDialogButtons->GetSizer();
    m_panelDialogButtons = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    
    // static line
    wxBoxSizer* bSizerButtonPanel;
    bSizerButtonPanel = new wxBoxSizer( wxVERTICAL );

    bSizerButtonPanel->Add( 0, 5, 0, wxALL, 0 );

    m_staticlineButtonLine = new wxStaticLine( m_panelDialogButtons, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    bSizerButtonPanel->Add( m_staticlineButtonLine, 0, wxEXPAND, 5 );

    // buttons
    wxBoxSizer* bSizerButtons;
    bSizerButtons = new wxBoxSizer( wxHORIZONTAL );

    m_buttonCreateLLambertShader = new wxButton( m_panelDialogButtons, wxID_OK, wxT("Lambert Shader"), wxDefaultPosition, wxDefaultSize, 0 );
    m_buttonCreateLLambertShader->Enable( false );
    bSizerButtons->Add( m_buttonCreateLLambertShader, 0, wxALL, 5 );

    m_buttonCreateLBlinnShader = new wxButton( m_panelDialogButtons, wxID_OK, wxT("Blinn Shader"), wxDefaultPosition, wxDefaultSize, 0 );
    m_buttonCreateLBlinnShader->Enable( false );
    bSizerButtons->Add( m_buttonCreateLBlinnShader, 0, wxALL, 5 );

    // cancel
    m_buttonCancel = new wxButton( m_panelDialogButtons, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizerButtons->Add( m_buttonCancel, 0, wxALIGN_RIGHT|wxALIGN_TOP|wxALL, 5 );

    bSizerButtonPanel->Add( bSizerButtons, 0, wxALIGN_BOTTOM|wxALIGN_RIGHT, 5 );

    m_panelDialogButtons->SetSizer( bSizerButtonPanel );
    m_panelDialogButtons->Layout();
    bSizerButtonPanel->Fit( m_panelDialogButtons );

    this->GetSizer()->Add( m_panelDialogButtons, 0, wxEXPAND, 5 );

    m_buttonCancel->Hide();
    m_buttonCancel->Enable( false );

    this->Layout();
    this->GetSizer()->Layout();
    this->GetSizer()->Fit( this );

  }


  /////////////////////////////////////////////////////////////////////////////
  // Called when the "Cancel" button is clicked. Ends the modal and
  // returns the default value of wxID_CANCEL
  //
  void ShaderBrowser::OnCancel( wxCommandEvent& evt )
  {
    Hide();
  }


  /////////////////////////////////////////////////////////////////////////////
  // Called when the |X| window close button is clicked. Ends the modal and
  // returns the default value of wxID_CANCEL
  //
  void ShaderBrowser::OnClose( wxCloseEvent& evt )
  {
    Hide();
  }


  /////////////////////////////////////////////////////////////////////////////
  // Caleld when the "OK" button is clicked. Does some error checking and the 
  // ends the modal and returns the wxID_OK
  //
  void ShaderBrowser::OnOK( wxCommandEvent& evt )
  {
    // get the button click shader type
    if ( evt.GetEventObject() == m_buttonCreateLLambertShader )
    {
      m_ShaderType = ShaderTypes::Lambert;
    }
    else if ( evt.GetEventObject() == m_buttonCreateLBlinnShader )
    {
      m_ShaderType = ShaderTypes::Blinn;
    }
    else 
    {
      m_ShaderType = ShaderTypes::Unknown;
      wxMessageBox( "Unknown shader type, this shouldn't be possible.", "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );
      return;
    }


    // get the shdaers
    m_ShaderFileIDs.clear();
    bool isDone = true;

    std::string error;

    const S_string& paths = GetPaths();
    for each ( const std::string& path in paths )
    {
      if ( IsTuidRequired() )
      {
        tuid fileID = TUID::Null;
        if ( File::ManagedFileDialog::ValidateFileID( path, TUID::Null, fileID, error ) )
        {
          m_ShaderFileIDs.insert( fileID );
        }
        else
        {
          // Error
          break;
        }
      }

      if ( !File::ManagedFileDialog::ValidateFinderSpec( path, error ) )
      {
        // Error
        break;
      }

      if ( !ValidatePath( path, error ) )
      {
        // Error
        break;
      }
    }

    if ( !error.empty() )
    {
      isDone = false;
      wxMessageBox( error.c_str(), "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );
    }


    if ( isDone )
    {
      for each ( const tuid& shaderId in m_ShaderFileIDs )
      {
        std::string shaderName = File::GlobalManager().GetPath( shaderId );
        FileSystem::StripPrefix( Finder::ProjectAssets(), shaderName );
        Maya::LoadShader( m_ShaderType, shaderId, shaderName );
      }
    }
  }



  /////////////////////////////////////////////////////////////////////////////
  // Called when an item is double-clicked.  
  // 
  void ShaderBrowser::OnItemActivated( wxListEvent& evt )
  {
    // don't end
    evt.Skip();
  }



  /////////////////////////////////////////////////////////////////////////////
  // 
  void ShaderBrowser::EnableOperationButton( bool enable )
  {
    m_buttonCreateLLambertShader->Enable( enable );
    m_buttonCreateLBlinnShader->Enable( enable );
  }

  
  /////////////////////////////////////////////////////////////////////////////
  // 
  bool ShaderBrowser::ValidatePath( const std::string& path, std::string& error )
  { 
    if ( !FileSystem::HasExtension( path, FinderSpecs::Asset::SHADER_DECORATION.GetDecoration() ) )
    {
      error = std::string( "File selected is not a shader, this can't happen!" );
      return false;
    }
    
    tuid shaderId = TUID::Null;
    shaderId = File::GlobalManager().GetID( path );
    if ( shaderId == TUID::Null )
    {
      error = std::string( "Could not locate the given shader in the file database!" );
      return false;
    }

    if ( !FileSystem::Exists( path ) )
    {
      error = std::string( "Could not locate the given shader on disk." );
      return false;
    }

    return true;
  }

} // namespace Maya