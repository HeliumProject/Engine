#pragma once

#include <map>

#include "Application/UI/PerforceUI/Panels.h"

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
  class Panel : public MainPanel 
  {
  public:

    //
    // Ctor/Dtor
    //

    Panel( wxWindow* parent, 
      int id = wxID_ANY,
      int changelist = RCS::DefaultChangesetId,
      const std::string& description = std::string( "" ),
      const PanelStyle panelStyle = PanelStyles::Default,
      const std::string& title = std::string( "" ),
      const std::string& titleDescription = std::string( "" ) );

    virtual ~Panel();


    //
    // Get/Set and Display Functions 
    //

    const int GetChangesetId() const { return m_Changeset.m_Id; }
    void SetChangeset( const RCS::Changeset& changeset, bool getFiles = false );

    const std::string& GetChangeDescription() const { return m_Changeset.m_Description; }
    void SetChangeDescription( const std::string& description = std::string("") );

    const std::vector< std::string >& GetFileList() const { return m_FilePaths; }
    void SetFileList( const std::vector< std::string >& filePaths );

    const std::string& GetJobStatus() const { return m_JobStatus; }
    void SetJobStatus( const std::string& jobStatus = std::string("") );

    const Action GetAction() const { return m_Action; }
    void SetAction( const Action action ) { m_Action = action; }

    bool GetReopenFiles() const { return m_ReopenFiles; }
    void SetReopenFiles( bool reopenFiles = true );

    const std::string& GetTitle() const { return m_Title; }
    void SetTitle( const std::string& title = std::string("") );

    const std::string& GetTitleDescription() const { return m_TitleDescription; }
    void SetTitleDescription( const std::string& titleDescription = std::string("") );


    bool IsFileSelected( const std::string& depotPath );


    void ShowTitle( const std::string& title = std::string(""), const std::string& description = std::string("") );
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
    typedef std::map< std::string, int > M_FileItemTable;

    //
    // Members
    //

    PanelStyle    m_PanelStyle;

    std::string           m_Title;
    std::string           m_TitleDescription;

    RCS::Changeset    m_Changeset;

    std::string           m_DateTime;

    std::string           m_JobStatus;

    std::vector< std::string >              m_FilePaths;

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