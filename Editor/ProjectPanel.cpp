#include "Precompile.h"

#include "ProjectPanel.h"
#include "ArtProvider.h"

#include "Core/Asset/AssetClass.h"

#include "Editor/FileDialog.h"

using namespace Helium;
using namespace Helium::Editor;

ProjectPanel::ProjectPanel( wxWindow *parent )
: ProjectPanelGenerated( parent )
{
    SetHelpText( TXT( "This is the project outliner.  Manage what's included in your project here." ) );

    //wxDataViewTextRenderer *render0 = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );
    wxDataViewIconTextRenderer *render0 = new wxDataViewIconTextRenderer();
    wxDataViewColumn *column0 = new wxDataViewColumn( "Name", render0, 0, 100, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE );
    m_DataViewCtrl->AppendColumn( column0 );

    //GetStore()->AppendColumn( wxT("wxDataViewIconText") );
    //wxDataViewIconTextRenderer *render = new wxDataViewIconTextRenderer( wxT("wxDataViewIconText"), mode )
    //wxDataViewColumn *ret = new wxDataViewColumn( label, render, GetStore()->GetColumnCount()-1, width, align, flags );
    //wxDataViewCtrl::AppendColumn( ret );

    wxDataViewTextRenderer *render1 = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );
    wxDataViewColumn *column1 = new wxDataViewColumn( "Details", render1, 1, 150, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE );
    m_DataViewCtrl->AppendColumn( column1 );

    std::set< tstring > extension;
    Asset::AssetClass::GetExtensions( extension );

    m_DropTarget = new FileDropTarget( extension );
    m_DropTarget->AddListener( FileDroppedSignature::Delegate( this, &ProjectPanel::OnDroppedFiles ) );
    SetDropTarget( m_DropTarget );

#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

#pragma TODO( "Use overlays for the add/create icons below" )
        m_AddFile->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::FileAdd ) );
        m_DeleteFile->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::FileDelete ) );

        m_ProjectManagementPanel->Layout();

        Layout();
        Thaw();
    }
}

ProjectPanel::~ProjectPanel()
{
}


void ProjectPanel::SetProject( Project* project )
{
    m_Project = project;
    m_Model = new ProjectViewModel();
    m_Model->SetProject( project );

    // the ctrl will now hold ownership via reference count
    m_DataViewCtrl->AssociateModel( m_Model.get() );
}

void ProjectPanel::OnAddFile( wxCommandEvent& event )
{
    if ( m_Project )
    {
        FileDialog openDlg( this, TXT( "Open" ), m_Project->a_Path.Get().Directory().c_str() );

        if ( openDlg.ShowModal() == wxID_OK )
        {
            Path path( (const wxChar*)openDlg.GetPath().c_str() );
            m_Project->AddPath( path );
        }
    }
}

void ProjectPanel::OnDeleteFile( wxCommandEvent& event )
{
    if ( m_Project )
    {
        for ( Helium::OrderedSet< Path* >::Iterator itr = m_Selected.Begin(), end = m_Selected.End(); itr != end; ++itr )
        {
            m_Project->RemovePath( *(*itr) );
        }
    }
}

void ProjectPanel::OnDroppedFiles( const FileDroppedArgs& args )
{
    HELIUM_ASSERT( m_Project );

    Asset::AssetClassPtr asset = Asset::AssetClass::Create( args.m_Path );

    if ( asset.ReferencesObject() )
    {
        if ( asset->GetSerializationPath().Exists() )
        {
            asset->SetSerializationPath( asset->GetSerializationPath().GetRelativePath( m_Project->a_Path.Get() ) );
            m_Project->AddPath( asset->GetSerializationPath() );
        }
    }
}