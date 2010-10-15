#include "Precompile.h"

#include "ProjectViewModel.h"

#include "Editor/FileIconsTable.h"
#include "Foundation/String/Units.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
ProjectViewModelNode::ProjectViewModelNode( ProjectViewModelNode* parent, const Helium::Path& path, const bool isContainer )
: m_ParentNode( parent )
, m_Path( path )
, m_IsContainer( isContainer )
{
}

ProjectViewModelNode::~ProjectViewModelNode()
{
    m_ParentNode = NULL;
    m_ChildNodes.clear();
}

ProjectViewModelNode* ProjectViewModelNode::GetParent()
{
    return m_ParentNode;
}

S_ProjectViewModelNodeChildren& ProjectViewModelNode::GetChildren()
{
    return m_ChildNodes;
}

bool ProjectViewModelNode::IsContainer() const
{
#pragma TODO ( "OR the file is a scene file, reflect file with manifest" )
    return ( m_IsContainer || m_ChildNodes.size() > 0 || m_Path.IsDirectory() ) ? true : false;
}

void ProjectViewModelNode::SetPath( const Helium::Path& path )
{
    if ( _tcsicmp( m_Path.c_str(), path.c_str() ) != 0 )
    {
        m_Path = path;
    }
}

const Helium::Path& ProjectViewModelNode::GetPath()
{
    return m_Path;
}

void ProjectViewModelNode::PathChanged( const Attribute< Helium::Path >::ChangeArgs& text )
{
    SetPath( text.m_NewValue );
}

tstring ProjectViewModelNode::GetName() const
{
    if ( m_Path.IsDirectory() )
    {
        return m_Path.Get();
    }
    else
    {
        return m_Path.Basename();
    }
}

tstring ProjectViewModelNode::GetDetails() const
{
    return tstring( TXT( "" ) );
}

tstring ProjectViewModelNode::GetFileSize() const
{
    if ( m_Path.IsDirectory() )
    {
        return tstring( TXT( "" ) );
    }
    else if ( m_Path.IsFile() )
    {
        u64 size = m_Path.Size();
        return BytesToString( size );
    }
    
    return tstring( TXT( "" ) );
}

///////////////////////////////////////////////////////////////////////////////
ProjectViewModel::ProjectViewModel()
: m_Project( NULL )
, m_RootNode( NULL )
{
}

ProjectViewModel::~ProjectViewModel()
{
    SetProject( NULL );
    ResetColumns();
}

wxDataViewColumn* ProjectViewModel::CreateColumn( u32 id )
{
    switch( id )
    {
    default:
        {
            return NULL;
        }
        break;

    case ProjectModelColumns::Name:
        {
            wxDataViewIconTextRenderer *render = new wxDataViewIconTextRenderer();

            wxDataViewColumn *column = new wxDataViewColumn(
                ProjectModelColumns::Label( ProjectModelColumns::Name ),
                render,
                m_ColumnLookupTable.size(),
                ProjectModelColumns::Width( ProjectModelColumns::Name ),
                wxALIGN_LEFT,
                wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE );

            m_ColumnLookupTable.push_back( id );

            return column;
        }
        break;

    case ProjectModelColumns::Details:
        {
            wxDataViewTextRenderer *render = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );

            wxDataViewColumn *column = new wxDataViewColumn(
                ProjectModelColumns::Label( ProjectModelColumns::Details ),
                render,
                m_ColumnLookupTable.size(),
                ProjectModelColumns::Width( ProjectModelColumns::Details ),
                wxALIGN_LEFT,
                wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE );

            m_ColumnLookupTable.push_back( id );

            return column;
        }
        break;

    case ProjectModelColumns::FileSize:
        {
            wxDataViewTextRenderer *render = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );

            wxDataViewColumn *column = new wxDataViewColumn(
                ProjectModelColumns::Label( ProjectModelColumns::FileSize ),
                render,
                m_ColumnLookupTable.size(),
                ProjectModelColumns::Width( ProjectModelColumns::FileSize ),
                wxALIGN_LEFT,
                wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE );

            m_ColumnLookupTable.push_back( id );

            return column;
        }
        break;
    }

    return NULL;
}

void ProjectViewModel::ResetColumns()
{
    m_ColumnLookupTable.clear();
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
        if ( m_RootNode )
        {
            m_Project->a_Path.Changed().RemoveMethod( m_RootNode.Ptr(), &ProjectViewModelNode::PathChanged );
            m_RootNode = NULL;
        }

        m_Project->e_PathAdded.RemoveMethod( this, &ProjectViewModel::OnPathAdded );
        m_Project->e_PathRemoved.RemoveMethod( this, &ProjectViewModel::OnPathRemoved );
        m_Project = NULL;
    }

    // Setup the new project view
    m_Project = project;
    if ( m_Project )
    {
        m_Project->e_PathAdded.AddMethod( this, &ProjectViewModel::OnPathAdded );
        m_Project->e_PathRemoved.AddMethod( this, &ProjectViewModel::OnPathRemoved );

        m_RootNode = new ProjectViewModelNode( NULL, m_Project->a_Path.Get(), true );
        m_Project->a_Path.Changed().AddMethod( m_RootNode.Ptr(), &ProjectViewModelNode::PathChanged );

        //AddChild( wxDataViewItem( (void*) m_RootNode.Ptr() ), Helium::Path( TXT( "Test Child.txt" ) ) );
        //AddChild( wxDataViewItem( (void*) m_RootNode.Ptr() ), Helium::Path( TXT( "C:/Projects/github/nocturnal/Helium/Editor/Icons/" ) ) );
    }
}

bool ProjectViewModel::AddChild( const wxDataViewItem& item, const Helium::Path& path )
{
    ProjectViewModelNode *parentNode = static_cast< ProjectViewModelNode* >( item.GetID() );
    if ( !parentNode )
    {
        parentNode = m_RootNode.Ptr();
    }

    Helium::Insert<S_ProjectViewModelNodeChildren>::Result inserted = parentNode->GetChildren().insert( new ProjectViewModelNode( parentNode, path ) );
#pragma TODO( "Add the file dependency to the parent node" )

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
        parentNode = m_RootNode.Ptr();
    }

    S_ProjectViewModelNodeChildren::const_iterator foundChild = parentNode->GetChildren().end();
    for ( S_ProjectViewModelNodeChildren::const_iterator itr = parentNode->GetChildren().begin(),
        end = parentNode->GetChildren().end(); itr != end; ++itr )
    {
        if ( _tcsicmp( (*itr)->GetPath().Get().c_str(), path.Get().c_str() ) == 0 )
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

#pragma TODO( "Remove the file dependency from teh parent node" )
    //m_Project->RemovePath( childNode->m_Path );

    // this should free the childNode if there are no more references to it
    parentNode->GetChildren().erase( childNode );

    wxDataViewItem parentItem( (void*) parentNode );
    ItemDeleted( parentItem, item );
}

bool ProjectViewModel::IsDropPossible( const wxDataViewItem& item )
{
    ProjectViewModelNode *node = static_cast< ProjectViewModelNode* >( item.GetID() );
    if ( !node
        || node == m_RootNode.Ptr() )
    {
        return true;
    }

    return false;
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
        || ( column >= m_ColumnLookupTable.size() ) )
    {
        return;
    }

    ProjectViewModelNode *node = static_cast< ProjectViewModelNode* >( item.GetID() );
    if ( !node )
    {
        return;
    }

    switch( m_ColumnLookupTable.at( column ) )
    {
    default:
        break;

    case ProjectModelColumns::Name:
        {
            i32 imageID = GlobalFileIconsTable().GetIconIDFromPath( node->GetPath() );
            variant << wxDataViewIconText( node->GetName(), GlobalFileIconsTable().GetSmallImageList()->GetIcon( imageID ) );
        }
        break;

    case ProjectModelColumns::Details:
        {
            variant = node->GetDetails();
        }
        break;

    case ProjectModelColumns::FileSize:
        {
            variant = node->GetFileSize();
        }
        break;
    }
}

bool ProjectViewModel::SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int column )
{
    // nothing should be setting column values yet!
    HELIUM_BREAK();
    return false;


    //if ( !item.IsOk()
    //    || ( column < 0 )
    //    || ( column >= m_ColumnLookupTable.size() ) )
    //{
    //    return false;
    //}

    //ProjectViewModelNode *node = static_cast< ProjectViewModelNode* >( item.GetID() );
    //if ( !node )
    //{
    //    return false;
    //}

    //switch( m_ColumnLookupTable.at( column ) )
    //{
    //default:
    //    {
    //        return false;
    //    }
    //    break;

    //case ProjectModelColumns::Name:
    //    {
    //        //wxDataViewIconText iconText;
    //        //iconText << variant;
    //        //node->m_Name = iconText.GetText();
    //    }
    //    break;

    //case ProjectModelColumns::Details:
    //    {
    //        //node->m_Details = variant.GetString();
    //    }
    //    break;

    //case ProjectModelColumns::Size:
    //    {
    //        //node->m_Details = variant.GetString();
    //    }
    //    break;
    //}

    //return true;
}

wxDataViewItem ProjectViewModel::GetParent( const wxDataViewItem& item ) const
{
    if ( !item.IsOk() )
    {
        return wxDataViewItem( 0 );
    }

    ProjectViewModelNode *childNode = static_cast< ProjectViewModelNode* >( item.GetID() );
    if ( !childNode
        || childNode == m_RootNode.Ptr() 
        || !childNode->GetParent() )
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
