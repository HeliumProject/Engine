#include "Precompile.h"

#include "ProjectPanel.h"
#include "ArtProvider.h"

using namespace Helium;
using namespace Helium::Core;
using namespace Helium::Editor;

ProjectPanel::ProjectPanel( wxWindow *parent )
: ProjectPanelGenerated( parent )
{
    SetHelpText( TXT( "This is the project outliner.  Manage what's included in your project here." ) );

#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

        m_AddFile->SetBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::AddProjectFile ) );
        m_CreateFolder->SetBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::CreateProjectFolder ) );
        m_Delete->SetBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::DeleteProjectItem ) );

        m_LayerManagementPanel->Layout();

        Layout();
        Thaw();
    }
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