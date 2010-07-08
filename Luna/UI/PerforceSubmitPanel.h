#pragma once

#include <map>

#include "PerforcePanels.h"

#include "Application/RCS/RCS.h"
#include "Application/RCS/Providers/Perforce/Perforce.h"

namespace PerforceUI
{
  ///////////////////////////////////////////////////////////////////////////////
  namespace Actions
  {
    enum Action
    {
      Submit,
      Update,
      Cancel
    };
  }
  typedef Actions::Action Action;


  ///////////////////////////////////////////////////////////////////////////////
  namespace PanelStyles
  {
    enum PanelStyle
    {
      Title             = 1 << 0,  // Show the client information      
      ClientDetails     = 1 << 1,  // Show the client information
      JobStatus         = 1 << 2,  // Show the job status selection
      CommitButtons     = 1 << 3,  // Show the commit buttons
    };

    static const u64 Default = ( ClientDetails | CommitButtons );
  }
  typedef u64 PanelStyle;


  ///////////////////////////////////////////////////////////////////////////////
  class SubmitPanel : public GeneratedSubmitPanel 
  {
  public:

    //
    // Ctor/Dtor
    //

    SubmitPanel( wxWindow* parent, 
      int id = wxID_ANY,
      int changelist = RCS::DefaultChangesetId,
      const tstring& description = TXT( "" ),
      const PanelStyle panelStyle = PanelStyles::Default,
      const tstring& title = TXT( "" ),
      const tstring& titleDescription = TXT( "" ) );

    virtual ~SubmitPanel();


    //
    // Get/Set and Display Functions 
    //

    const int GetChangesetId() const { return m_Changeset.m_Id; }
    void SetChangeset( const RCS::Changeset& changeset, bool getFiles = false );

    const tstring& GetChangeDescription() const { return m_Changeset.m_Description; }
    void SetChangeDescription( const tstring& description = TXT("") );

    const std::vector< tstring >& GetFileList() const { return m_FilePaths; }
    void SetFileList( const std::vector< tstring >& filePaths );

    const tstring& GetJobStatus() const { return m_JobStatus; }
    void SetJobStatus( const tstring& jobStatus = TXT( "" ) );

    const Action GetAction() const { return m_Action; }
    void SetAction( const Action action ) { m_Action = action; }

    bool GetReopenFiles() const { return m_ReopenFiles; }
    void SetReopenFiles( bool reopenFiles = true );

    const tstring& GetTitle() const { return m_Title; }
    void SetTitle( const tstring& title = TXT( "" ) );

    const tstring& GetTitleDescription() const { return m_TitleDescription; }
    void SetTitleDescription( const tstring& titleDescription = TXT( "" ) );


    bool IsFileSelected( const tstring& depotPath );


    void ShowTitle( const tstring& title = TXT(""), const tstring& description = TXT("") );
    void HideTitle();

    void ShowJobStatus( bool show = true );
    void HideJobStatus() { return ShowJobStatus( false ); } 

    void ShowClientDetails( bool show = true );
    void HideClientDetails() { return ShowClientDetails( false ); }

    void ShowCommitButtons( bool show = true );
    void HideCommitButtons() { return ShowCommitButtons( false ); }


    virtual void ConnectListeners();
    virtual void DisconnectListeners();

    bool TransferDataToForm();
    bool TransferDataFromForm();

    void CommitChanges();

  protected:

    virtual void Clear();
    virtual void Populate();

    //
    // Virtual event handlers, overide them in your derived class
    //

    virtual void OnSelectAllButtonClick( wxCommandEvent& event );
    virtual void OnUnselectAllButtonClick( wxCommandEvent& event );
    virtual void OnSubmitButtonClick( wxCommandEvent& event );
    virtual void OnUpdateButtonClick( wxCommandEvent& event );
    virtual void OnCancelButtonClick( wxCommandEvent& event );
    virtual void OnSpecNotesButtonClick( wxCommandEvent& event );
    virtual void OnHelpButtonClick( wxCommandEvent& event );

  private:
    typedef std::map< tstring, int > M_FileItemTable;

    //
    // Members
    //

    PanelStyle    m_PanelStyle;

    tstring           m_Title;
    tstring           m_TitleDescription;

    RCS::Changeset    m_Changeset;

    tstring           m_DateTime;

    tstring           m_JobStatus;

    std::vector< tstring >              m_FilePaths;

    bool                  m_ReopenFiles;
    Action                m_Action;

    // Stored for UI
    M_FileItemTable       m_FileItemTable;


    //
    // PanelStyle Helpers
    //

    inline bool ShouldShowClientDetails() { return ( ( m_PanelStyle & PanelStyles::ClientDetails ) == PanelStyles::ClientDetails ); }
    inline bool ShouldShowTitle() { return ( !m_Title.empty() && !m_Changeset.m_Description.empty() && ( m_PanelStyle & PanelStyles::Title ) == PanelStyles::Title ); }
    inline bool ShouldShowJobStatus() { return ( ( m_PanelStyle & PanelStyles::JobStatus ) == PanelStyles::JobStatus ); }
    inline bool ShouldShowCommitButtons() { return ( ( m_PanelStyle & PanelStyles::CommitButtons ) == PanelStyles::CommitButtons ); }

    inline bool HasSubmitAction() { return ( ( m_PanelStyle & Actions::Submit ) == Actions::Submit ); }
    inline bool HasUpdateAction() { return ( ( m_PanelStyle & Actions::Update ) == Actions::Update ); }
    inline bool HasCancelAction() { return ( ( m_PanelStyle & Actions::Cancel ) == Actions::Cancel ); }

  };
}