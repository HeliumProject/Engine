#pragma once

#include "EditorGenerated.h"

#include "ProjectViewModel.h"

namespace Editor
{
    class ProjectPanel : public ProjectPanelGenerated
    {
    public:
   	    ProjectPanel( wxWindow* parent );
        ~ProjectPanel();

        void SetProject( Project* project );

    protected:
        ProjectViewModel m_Model;
    };
}