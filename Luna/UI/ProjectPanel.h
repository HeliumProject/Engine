#pragma once

#include "LunaGenerated.h"

#include "ProjectViewModel.h"

namespace Luna
{
    class ProjectPanel : public ProjectPanelGenerated
    {
    public:
   	    ProjectPanel( wxWindow* parent, Project* project );

    protected:
        ProjectViewModel m_Model;
    };
}