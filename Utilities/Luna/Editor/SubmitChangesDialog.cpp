#include "Precompile.h"
#include "SubmitChangesDialog.h"
#include "EditorGenerated.h"

#include "rcs/rcs.h"
#include "Platform/Process.h"

#include <sstream>

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
SubmitChangesDialog::SubmitChangesDialog( wxWindow* parent )
: wxDialog( parent, wxID_ANY, "Commit changes?", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
, m_Panel( NULL )
{
  m_Panel = new SubmitChangesPanel( this, wxID_ANY );
  SetMinSize( m_Panel->GetMinSize() );
  SetSize( m_Panel->GetSize() );

  wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
  sizer->Add( m_Panel, 1, wxEXPAND );
  SetSizer( sizer );
  Layout();

  // Callbacks
  m_Panel->m_ButtonYes->Connect( m_Panel->m_ButtonYes->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SubmitChangesDialog::OnButtonClicked ), NULL, this );
  m_Panel->m_ButtonNo->Connect( m_Panel->m_ButtonNo->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SubmitChangesDialog::OnButtonClicked ), NULL, this );
  m_Panel->m_ButtonCancel->Connect( m_Panel->m_ButtonCancel->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SubmitChangesDialog::OnButtonClicked ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SubmitChangesDialog::~SubmitChangesDialog()
{
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to only show the dialog if there is a changelist available.
// 
int SubmitChangesDialog::ShowModal()
{
  int result = wxID_NO;

  // If there are files in the default changelist, actually show the dialog.
  RCS::V_File files;

  try
  {
    RCS::GetOpenedFiles( files );
  }
  catch ( Nocturnal::Exception& ex )
  {
    std::stringstream str;
    str << "Unable to get opened files: " << ex.what();
    wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK );
    return result;
  }

  u32 numFiles = 0;
  wxArrayString listItems;
  RCS::V_File::const_iterator fileItr = files.begin();
  RCS::V_File::const_iterator fileEnd = files.end();
  for ( ; fileItr != fileEnd; ++fileItr )
  {
    const RCS::File& rcsFile = *fileItr;
    if ( rcsFile.m_ChangesetId == RCS::DefaultChangesetId )
    {
      listItems.Add( rcsFile.m_DepotPath.c_str() );
      ++numFiles;
    }
  }

  if ( numFiles > 0 )
  {
    m_Panel->m_ListBoxFiles->Append( listItems );

    result = __super::ShowModal();
  }
  else
  {
    // If there's no files in the changelist, ask the user if they want to launch P4V.
    if ( wxYES == wxMessageBox( "There are no files to commit in your default changelist.\nWould you like to launch P4V?", "Launch P4V?", wxYES_NO | wxCENTER | wxICON_QUESTION, GetParent() ) )
    {
      Platform::Execute( "p4v.exe" );
    }
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Attempts to checkin the default changelist.
// 
void SubmitChangesDialog::Commit()
{
  std::string error;

  RCS::Changeset changeset;
  changeset.m_Description = "Luna Session Manager: Commit Default Changelist";
  changeset.Create();
  if ( changeset.m_Id < 0 || changeset.m_Id == RCS::DefaultChangesetId )
  {
    error = "Could not create new Perforce changlist.";
  }

  RCS::V_File files;
  if ( error.empty() )
  {
    try
    {
      RCS::GetOpenedFiles( files );
    }
    catch ( const Nocturnal::Exception& e )
    {
      std::stringstream str;
      error = "Unable to get opened files: " + e.Get();
    }
  }

  bool changesetHasFiles = false;
  try
  {
    for ( RCS::V_File::iterator fileItr = files.begin(), fileEnd = files.end();
      fileItr != fileEnd; ++fileItr )
    {
      if ( ( *fileItr ).GetChangesetId() == RCS::DefaultChangesetId )
      {
        changeset.Reopen( *fileItr );
      }
    }
  }
  catch ( const Nocturnal::Exception& e )
  {
    error = "Failed to reopen files in the default change list: " + e.Get();
  }

  if ( error.empty() )
  {
    try
    {
      changeset.Commit();
    }
    catch ( const Nocturnal::Exception& e )
    {
      error = "Commit failed: " + e.Get();
    }
  }

  if ( !error.empty() )
  {
    error += "\n";
    error += "Would you like to launch P4V?";

    if ( wxYES == wxMessageBox( error, "Launch P4V?", wxYES_NO | wxCENTER | wxICON_ERROR, this ) )
    {
      Platform::Execute( "p4v.exe" );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when a button the dialog is pressed.  Carries out the command and
// ends the dialog.
// 
void SubmitChangesDialog::OnButtonClicked( wxCommandEvent& args )
{
  switch ( args.GetId() )
  {
  case wxID_YES:
    Commit();
    break;

  case wxID_NO:
    Platform::Execute( "p4v.exe" );
    break;

  case wxID_CANCEL:
    // Do nothing, just here for completeness
    break;
  }
  EndModal( args.GetId() );
}
