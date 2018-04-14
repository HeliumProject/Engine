#pragma once

#include <map>

#include "PerforceGenerated.h"

#include "Application/RCS.h"
#include "Editor/Perforce/Perforce.h"

namespace Helium
{
    namespace Editor
    {
        ///////////////////////////////////////////////////////////////////////////////
        namespace PerforceSubmitActions
        {
            enum PerforceSubmitAction
            {
                Submit,
                Update,
                Cancel
            };
        }
        typedef PerforceSubmitActions::PerforceSubmitAction PerforceSubmitAction;


        ///////////////////////////////////////////////////////////////////////////////
        namespace PerforceSubmitPanelStyles
        {
            enum PerforceSubmitPanelStyle
            {
                Title             = 1 << 0,  // Show the client information      
                ClientDetails     = 1 << 1,  // Show the client information
                JobStatus         = 1 << 2,  // Show the job status selection
                CommitButtons     = 1 << 3,  // Show the commit buttons
            };

            static const uint32_t Default = ( ClientDetails | CommitButtons );
        }
        typedef uint32_t PanelStyle;


        ///////////////////////////////////////////////////////////////////////////////
        class PerforceSubmitPanel : public PerforceSubmitPanelGenerated 
        {
        public:

            //
            // Ctor/Dtor
            //

            PerforceSubmitPanel( wxWindow* parent, 
                int id = wxID_ANY,
                int changelist = RCS::DefaultChangesetId,
                const std::string& description = "",
                const PanelStyle panelStyle = PerforceSubmitPanelStyles::Default,
                const std::string& title = "",
                const std::string& titleDescription = "" );

            virtual ~PerforceSubmitPanel();


            //
            // Get/Set and Display Functions 
            //

            const int GetChangesetId() const { return m_Changeset.m_Id; }
            void SetChangeset( const RCS::Changeset& changeset, bool getFiles = false );

            const std::string& GetChangeDescription() const { return m_Changeset.m_Description; }
            void SetChangeDescription( const std::string& description = "" );

            const std::vector< std::string >& GetFileList() const { return m_FilePaths; }
            void SetFileList( const std::vector< std::string >& filePaths );

            const std::string& GetJobStatus() const { return m_JobStatus; }
            void SetJobStatus( const std::string& jobStatus = "" );

            const PerforceSubmitAction GetAction() const { return m_Action; }
            void SetAction( const PerforceSubmitAction action ) { m_Action = action; }

            bool GetReopenFiles() const { return m_ReopenFiles; }
            void SetReopenFiles( bool reopenFiles = true );

            const std::string& GetTitle() const { return m_Title; }
            void SetTitle( const std::string& title = "" );

            const std::string& GetTitleDescription() const { return m_TitleDescription; }
            void SetTitleDescription( const std::string& titleDescription = "" );


            bool IsFileSelected( const std::string& depotPath );


            void ShowTitle( const std::string& title = "", const std::string& description = "" );
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
            PerforceSubmitAction                m_Action;

            // Stored for UI
            M_FileItemTable       m_FileItemTable;


            //
            // PanelStyle Helpers
            //

            inline bool ShouldShowClientDetails() { return ( ( m_PanelStyle & PerforceSubmitPanelStyles::ClientDetails ) == PerforceSubmitPanelStyles::ClientDetails ); }
            inline bool ShouldShowTitle() { return ( !m_Title.empty() && !m_Changeset.m_Description.empty() && ( m_PanelStyle & PerforceSubmitPanelStyles::Title ) == PerforceSubmitPanelStyles::Title ); }
            inline bool ShouldShowJobStatus() { return ( ( m_PanelStyle & PerforceSubmitPanelStyles::JobStatus ) == PerforceSubmitPanelStyles::JobStatus ); }
            inline bool ShouldShowCommitButtons() { return ( ( m_PanelStyle & PerforceSubmitPanelStyles::CommitButtons ) == PerforceSubmitPanelStyles::CommitButtons ); }

            inline bool HasSubmitAction() { return ( ( m_PanelStyle & PerforceSubmitActions::Submit ) == PerforceSubmitActions::Submit ); }
            inline bool HasUpdateAction() { return ( ( m_PanelStyle & PerforceSubmitActions::Update ) == PerforceSubmitActions::Update ); }
            inline bool HasCancelAction() { return ( ( m_PanelStyle & PerforceSubmitActions::Cancel ) == PerforceSubmitActions::Cancel ); }

        };
    }
}