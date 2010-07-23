#pragma once

#include "LunaGenerated.h"

#include "ProjectViewModel.h"

namespace Luna
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