#include "Precompile.h"

#include "ProjectViewModel.h"

#include "Editor/FileIconsTable.h"
#include "Foundation/String/Units.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
ProjectViewModelNode::ProjectViewModelNode( ProjectViewModelNode* parent, const Helium::Path& path, const Document* document, const bool isContainer )
: m_ParentNode( parent )
, m_Path( path )
, m_Document( NULL )
, m_IsContainer( isContainer )
{
    if ( document )
    {
        ConnectDocument( document );
    }
}

ProjectViewModelNode::~ProjectViewModelNode()
{
    m_ParentNode = NULL;

    DisconnectDocument();

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

//void ProjectViewModelNode::PathChanged( const Attribute< Helium::Path >::ChangeArgs& text )
//{
//    SetPath( text.m_NewValue );
//}

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
        uint64_t size = m_Path.Size();
        return BytesToString( size );
    }

    return tstring( TXT( "" ) );
}

const Document* ProjectViewModelNode::GetDocument() const
{
    return m_Document;
}

void ProjectViewModelNode::ConnectDocument( const Document* document)
{
    if ( m_Document && m_Document != document )
    {
        DisconnectDocument();
    }

    m_Document = document;
    if ( m_Document )
    {
        m_Document->e_Saved.AddMethod( this, &ProjectViewModelNode::DocumentSaved );
        m_Document->e_Closed.AddMethod( this, &ProjectViewModelNode::DocumentClosed );
        m_Document->e_PathChanged.AddMethod( this, &ProjectViewModelNode::DocumentPathChanged );
    }
}

void ProjectViewModelNode::DisconnectDocument()
{
    if ( m_Document )
    {
        m_Document->e_Saved.RemoveMethod( this, &ProjectViewModelNode::DocumentSaved );
        m_Document->e_Closed.RemoveMethod( this, &ProjectViewModelNode::DocumentClosed );
        m_Document->e_PathChanged.RemoveMethod( this, &ProjectViewModelNode::DocumentPathChanged );
        m_Document = NULL;
    }
}

void ProjectViewModelNode::DocumentSaved( const DocumentEventArgs& args )
{
#pragma TODO( "Rachel WIP: "__FUNCTION__" - Remove the icon dirty overlay" )
}

void ProjectViewModelNode::DocumentClosed( const DocumentEventArgs& args )
{
    DisconnectDocument();
}

void ProjectViewModelNode::DocumentPathChanged( const DocumentPathChangedArgs& args )
{
    m_Path = args.m_Document->GetPath();
}

///////////////////////////////////////////////////////////////////////////////
ProjectViewModel::ProjectViewModel( DocumentManager* documentManager )
: m_DocumentManager( documentManager )
, m_Project( NULL )
, m_RootNode( NULL )
{
}

ProjectViewModel::~ProjectViewModel()
{
    SetProject( NULL );
    ResetColumns();
}

wxDataViewColumn* ProjectViewModel::CreateColumn( uint32_t id )
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

void ProjectViewModel::SetProject( Project* project, const Document* document )
{
    if ( project == m_Project )
    {
        return;
    }

    // Cleanup the old view
    if ( m_Project )
    {
        // Disconnect to the Project
        m_Project->e_PathAdded.RemoveMethod( this, &ProjectViewModel::OnPathAdded );
        m_Project->e_PathRemoved.RemoveMethod( this, &ProjectViewModel::OnPathRemoved );

        // Remove the Project's Children
#pragma TODO( "Rachel WIP: "__FUNCTION__" - Remove and disconnect the project's children" )
        m_MM_ProjectViewModelNodesByPath.clear();

        // Remove the Node
        if ( m_RootNode )
        {
            if ( m_RootNode->GetDocument() )
            {
                m_RootNode->GetDocument()->d_Save.Clear();
                m_RootNode->GetDocument()->e_Closed.RemoveMethod( this, &ProjectViewModel::OnProjectClosed );
                m_RootNode->GetDocument()->e_PathChanged.RemoveMethod( this, &ProjectViewModel::OnProjectPathChanged );
            }

            //m_Project->a_Path.Changed().RemoveMethod( m_RootNode.Ptr(), &ProjectViewModelNode::PathChanged );
            m_RootNode = NULL;
        }

        m_Project = NULL;
    }

    // Setup the new project view
    m_Project = project;
    if ( m_Project )
    {
        // Create the Node     
        m_RootNode = new ProjectViewModelNode( NULL, m_Project->a_Path.Get(), document, true );
        m_MM_ProjectViewModelNodesByPath.insert( MM_ProjectViewModelNodesByPath::value_type( m_Project->a_Path.Get(), m_RootNode.Ptr() ));

        // Add the Project's Children
        for ( std::set< Path >::const_iterator itr = m_Project->Paths().begin(), end = m_Project->Paths().end();
            itr != end; ++itr )
        {
            AddChildItem( wxDataViewItem( (void*) m_RootNode.Ptr() ), *itr );
        }

        // Connect to the Project
#pragma TODO( "Rachel WIP: "__FUNCTION__" - OnProjectSave and OnProjectPathChanged really ought to be added to MainFrame" )
        m_RootNode->GetDocument()->d_Save.Set( this, &ProjectViewModel::OnProjectSave );
        m_RootNode->GetDocument()->e_Closed.AddMethod( this, &ProjectViewModel::OnProjectClosed );
        m_RootNode->GetDocument()->e_PathChanged.AddMethod( this, &ProjectViewModel::OnProjectPathChanged );

        m_Project->e_PathAdded.AddMethod( this, &ProjectViewModel::OnPathAdded );
        m_Project->e_PathRemoved.AddMethod( this, &ProjectViewModel::OnPathRemoved );
        //m_Project->a_Path.Changed().AddMethod( m_RootNode.Ptr(), &ProjectViewModelNode::PathChanged );
    }
}

bool ProjectViewModel::AddChildItem( const wxDataViewItem& parenItem, const Helium::Path& path )
{
    // Get the parent node
    ProjectViewModelNode *parentNode = static_cast< ProjectViewModelNode* >( parenItem.GetID() );
    if ( !parentNode )
    {
        parentNode = m_RootNode.Ptr();
    }

    // Create the child node
    const Document* document = m_DocumentManager->FindDocument( path );
    Helium::Insert<S_ProjectViewModelNodeChildren>::Result inserted = parentNode->GetChildren().insert( new ProjectViewModelNode( parentNode, path, document ) );
    if ( inserted.second )
    {
        ProjectViewModelNode* childNode = (*inserted.first);

        // See if the document is already open
        //MM_ProjectViewModelNodesByPath::iterator findNode = m_MM_ProjectViewModelNodesByPath.find( path );
        //if ( findNode != m_MM_ProjectViewModelNodesByPath.end()
        //    && findNode->first == path
        //    && findNode->second->GetDocument() ) 
        //{
        //    childNode->ConnectDocument( findNode->second->GetDocument() );
        //}

        // Add the node to the multimap and call ItemAdded
        m_MM_ProjectViewModelNodesByPath.insert( MM_ProjectViewModelNodesByPath::value_type( path, childNode ));
        ItemAdded( (void*)parentNode, (void*)childNode );

        return true;
    }
    return false;
}

bool ProjectViewModel::RemoveChildItem( const wxDataViewItem& parenItem, const Helium::Path& path )
{
    ProjectViewModelNode *parentNode = static_cast< ProjectViewModelNode* >( parenItem.GetID() );
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
        RemoveItem( wxDataViewItem( (void*)(*foundChild) ) );

        return true;
    }
    return false;
}

void ProjectViewModel::RemoveItem( const wxDataViewItem& item )
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

#pragma TODO( "Rachel WIP: "__FUNCTION__" - remove all of childNode's children" )

    // remove it from teh multimap
    for ( MM_ProjectViewModelNodesByPath::iterator lower = m_MM_ProjectViewModelNodesByPath.lower_bound( childNode->GetPath() ),
        upper = m_MM_ProjectViewModelNodesByPath.upper_bound( childNode->GetPath() );
        lower != upper && lower != m_MM_ProjectViewModelNodesByPath.end();
    ++lower )
    {
        if ( lower->second == childNode )
        {
            m_MM_ProjectViewModelNodesByPath.erase( lower );
            break;
        }
    }

    // Remove from the parent's childern
    // this should free the childNode if there are no more references to it
    parentNode->GetChildren().erase( childNode );

    ItemDeleted( wxDataViewItem( (void*) parentNode ), item );
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
        AddChildItem( wxDataViewItem( (void*) m_RootNode.Ptr() ), path );   
    }
}

void ProjectViewModel::OnPathRemoved( const Helium::Path& path )
{
    if ( m_RootNode )
    {
        RemoveChildItem( wxDataViewItem( (void*) m_RootNode.Ptr() ), path );   
    }
}

void ProjectViewModel::OnProjectSave( const DocumentEventArgs& args )
{
    const Document* document = static_cast< const Document* >( args.m_Document );
    HELIUM_ASSERT( document );

    if ( document 
        && m_Project 
        && document->GetPath() == m_Project->a_Path.Get() )
    {
        args.m_Result = m_Project->Save();
    }
}

void ProjectViewModel::OnProjectClosed( const DocumentEventArgs& args )
{
    if ( m_Project )
    {
        SetProject( NULL );
    }
}

void ProjectViewModel::OnProjectPathChanged( const DocumentPathChangedArgs& args )
{
    m_Project->a_Path.Set( args.m_Document->GetPath() );
}

void ProjectViewModel::OnDocumentAdded( const DocumentEventArgs& args )
{
    const Document* document = static_cast< const Document* >( args.m_Document );
    HELIUM_ASSERT( document );

    if ( document )
    {
        for ( MM_ProjectViewModelNodesByPath::iterator lower = m_MM_ProjectViewModelNodesByPath.lower_bound( document->GetPath() ),
            upper = m_MM_ProjectViewModelNodesByPath.upper_bound( document->GetPath() );
            lower != upper && lower != m_MM_ProjectViewModelNodesByPath.end();
        ++lower )
        {
            ProjectViewModelNode *node = lower->second;
            node->ConnectDocument( document );
        }
    }
}

void ProjectViewModel::OnDocumentRemoved( const DocumentEventArgs& args )
{
    const Document* document = static_cast< const Document* >( args.m_Document );
    HELIUM_ASSERT( document );

    if ( document )
    {
        for ( MM_ProjectViewModelNodesByPath::iterator lower = m_MM_ProjectViewModelNodesByPath.lower_bound( document->GetPath() ),
            upper = m_MM_ProjectViewModelNodesByPath.upper_bound( document->GetPath() );
            lower != upper && lower != m_MM_ProjectViewModelNodesByPath.end();
        ++lower )
        {
            ProjectViewModelNode *node = lower->second;
            node->DisconnectDocument();
        }
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
            int32_t imageID = GlobalFileIconsTable().GetIconIDFromPath( node->GetPath() );
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

    uint32_t numAdded = 0;
    for ( S_ProjectViewModelNodeChildren::const_iterator itr = parentNode->GetChildren().begin(),
        end = parentNode->GetChildren().end(); itr != end; ++itr, ++numAdded )
    {
        items.Add( wxDataViewItem( (void*) (*itr) ) );
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
