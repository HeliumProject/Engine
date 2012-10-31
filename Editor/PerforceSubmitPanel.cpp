#include "EditorPch.h"
#include "PerforceSubmitPanel.h"

#include <set>
#include <sstream>

#include "Platform/Environment.h"
#include "Foundation/Insert.h" 

#include <wx/msgdlg.h>

using namespace Helium;
using namespace Helium::Editor;

static const tchar_t* s_DefaultDescription = TXT( "<enter description here>" );

///////////////////////////////////////////////////////////////////////////////

static const tchar_t* g_OperationStrings[] = { 
    TXT( "<unknown>" ),
    TXT( "<none>" ),
    TXT( "<add>" ),
    TXT( "<edit>" ),
    TXT( "<delete>" ),
    TXT( "<branch>" ),
    TXT( "<integrate>" )
};

inline const tchar_t* GetOperationString( RCS::Operation operation )
{
    if ( operation > sizeof( g_OperationStrings ) )
    {
        return g_OperationStrings[ RCS::Operations::Unknown ];
    }

    return g_OperationStrings[operation];
}

///////////////////////////////////////////////////////////////////////////////
PerforceSubmitPanel::PerforceSubmitPanel
( 
 wxWindow* parent, 
 int id,
 int changelist,
 const tstring& description,
 const PanelStyle panelStyle,
 const tstring& title,
 const tstring& titleDescription
 )
 : PerforceSubmitPanelGenerated( parent, id, wxDefaultPosition, wxSize( 550,400 ), wxTAB_TRAVERSAL )
 , m_PanelStyle( panelStyle )
 , m_Title( title )
 , m_TitleDescription( titleDescription )
 , m_ReopenFiles( false )
 , m_Action( PerforceSubmitActions::Cancel )
{
    m_FilePaths.clear();

    /*
    V_VersionInfo info;
    Opened( info, m_Changelist );

    V_VersionInfo::const_iterator itr = info.begin();
    V_VersionInfo::const_iterator end = info.end();
    for ( ; itr != end; ++itr )
    {
    m_FilePaths.push_back( itr->m_DepotPath );
    }

    if ( m_Description.empty() )
    {
    m_Description = s_DefaultDescription;
    }
    */
    Populate();
    Layout();

    m_DescriptionTextCtrl->SetFocus();

    ConnectListeners();
}

///////////////////////////////////////////////////////////////////////////////
PerforceSubmitPanel::~PerforceSubmitPanel()
{
    m_FilePaths.clear();

    DisconnectListeners();
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::SetChangeset( const RCS::Changeset& changeset, bool getFiles )
{
    m_Changeset = changeset;

    if ( getFiles )
    {
        m_FilePaths.clear();

        RCS::V_File openedFiles;
        RCS::GetOpenedFiles( openedFiles );

        for( RCS::V_File::const_iterator itr = openedFiles.begin(), end = openedFiles.end(); itr != end; ++itr )
        {
            if ( (*itr).m_ChangesetId == m_Changeset.m_Id )
            {
                m_FilePaths.push_back( (*itr).m_DepotPath );
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::SetFileList( const std::vector< tstring >& filePaths )
{
    m_FilePaths.clear();
    m_FilePaths = filePaths;
}


///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::SetChangeDescription( const tstring& description )
{
    if ( m_Changeset.m_Description != description )
    {
        m_Changeset.m_Description = description;
    }
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::SetJobStatus( const tstring& jobStatus )
{
    if ( m_JobStatus != jobStatus )
    {
        m_JobStatus = jobStatus;
    }
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::SetReopenFiles( bool reopenFiles )
{ 
    if ( m_ReopenFiles != reopenFiles )
    {
        m_ReopenFiles = reopenFiles;
    }
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::SetTitleDescription( const tstring& titleDescription )
{
    if ( m_TitleDescription != titleDescription )
    {
        m_TitleDescription = titleDescription;
    }
}

///////////////////////////////////////////////////////////////////////////////
bool PerforceSubmitPanel::IsFileSelected( const tstring& depotPath )
{
    if ( !ShouldShowCommitButtons() )
        return true;

    M_FileItemTable::iterator foundIndex = m_FileItemTable.find( depotPath );
    if ( ( foundIndex != m_FileItemTable.end() )
        && m_FileCheckList->IsChecked( foundIndex->second ) )
    {
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool PerforceSubmitPanel::TransferDataToForm()
{
    DisconnectListeners();

    Clear();
    Populate();

    ConnectListeners();

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool PerforceSubmitPanel::TransferDataFromForm()
{
    if ( HasCancelAction() )
    {
        return true;
    }

    m_Changeset.m_Description = m_DescriptionTextCtrl->GetValue().c_str();
    if ( m_Changeset.m_Description.empty() || ( m_Changeset.m_Description.compare( s_DefaultDescription ) == 0 ) )
    {
        wxMessageBox( TXT( "Please enter a description." ), TXT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, GetParent() );
        return false;
    }

    m_JobStatus   = m_JobStatusChoice->GetStringSelection().c_str();
    m_ReopenFiles = m_ReopenCheckBox->IsChecked();


    if ( HasSubmitAction() )
    {
        // verify that we have at least one file selected
        bool foundOne = false;

        for each ( const tstring depotPath in m_FilePaths )
        {
            if ( IsFileSelected( depotPath ) )
            {
                foundOne = true;
                break;
            }
        }

        if ( HasSubmitAction() && !foundOne )
        {
            wxMessageBox( TXT( "You cannot submit a changelist unless there are info with checkmarks to be included in the submission." ), TXT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, GetParent() );
            return false;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::Clear()
{
    m_DescriptionTextCtrl->Clear();  
    m_FileCheckList->Clear();
    m_FileListBox->Clear();
    m_FileItemTable.clear();
}


///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::Populate()
{
    m_TitlePanel->Show( ShouldShowTitle() ); 
    if ( ShouldShowTitle() )
    {
        m_TitleStaticText->SetLabel( m_Title );
        m_PanelDescriptionStaticText->SetLabel( m_TitleDescription );
        m_PanelDescriptionStaticText->Wrap( GetMinWidth() - 10 );
    }

    m_ClientDetailsPanel->Show( ShouldShowClientDetails() );
    if ( ShouldShowClientDetails() )
    {
        tstringstream changelistStr;
        changelistStr << m_Changeset.m_Id;
        m_ChangeStaticText->SetLabel( changelistStr.str().c_str() );

        __time64_t now;
        _time64( &now );

        struct tm *dateTime;
        dateTime = _localtime64( &now );

        tchar_t dateTimeStr[128];
        _tcsftime( dateTimeStr, 128, TXT( "%Y/%m/%d %H:%M:%S" ), dateTime );
        m_DateStaticText->SetLabel( dateTimeStr );

        m_StatusStaticText->SetLabel( TXT( "pending" ) );
    }

    if ( m_Changeset.m_Description.empty() )
    {
        m_Changeset.m_Description = s_DefaultDescription;
    }
    m_DescriptionTextCtrl->SetValue( m_Changeset.m_Description.c_str() );

    if ( m_Changeset.m_Description.compare( s_DefaultDescription ) == 0 )
    {
        m_DescriptionTextCtrl->SetFocus();
        m_DescriptionTextCtrl->SelectAll();
    }

    m_JobStatusPanel->Show( ShouldShowJobStatus() );
    if ( ShouldShowJobStatus() )
    {
        m_JobStatusChoice->SetStringSelection( m_JobStatus );
    }

    bool shouldShowCommitButtons = ShouldShowCommitButtons();

    m_FileListBox->Show( !shouldShowCommitButtons );

    m_CommitButtonsPanel->Show( shouldShowCommitButtons );
    m_FileCheckList->Show( shouldShowCommitButtons );
    m_SelectAllButtonsPanel->Show( shouldShowCommitButtons );

    wxListBox* listBox = ( shouldShowCommitButtons ? m_FileCheckList : m_FileListBox );

    //File File;

    for each ( const tstring depotPath in m_FilePaths )
    {
        //GetInfo( depotPath, File );

        tstring displayPath = depotPath;
        //displayPath += " ";
        //displayPath += GetOperationString( File.m_Operation );

        Helium::StdInsert<M_FileItemTable>::Result inserted = m_FileItemTable.insert( M_FileItemTable::value_type( depotPath, listBox->Append( displayPath.c_str() ) ) );
        if ( shouldShowCommitButtons && inserted.second )
        {
            m_FileCheckList->Check( inserted.first->second, true ); //File.IsCheckedOutByMe() );
        }
    }

    m_ReopenCheckBox->SetValue( m_ReopenFiles );

    Layout();
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::ConnectListeners()
{
    // Reconnect listeners
    m_SelectAllButton->Connect( m_SelectAllButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PerforceSubmitPanel::OnSelectAllButtonClick ), NULL, this );
    m_UnselectButton->Connect( m_UnselectButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PerforceSubmitPanel::OnUnselectAllButtonClick ), NULL, this );
    m_SubmitButton->Connect( m_SubmitButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PerforceSubmitPanel::OnSubmitButtonClick ), NULL, this );
    m_UpdateButton->Connect( m_UpdateButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PerforceSubmitPanel::OnUpdateButtonClick ), NULL, this );
    m_CancelButton->Connect( m_CancelButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PerforceSubmitPanel::OnCancelButtonClick ), NULL, this );
    m_SpecNoteButton->Connect( m_SpecNoteButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PerforceSubmitPanel::OnSpecNotesButtonClick ), NULL, this );
    m_HelpButton->Connect( m_HelpButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PerforceSubmitPanel::OnHelpButtonClick ), NULL, this );

}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::DisconnectListeners()
{
    // Disconnect listeners
    m_SelectAllButton->Disconnect( m_SelectAllButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PerforceSubmitPanel::OnSelectAllButtonClick ), NULL, this );
    m_UnselectButton->Disconnect( m_UnselectButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PerforceSubmitPanel::OnUnselectAllButtonClick ), NULL, this );
    m_SubmitButton->Disconnect( m_SubmitButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PerforceSubmitPanel::OnSubmitButtonClick ), NULL, this );
    m_UpdateButton->Disconnect( m_UpdateButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PerforceSubmitPanel::OnUpdateButtonClick ), NULL, this );
    m_CancelButton->Disconnect( m_CancelButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PerforceSubmitPanel::OnCancelButtonClick ), NULL, this );
    m_SpecNoteButton->Disconnect( m_SpecNoteButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PerforceSubmitPanel::OnSpecNotesButtonClick ), NULL, this );
    m_HelpButton->Disconnect( m_HelpButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PerforceSubmitPanel::OnHelpButtonClick ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::ShowTitle( const tstring& title, const tstring& description )
{
    if ( !ShouldShowTitle() 
        || title.compare( m_Title ) != 0 
        || description.compare( m_Changeset.m_Description ) != 0 )
    {
        m_Title = title;
        m_TitleDescription = description;

        m_PanelStyle |= PerforceSubmitPanelStyles::Title;

        TransferDataToForm();
    }
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::HideTitle()
{
    if ( ShouldShowTitle() )
    {
        m_PanelStyle &= ~PerforceSubmitPanelStyles::Title;

        TransferDataToForm();
    }
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::ShowJobStatus( bool show )
{
    if ( show != ShouldShowJobStatus() )
    {
        if ( show )
        {
            m_PanelStyle |= PerforceSubmitPanelStyles::JobStatus;
        }
        else 
        {
            m_PanelStyle &= ~PerforceSubmitPanelStyles::JobStatus;
        }
        TransferDataToForm();
    }
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::ShowClientDetails( bool show )
{
    if ( show != ShouldShowClientDetails() )
    {
        if ( show )
        {
            m_PanelStyle |= PerforceSubmitPanelStyles::ClientDetails;
        }
        else 
        {
            m_PanelStyle &= ~PerforceSubmitPanelStyles::ClientDetails;
        }
        TransferDataToForm();
    }
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::ShowCommitButtons( bool show )
{
    if ( show != ShouldShowCommitButtons() )
    {
        if ( show )
        {
            m_PanelStyle |= PerforceSubmitPanelStyles::CommitButtons;
        }
        else 
        {
            m_PanelStyle &= ~PerforceSubmitPanelStyles::CommitButtons;
        }
        TransferDataToForm();
    }
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::OnSelectAllButtonClick( wxCommandEvent& event )
{ 
    m_FileCheckList->Freeze();
    for each ( const M_FileItemTable::value_type& indexPair in m_FileItemTable )
    {
        m_FileCheckList->Check( indexPair.second, true );
    }
    m_FileCheckList->Thaw();

    event.Skip(); 
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::OnUnselectAllButtonClick( wxCommandEvent& event )
{ 
    m_FileCheckList->Freeze();
    for each ( const M_FileItemTable::value_type& indexPair in m_FileItemTable )
    {
        m_FileCheckList->Check( indexPair.second, false );
    }
    m_FileCheckList->Thaw();

    event.Skip(); 
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::OnSubmitButtonClick( wxCommandEvent& event )
{ 
    m_Action = PerforceSubmitActions::Submit;

    event.Skip(); 
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::OnUpdateButtonClick( wxCommandEvent& event )
{ 
    m_Action = PerforceSubmitActions::Update;

    event.Skip(); 
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::OnCancelButtonClick( wxCommandEvent& event )
{ 
    m_Action = PerforceSubmitActions::Cancel;

    event.Skip(); 
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::OnSpecNotesButtonClick( wxCommandEvent& event )
{ 
    event.Skip(); 
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitPanel::OnHelpButtonClick( wxCommandEvent& event )
{ 
    event.Skip(); 
}


inline bool CanReopenFile( RCS::Operation operation )
{
    return ( ( operation == RCS::Operations::Add )
        || ( operation == RCS::Operations::Branch )
        || ( operation == RCS::Operations::Edit )
        || ( operation == RCS::Operations::Integrate ) );
}

///////////////////////////////////////////////////////////////////////////////
// Gets the list of FilesInfos and submits the changelist if PerforceSubmitAction::Submit is true.
// 
void PerforceSubmitPanel::CommitChanges()
{
    if ( m_Action == PerforceSubmitActions::Cancel )
    {
        return;
    }

    // gather the list of info to submit and reopn info if they should 
    // be removed from this changelist 
    std::set< tstring > reopenFilePaths;

    for ( std::vector< tstring >::const_iterator itr = m_FilePaths.begin(), end = m_FilePaths.end(); itr != end; ++itr )
    {
        // if the commit buttons are showing (meaning that the user also 
        // sees the checkboxes), and the file was unselected - move it
        // to the default changelist
        if ( ShouldShowCommitButtons() && !IsFileSelected( *itr ) )
        {
            // move it to the default changelist
            RCS::File rcsFile( *itr );
            RCS::DefaultChangeset().Reopen( rcsFile ); 
            continue;
        }

        RCS::File rcsFile( *itr );
        rcsFile.GetInfo();

        // if the file can and should be reopened after submit, add it to the list
        if ( m_ReopenFiles && IsFileSelected( *itr ) && CanReopenFile( rcsFile.m_Operation ) )
        {
            reopenFilePaths.insert( *itr );
        }

        // if for what ever reason the file is not in this changelist, move it
        if ( rcsFile.m_ChangesetId != m_Changeset.m_Id )
        {
            m_Changeset.Reopen( rcsFile );
        }
    }

    if ( m_Action == PerforceSubmitActions::Submit )
    {
        if ( m_Changeset.m_Id == RCS::DefaultChangesetId )
        {
            RCS::V_File openedFiles;
            RCS::GetOpenedFiles( openedFiles );

            if ( openedFiles.size() > 0 )
            {
                m_Changeset.Create();
                if ( m_Changeset.m_Description.empty() )
                {
                    Helium::GetUsername( m_Changeset.m_Description );
                }

                RCS::V_File::iterator fItr = openedFiles.begin();
                RCS::V_File::iterator fEnd = openedFiles.end();
                for ( ; fItr != fEnd; ++fItr )
                {
                    m_Changeset.Reopen( (*fItr) );
                }
            }
        }

        m_Changeset.Commit();
        m_Changeset.Clear();

        // reopen info
        if ( m_ReopenFiles )
        {
            for( std::set< tstring >::const_iterator reopenItr = reopenFilePaths.begin(), reopenEnd = reopenFilePaths.end(); reopenItr != reopenEnd; ++reopenItr )
            {
                RCS::File rcsFile( (*reopenItr) );
                rcsFile.Edit();
            }
        }
    }
}
