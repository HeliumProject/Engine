#pragma once

#include <wx/dataview.h>

#include "Project.h"

namespace Luna
{
    class ProjectViewModel : public wxDataViewModel
    {
    public:
        ProjectViewModel();
        void SetProject( Project* project );

        unsigned int GetColumnCount() const NOC_OVERRIDE;
        wxString GetColumnType(unsigned int) const NOC_OVERRIDE;

        void GetValue(wxVariant &,const wxDataViewItem &,unsigned int) const NOC_OVERRIDE;
        bool SetValue(const wxVariant &,const wxDataViewItem &,unsigned int) NOC_OVERRIDE;

        wxDataViewItem GetParent(const wxDataViewItem &) const NOC_OVERRIDE;
        unsigned int GetChildren(const wxDataViewItem &,wxDataViewItemArray &) const NOC_OVERRIDE;

        bool IsContainer(const wxDataViewItem &) const NOC_OVERRIDE;

    protected:
        Project* m_Project;
    };
}