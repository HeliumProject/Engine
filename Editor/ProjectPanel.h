#pragma once

#include "ProjectViewModel.h"

#include "Foundation/Container/OrderedSet.h"
#include "Core/Project.h"

#include "Editor/EditorGenerated.h"
#include "Editor/FileDropTarget.h"

namespace Helium
{
    namespace Editor
    {
        class ProjectPanel : public ProjectPanelGenerated
        {
        public:
            ProjectPanel( wxWindow* parent );
            virtual ~ProjectPanel();

            void SetProject( Project* project );

        protected:
            // UI event handlers
			virtual void OnAddFile( wxCommandEvent& event ) HELIUM_OVERRIDE;
			virtual void OnDeleteFile( wxCommandEvent& event ) HELIUM_OVERRIDE;

            void OnOptionsMenuOpen( wxMenuEvent& event );
            void OnOptionsMenuClose( wxMenuEvent& event );
            void OnOptionsMenuSelect( wxCommandEvent& event );

            void OnSelectionChanged( wxDataViewEvent& event );

            void OnDragOver( FileDroppedArgs& args );
            virtual void OnDroppedFiles( const FileDroppedArgs& args );

        protected:
            ProjectPtr m_Project;
            wxObjectDataPtr< ProjectViewModel > m_Model;
            wxMenu* m_OptionsMenu;

            OrderedSet< Path* > m_Selected;
            
            FileDropTarget* m_DropTarget;
        };
    }
}