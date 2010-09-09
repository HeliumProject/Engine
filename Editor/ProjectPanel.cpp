#include "Precompile.h"

#include "ProjectPanel.h"
#include "ArtProvider.h"

#include "Core/Asset/AssetClass.h"

#include "Editor/FileDialog.h"

using namespace Helium;
using namespace Helium::Core;
using namespace Helium::Editor;

void ProjectViewModel::SetProject( Project* project )
{
    m_Project = project;
    m_Project->e_PathAdded.AddMethod( this, &ProjectViewModel::PathAdded );
    m_Project->e_PathRemoved.AddMethod( this, &ProjectViewModel::PathRemoved );
}

void ProjectViewModel::PathAdded( const Path& path )
{
#pragma TODO( "Auto-build relevant tree for this path" )
    //    ItemAdded( args.m_NewParent, args.m_Node );
}

void ProjectViewModel::PathRemoved( const Path& path )
{
#pragma TODO( "Remove unnecessary tree nodes given the removal of this path" )
    //ItemDeleted( args.m_OldParent, args.m_Node );
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
    Path* node = static_cast< Path* >( item.GetID() );

    if ( node )
    {
        switch ( column )
        {
        case 0:
            value = node->Get().c_str();
            break;

        case 1:
            break;
        }
    }
    else
    {
        Project* project = static_cast< Project* >( item.GetID() );
        if ( project )
        {
            switch ( column )
            {
            case 0:
                value = project->a_Path.Get().c_str();
                break;

            case 1:
                break;
            }
        }
    }
}

bool ProjectViewModel::SetValue(const wxVariant& value, const wxDataViewItem& item, unsigned int column)
{
    return true;
}

wxDataViewItem ProjectViewModel::GetParent(const wxDataViewItem& item) const
{
#pragma TODO( "Get the parent node of a path" )
    return NULL;
}

unsigned int ProjectViewModel::GetChildren(const wxDataViewItem& item, wxDataViewItemArray& items) const
{
#pragma TODO( "Get the children of a directory node" )
    return 0;
}

bool ProjectViewModel::IsContainer(const wxDataViewItem& item) const
{
    Path* path = static_cast< Path* >( item.GetID() );

    if ( path )
    {
        return false;
    }

    return true;
}

ProjectPanel::ProjectPanel( wxWindow *parent )
: ProjectPanelGenerated( parent )
{
    SetHelpText( TXT( "This is the project outliner.  Manage what's included in your project here." ) );

    wxDataViewTextRenderer *render0 = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );
    wxDataViewColumn *column0 = new wxDataViewColumn( "Name", render0, 0, 100, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE );
    m_DataViewCtrl->AppendColumn( column0 );

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
        m_AddFile->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::File ) );
        m_Delete->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Delete ) );

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
    m_Model = new ProjectViewModel ();
    m_Model->SetProject( project );
    m_DataViewCtrl->AssociateModel( m_Model.get() ); // the ctrl will now hold ownership via reference count
    m_Model->ItemAdded( NULL, m_Project.Ptr() );
}

void ProjectPanel::OnAddPath( wxCommandEvent& event )
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

void ProjectPanel::OnDelete( wxCommandEvent& event )
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