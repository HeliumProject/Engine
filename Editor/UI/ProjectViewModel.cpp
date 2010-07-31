#include "Precompile.h"
#include "ProjectViewModel.h"

using namespace Helium;
using namespace Helium::Core;
using namespace Helium::Editor;

ProjectViewModel::ProjectViewModel()
: m_Project( NULL )
{

}

void ProjectViewModel::SetProject( Project* project )
{
    m_Project = project;
}

unsigned int ProjectViewModel::GetColumnCount() const
{
    return 2;
}

wxString ProjectViewModel::GetColumnType(unsigned int type) const
{
    switch ( type )
    {
    case 0:
        return TXT("Name");

    case 1:
        return TXT("Details");
    }

    return TXT("Unknown");
}

void ProjectViewModel::GetValue(wxVariant& value, const wxDataViewItem& item, unsigned int column) const
{

}

bool ProjectViewModel::SetValue(const wxVariant& value, const wxDataViewItem& item, unsigned int column)
{
    return true;
}

wxDataViewItem ProjectViewModel::GetParent(const wxDataViewItem& item) const
{
    return NULL;
}

unsigned int ProjectViewModel::GetChildren(const wxDataViewItem& item, wxDataViewItemArray& items) const
{
    return 0;
}

bool ProjectViewModel::IsContainer(const wxDataViewItem& item) const
{
    return false;
}