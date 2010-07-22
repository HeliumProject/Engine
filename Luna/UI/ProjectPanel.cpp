#include "Precompile.h"

#include "ProjectPanel.h"

using namespace Luna;

ProjectPanel::ProjectPanel(wxWindow *parent, Project* project)
: ProjectPanelGenerated( parent )
, m_Model( project )
{
    m_DataViewCtrl->AssociateModel( &m_Model );
}

ProjectPanel::~ProjectPanel()
{
#pragma TODO("If we don't do this here we crash in a base class.... wtf -Geoff")
    m_DataViewCtrl->Destroy();
    m_DataViewCtrl = NULL;
}
