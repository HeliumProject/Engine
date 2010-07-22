#include "Precompile.h"

#include "ProjectPanel.h"

using namespace Luna;

ProjectPanel::ProjectPanel(wxWindow *parent, Project* project)
: ProjectPanelGenerated( parent )
, m_Model( project )
{
    m_DataViewCtrl->AssociateModel( &m_Model );
}
