#include "Precompile.h"

#include "ProjectViewModel.h"

#include "Editor/FileIconsTable.h"

using namespace Helium;
using namespace Helium::Editor;


///////////////////////////////////////////////////////////////////////////////
ProjectViewModel::ProjectViewModel()
: m_Project( NULL )
, m_RootNode( NULL )
{
}

ProjectViewModel::~ProjectViewModel()
{
    //SetProject( NULL );
}

void ProjectViewModel::SetProject( Project* project )
{
    if ( project == m_Project )
    {
        return;
    }

    // Cleanup the old view
    if ( m_Project )
    {
        m_Project->e_PathAdded.RemoveMethod( this, &ProjectViewModel::OnPathAdded );
        m_Project->e_PathRemoved.RemoveMethod( this, &ProjectViewModel::OnPathRemoved );
        m_Project = NULL;
    }

    if ( m_RootNode )
    {
        m_Project->a_Path.Changed().RemoveMethod( m_RootNode.Ptr(), &ProjectViewModelNode::PathChanged );
        m_RootNode = NULL;
    }

    // Setup the new project view
    m_Project = project;
    if ( m_Project )
    {
        m_Project->e_PathAdded.AddMethod( this, &ProjectViewModel::OnPathAdded );
        m_Project->e_PathRemoved.AddMethod( this, &ProjectViewModel::OnPathRemoved );

        m_RootNode = new ProjectViewModelNode( NULL, m_Project->a_Path.Get() );
        m_RootNode->m_IsContainer = true;
        m_Project->a_Path.Changed().AddMethod( m_RootNode.Ptr(), &ProjectViewModelNode::PathChanged );

        AddChild( wxDataViewItem( (void*) m_RootNode.Ptr() ), Helium::Path( TXT( "Test Child.txt" ) ) );
    }
}

bool ProjectViewModel::AddChild( const wxDataViewItem& item, const Helium::Path& path )
{
    ProjectViewModelNode *parentNode = static_cast< ProjectViewModelNode* >( item.GetID() );
    if ( !parentNode )
    {
        return false;
    }

    Helium::Insert<S_ProjectViewModelNodeChildren>::Result inserted = parentNode->GetChildren().insert( new ProjectViewModelNode( parentNode, path ) );

    if ( inserted.second )
    {
        ItemAdded( (void*)parentNode, (void*)(*inserted.first) );
        return true;
    }
    return false;
}

bool ProjectViewModel::RemoveChild( const wxDataViewItem& item, const Helium::Path& path )
{
    ProjectViewModelNode *parentNode = static_cast< ProjectViewModelNode* >( item.GetID() );
    if ( !parentNode )
    {
        return false;
    }

    S_ProjectViewModelNodeChildren::const_iterator foundChild = parentNode->GetChildren().end();
    for ( S_ProjectViewModelNodeChildren::const_iterator itr = parentNode->GetChildren().begin(),
        end = parentNode->GetChildren().end(); itr != end; ++itr )

    {
        if ( _tcsicmp( (*itr)->m_Path.Get().c_str(), path.Get().c_str() ) == 0 )
        {
            foundChild = itr;
            break;
        }
    }

    if ( foundChild != parentNode->GetChildren().end() )
    {
        wxDataViewItem childItem( (*foundChild).Ptr() );
        Delete( childItem );
        return true;
    }
    return false;
}

void ProjectViewModel::Delete( const wxDataViewItem& item )
{
    ProjectViewModelNode *childNode = static_cast< ProjectViewModelNode* >( item.GetID() );
    if ( !childNode )
    {
        return;
    }

    ProjectViewModelNode *parentNode = childNode->GetParent();
    if ( !parentNode )
    {
        // they are trying to delete the m_RootNode
        return;
    }

    // this should free the childNode if there are no more references to it
    parentNode->GetChildren().erase( childNode );

    wxDataViewItem parentItem( (void*) parentNode );
    ItemDeleted( parentItem, item );
}

void ProjectViewModel::OnPathAdded( const Helium::Path& path )
{
    if ( m_RootNode )
    {
        AddChild( wxDataViewItem( (void*) m_RootNode.Ptr() ), path );   
    }
}

void ProjectViewModel::OnPathRemoved( const Helium::Path& path )
{
    if ( m_RootNode )
    {
        RemoveChild( wxDataViewItem( (void*) m_RootNode.Ptr() ), path );   
    }
}

unsigned int ProjectViewModel::GetColumnCount() const
{
    return ProjectModelColumns::COUNT;
}

wxString ProjectViewModel::GetColumnType( unsigned int type ) const
{
    if ( ( type >= 0 )
        && ( type < ProjectModelColumns::COUNT ) )
    {
        return ProjectModelColumns::Label( type );
    }
    else
    {
        return ProjectModelColumns::s_Labels[ProjectModelColumns::COUNT];
    }
}

void ProjectViewModel::GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int column ) const
{
    if ( !item.IsOk()
        || ( column < 0 )
        || ( column >= ProjectModelColumns::COUNT ) )
    {
        return;
    }

    ProjectViewModelNode *node = static_cast< ProjectViewModelNode* >( item.GetID() );
    if ( !node )
    {
        return;
    }
    
    switch( column )
    {
    default:
        break;

    case ProjectModelColumns::Name:
        //m_DataViewCtrl->SetImageList( GlobalFileIconsTable().GetSmallImageList() );
        //variant = node->m_Name;
        i32 imageID = GlobalFileIconsTable().GetIconIDFromPath( node->m_Path );
        variant << wxDataViewIconText( node->m_Name, GlobalFileIconsTable().GetSmallImageList()->GetIcon( imageID ) );
        break;

    //case ProjectModelColumns::Details:
    //    variant = node->m_Details;
    //    break;
    }
}

bool ProjectViewModel::SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int column )
{
    if ( !item.IsOk()
        || ( column < 0 )
        || ( column >= ProjectModelColumns::COUNT ) )
    {
        return false;
    }

    ProjectViewModelNode *node = static_cast< ProjectViewModelNode* >( item.GetID() );
    if ( !node )
    {
        return false;
    }
    
    switch( column )
    {
    default:
        return false;
        break;

    case ProjectModelColumns::Name:
        node->m_Name = variant.GetString();
        break;

    //case ProjectModelColumns::Details:
    //    node->m_Details = variant.GetString();
    //    break;
    }

    return true;
}

wxDataViewItem ProjectViewModel::GetParent( const wxDataViewItem& item ) const
{
    if ( !item.IsOk() )
    {
        return wxDataViewItem( 0 );
    }

    ProjectViewModelNode *childNode = static_cast< ProjectViewModelNode* >( item.GetID() );
    if ( !childNode
        || childNode == m_RootNode 
        || !childNode->m_ParentNode )
    {
        return wxDataViewItem( 0 );
    }

    return wxDataViewItem( (void*) childNode->GetParent() );
}

unsigned int ProjectViewModel::GetChildren( const wxDataViewItem& item, wxDataViewItemArray& items ) const
{
    ProjectViewModelNode *parentNode = static_cast< ProjectViewModelNode* >( item.GetID() );
    if ( !parentNode )
    {
        items.Add( wxDataViewItem( (void*) m_RootNode.Ptr() ) );
        return 1;
    }

    if ( parentNode->GetChildren().size() < 1 )
    {
        return 0;
    }

    u32 numAdded = 0;
    for ( S_ProjectViewModelNodeChildren::const_iterator itr = parentNode->GetChildren().begin(),
        end = parentNode->GetChildren().end(); itr != end; ++itr, ++numAdded )

    {
        items.Add( wxDataViewItem( (void*) (*itr).Ptr() ) );
    }

    return numAdded;
}

bool ProjectViewModel::IsContainer( const wxDataViewItem& item ) const
{
    // root node can have children
    if ( !item.IsOk() )
    {
        return true;
    }

    ProjectViewModelNode *node = static_cast< ProjectViewModelNode* >( item.GetID() );
    return node ? node->IsContainer() : false;
}
