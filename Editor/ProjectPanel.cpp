#include "Precompile.h"

#include "ProjectPanel.h"
#include "ArtProvider.h"

#include "Core/Asset/AssetClass.h"

using namespace Helium;
using namespace Helium::Core;
using namespace Helium::Editor;

void ProjectViewModel::SetProject( Project* project )
{
    m_Project = project;
    m_Project->ChildAdded().AddMethod( this, &ProjectViewModel::ChildAdded );
    m_Project->ChildRemoved().AddMethod( this, &ProjectViewModel::ChildRemoved );
    m_Project->Initialize();
}

void ProjectViewModel::ChildAdded( const Reflect::DocumentHierarchyChangeArgs& args )
{
    ItemAdded( args.m_NewParent, args.m_Node );
}

void ProjectViewModel::ChildRemoved( const Reflect::DocumentHierarchyChangeArgs& args )
{
    ItemDeleted( args.m_OldParent, args.m_Node );
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
    Reflect::DocumentElement* node = static_cast< Reflect::DocumentElement* >( item.GetID() );

    if ( node )
    {
        ProjectFile* file = Reflect::ObjectCast<ProjectFile>( node );
        if ( file )
        {
            switch ( column )
            {
            case 0:
                value = file->Path().Get().c_str();
                break;

            case 1:
                break;
            }
        }
        else
        {
            ProjectFolder* folder = Reflect::ObjectCast<ProjectFolder>( node );
            if ( folder )
            {
                switch ( column )
                {
                case 0:
                    value = folder->Name().Get();
                    break;

                case 1:
                    break;
                }
            }
            else
            {
                Project* project = Reflect::ObjectCast<Project>( node );
                if ( project )
                {
                    switch ( column )
                    {
                    case 0:
                        value = project->Path().Get().c_str();
                        break;

                    case 1:
                        break;
                    }
                }
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
    Reflect::DocumentElement* node = static_cast< Reflect::DocumentElement* >( item.GetID() );

    if ( node )
    {
        return node->GetParent();
    }

    return NULL;
}

unsigned int ProjectViewModel::GetChildren(const wxDataViewItem& item, wxDataViewItemArray& items) const
{
    Reflect::DocumentElement* node = static_cast< Reflect::DocumentElement* >( item.GetID() );

    if ( node )
    {
        for ( std::vector< Reflect::DocumentNodePtr >::const_iterator itr = node->GetChildren().begin()
            , end = node->GetChildren().end()
            ; itr != end
            ; ++itr )
        {
            items.push_back( itr->Ptr() );
        }

        return items.size();
    }
    
    return 0;
}

bool ProjectViewModel::IsContainer(const wxDataViewItem& item) const
{
    Reflect::DocumentElement* node = static_cast< Reflect::DocumentElement* >( item.GetID() );

    if ( node )
    {
        return node->HasType( Reflect::GetType< ProjectFolder >() )
            || node->HasType( Reflect::GetType< Project >() );
    }

    return false;
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

    m_DropTarget = new FileDropTarget( TXT( "obj" ) );
    m_DropTarget->AddListener( FileDroppedSignature::Delegate( this, &ProjectPanel::OnDroppedFiles ) );
    SetDropTarget( m_DropTarget );

#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

#pragma TODO( "Use overlays for the add/create icons below" )
        m_AddFile->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::File ) );
        m_CreateFolder->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::ProjectFolder ) );
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

void ProjectPanel::OnAddFile( wxCommandEvent& event )
{
    if ( m_Project )
    {
        ProjectFilePtr file = new ProjectFile ();
        file->Path().Set( Helium::Path( TXT("File") ) );
        m_Project->AddChild( file );
    }
}

void ProjectPanel::OnCreateFolder( wxCommandEvent& event )
{
    if ( m_Project )
    {
        ProjectFolderPtr file = new ProjectFolder ();
        file->Name().Set( TXT("Folder") );
        m_Project->AddChild( file );
    }
}

void ProjectPanel::OnDelete( wxCommandEvent& event )
{
    if ( m_Project )
    {
        for ( Helium::OrderedSet< Reflect::DocumentElementPtr >::Iterator itr = m_Selected.Begin(), end = m_Selected.End(); itr != end; ++itr )
        {
            m_Project->RemoveChild( *itr );
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
            ProjectFilePtr file = new ProjectFile();
            asset->SetSerializationPath( asset->GetSerializationPath().GetRelativePath( m_Project->Path().Get() ) );
            file->Path().Set( asset->GetSerializationPath().Get() );
            m_Project->AddChild( file );
        }
    }
}