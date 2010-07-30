#include "Precompile.h"

#include "ProjectPanel.h"

using namespace Helium;
using namespace Helium::Editor;

ProjectPanel::ProjectPanel( wxWindow *parent )
: ProjectPanelGenerated( parent )
{
}

ProjectPanel::~ProjectPanel()
{
#pragma TODO("If we don't do this here we crash in a base class.... wtf -Geoff")
    m_DataViewCtrl->Destroy();
    m_DataViewCtrl = NULL;
}

void ProjectPanel::SetProject( Project* project )
{
    m_Model.SetProject( project );
    m_DataViewCtrl->AssociateModel( &m_Model );
}