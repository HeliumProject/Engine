#pragma once

#include "ProjectViewModel.h"

#include "Foundation/Container/OrderedSet.h"
#include "Foundation/DocumentManager.h"

#include "SceneGraph/Project.h"

#include "Editor/EditorGenerated.h"
#include "Editor/DragDrop/FileDropTarget.h"

namespace Helium
{
    namespace Editor
    {
        class ProjectPanel : public ProjectPanelGenerated
        {
        public:
            ProjectPanel( wxWindow* parent, DocumentManager* documentManager );
            virtual ~ProjectPanel();

            void OpenProject( Project* project, const Document* document = NULL );
            void CloseProject();

            void SetActive( const Path& path, bool active );

        protected:
            void GeneralSettingsChanged( const Reflect::ObjectChangeArgs& args );

            void PopulateOpenProjectListItems();
            void OnRecentProjectButtonClick( wxCommandEvent& event );
            virtual void OnOpenProjectButtonClick( wxCommandEvent& event );
            virtual void OnNewProjectButtonClick( wxCommandEvent& event );

            // UI event handlers
            virtual void OnContextMenu( wxContextMenuEvent& event );
            
            virtual void OnActivateItem( wxDataViewEvent& event );

            virtual void OnUpdateUI( wxUpdateUIEvent& event );

            void ProjectPanel::OnAddItems( wxCommandEvent& event );
            void ProjectPanel::OnDeleteItems( wxCommandEvent& event );

			//virtual void OnAddFile( wxCommandEvent& event ) HELIUM_OVERRIDE;
			//virtual void OnDeleteFile( wxCommandEvent& event ) HELIUM_OVERRIDE;

            void OnOptionsMenuOpen( wxMenuEvent& event );
            void OnOptionsMenuClose( wxMenuEvent& event );
            void OnOptionsMenuSelect( wxCommandEvent& event );

            void OnSelectionChanged( wxDataViewEvent& event );

            void OnDragOver( FileDroppedArgs& args );
            virtual void OnDroppedFiles( const FileDroppedArgs& args );

        protected:
            DocumentManager* m_DocumentManager;
            Project* m_Project;
            wxObjectDataPtr< ProjectViewModel > m_Model;
            wxMenu* m_OptionsMenu;
            wxMenu m_ContextMenu;

            OrderedSet< Path* > m_Selected;

            typedef std::map< wxWindowID, Path > M_ProjectMRULookup;
            M_ProjectMRULookup m_ProjectMRULookup;
            
            FileDropTarget* m_DropTarget;
        };
    }
}