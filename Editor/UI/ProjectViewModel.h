#pragma once

#include <wx/dataview.h>

#include "Project.h"

namespace Editor
{
    class ProjectViewModel : public wxDataViewModel
    {
    public:
        ProjectViewModel();
        void SetProject( Project* project );

        unsigned int GetColumnCount() const HELIUM_OVERRIDE;
        wxString GetColumnType(unsigned int) const HELIUM_OVERRIDE;

        void GetValue(wxVariant &,const wxDataViewItem &,unsigned int) const HELIUM_OVERRIDE;
        bool SetValue(const wxVariant &,const wxDataViewItem &,unsigned int) HELIUM_OVERRIDE;

        wxDataViewItem GetParent(const wxDataViewItem &) const HELIUM_OVERRIDE;
        unsigned int GetChildren(const wxDataViewItem &,wxDataViewItemArray &) const HELIUM_OVERRIDE;

        bool IsContainer(const wxDataViewItem &) const HELIUM_OVERRIDE;

    protected:
        Project* m_Project;
    };
}