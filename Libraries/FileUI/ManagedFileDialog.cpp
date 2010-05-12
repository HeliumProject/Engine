#include "stdafx.h"

#include "ManagedFileDialog.h"

#include "Finder/Finder.h"
#include "File/Manager.h"
#include "Console/Console.h"
#include "FileSystem/FileSystem.h"

#include "Common/Types.h"

namespace File
{
  
  /////////////////////////////////////////////////////////////////////////////
  // Constructor
  // 
  ManagedFileDialog::ManagedFileDialog
    (
    wxWindow *parent,
    const wxString& message,                 /* = wxFileSelectorPromptStr, */
    const wxString& defaultDir,              /* = wxEmptyString, */
    const wxString& defaultFile,             /* = wxEmptyString, */
    const wxString& wildCard,                /* = wxFileSelectorDefaultWildcardStr, */
    UIToolKit::FileDialogStyle style,        /* = FileDialogStyles::DefaultOpen, */
    const wxPoint& pos,                      /* = wxDefaultPosition, */
    const wxSize& sz,                        /* = wxDefaultSize, */
    const wxString& name                     /* = wxFileDialogNameStr */
    )
    : UIToolKit::FileDialog( parent, message, defaultDir, defaultFile, wildCard, style, pos, sz, name )
    , m_IsTuidRequired( false )
    , m_RequestedFileID( TUID::Null )
  {
  }

  
  /////////////////////////////////////////////////////////////////////////////
  // Destructor
  // 
  ManagedFileDialog::~ManagedFileDialog()
  {
  }

  
  /////////////////////////////////////////////////////////////////////////////
  // Overridden from base class to provide TUID and FinderSpec validation.
  // 
  int ManagedFileDialog::ShowModal()
  {
    bool isDone = false;
    int result = wxID_CANCEL;

    m_FileIDs.clear();

    while ( !isDone && ( ( result = __super::ShowModal() ) == wxID_OK ) )
    {
      isDone = true;

      // Message to user if there is a problem with any of the selected files.
      std::string error;

      m_FileIDs.clear();
      S_string::const_iterator fileItr = m_Files.begin();
      S_string::const_iterator fileEnd = m_Files.end();
      for ( ; fileItr != fileEnd; ++fileItr )
      {
        if ( !FileSystem::HasPrefix( Finder::ProjectAssets(), *fileItr ) )
        {
          // Error
          error = "File '" + *fileItr + "' is not valid because it is not project asset directory: " + Finder::ProjectAssets() + ".";
          break;
        }

        if ( IsTuidRequired() )
        {
          tuid assignedFileID = TUID::Null;
          std::string error;
          if ( ManagedFileDialog::ValidateFileID( *fileItr, m_RequestedFileID, assignedFileID, error ) )
          {
            m_FileIDs.insert( assignedFileID );
          }
          else
          {
            // Error (message is already filled out)
            break;
          }
        }

        if ( !ManagedFileDialog::ValidateFinderSpec( *fileItr, error ) )
        {
          // Error (message is already filled out)
          break;
        }
      }

      if ( !error.empty() )
      {
        wxMessageBox( error.c_str(), "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );
        isDone = false;
      }
    }

    return result;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns true if this dialog requires all selected files to have a TUID.
  // 
  bool ManagedFileDialog::IsTuidRequired() const
  {
    return m_IsTuidRequired;
  }

  
  /////////////////////////////////////////////////////////////////////////////
  // Allows you to enable or diable TUID validation for this dialog.
  // 
  void ManagedFileDialog::SetTuidRequired( bool isRequired )
  {
    m_IsTuidRequired = isRequired;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns the TUID for the selected file (when dialog is in single-selct mode).
  // Only valid if ShowModal returned wx_ID_OK and the dialog has been set to require
  // TUIDs.
  // 
  tuid ManagedFileDialog::GetFileID() const
  {
    // Only call this function if you required the dialog to use TUIDs.
    NOC_ASSERT( IsTuidRequired() );

    // Only call this function when working with a dialog in single-select mode
    NOC_ASSERT( !IsMultipleSelectionEnabled() );

    tuid result = TUID::Null;
    if ( !m_FileIDs.empty() )
    {
      result = *m_FileIDs.begin();
    }
    return result;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns the TUIDs for the list of selected files.  Only valid if ShowModal
  // returned wxID_OK and the dialog has been set to require TUIDs.
  // 
  const S_tuid& ManagedFileDialog::GetFileIDs() const
  {
    // Only call this function if you required the dialog to use TUIDs.
    NOC_ASSERT( IsTuidRequired() );

    // Only call this function when working with a dialog in multi-select mode
    NOC_ASSERT( IsMultipleSelectionEnabled() );

    return m_FileIDs;
  }

  /////////////////////////////////////////////////////////////////////////////
  // This option only has meaning for single file select dialogs that are required
  // to return a file with a TUID.  If the file that is selected does not have
  // a TUID, this is the TUID that should be used (it will be checked to make
  // sure it does not already belong to something).
  // 
  void ManagedFileDialog::SetRequestedFileID( const tuid& request )
  {
    // Only call this function if you required the dialog to use TUIDs.
    NOC_ASSERT( IsTuidRequired() );

    // Only call this function when working with a dialog in single-select mode
    NOC_ASSERT( !IsMultipleSelectionEnabled() );

    m_RequestedFileID = request;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Validate that the path conforms to the specified finder spec.  Returns
  // true if the path is ok.  Returns false if there is a problem with the
  // path and fills out the error parameter with a description of the problem.
  // 
  bool ManagedFileDialog::ValidateFinderSpec( const std::string& path, std::string& error )
  {
    // TODO
    return true;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Validates that the specified path can be given a TUID.  You can specify
  // the idToTry if you already know what the TUID should be (set to TUID::Null
  // if you don't care what TUID the path gets).  This function will add the
  // path to the resolver if it is not already there.  If the path was able
  // to be resolved to a TUID, this function returns true and the fileID parameter
  // will contain the TUID that goes with the path.  If there is a problem, this
  // function returns false and the error parameter will contain a description
  // of the problem.
  // 
  bool ManagedFileDialog::ValidateFileID( const std::string& path, tuid idToTry, tuid& fileID, std::string& error )
  {
    fileID = TUID::Null;

    // If idToTry is not NULL, make sure it's legitimate
    if ( idToTry != TUID::Null )
    {
      std::string foundPath;
      if ( File::GlobalManager().GetPath( idToTry, foundPath ) )
      {
        if ( foundPath != path )
        {
          error = "Your file resolver is corrupt.  Please contact the tools department.";
          return false;
        }
      }
    }

    try
    {
      fileID = File::GlobalManager().Open( path, idToTry );
    }
    catch ( const Nocturnal::Exception& ex )
    {
      // Error trying to add this file to the resolver.
      fileID = TUID::Null;
      error = ex.what();
    }

    return fileID != TUID::Null;
  }


} // namespace UIToolKit
