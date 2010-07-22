#pragma once

#include "LunaGenerated.h"

#include "ProjectViewModel.h"

namespace Luna
{
    class ProjectPanel : public ProjectPanelGenerated
    {
    public:
   	    ProjectPanel( wxWindow* parent, Project* project );
        ~ProjectPanel();

    protected:
        ProjectViewModel m_Model;
    };
}