#include "Project.h"

using namespace Helium;

REFLECT_DEFINE_CLASS( Project );

Project::Project( const Path& path )
{
    a_Path.Set( path );
}

Project::~Project()
{
}

void Project::AddPath( const Path& path )
{
    Path relativePath = path.GetRelativePath( a_Path.Get() );
    HELIUM_ASSERT( !relativePath.IsAbsolute() );
    std::pair< std::set< Path >::iterator, bool > result = m_Paths.insert( relativePath );
    if ( result.second )
    {
        e_PathAdded.Raise( relativePath );
        e_HasChanged.Raise( DocumentObjectChangedArgs( true ) );
    }
}

void Project::RemovePath( const Path& path )
{
    Path relativePath = path.GetRelativePath( a_Path.Get() );
    std::set< Path >::iterator itr = m_Paths.find( relativePath );
    if ( itr != m_Paths.end() )
    {
        m_Paths.erase( itr );
        e_PathRemoved.Raise( relativePath );
        e_HasChanged.Raise( DocumentObjectChangedArgs( true ) );
    }
}

void Project::ConnectDocument( Document* document )
{
    document->d_Save.Set( this, &Project::OnDocumentSave );
    document->e_PathChanged.AddMethod( this, &Project::OnDocumentPathChanged );

    e_HasChanged.AddMethod( document, &Document::OnObjectChanged );
}

void Project::DisconnectDocument( const Document* document )
{
    document->d_Save.Clear();
    document->e_PathChanged.AddMethod( this, &Project::OnDocumentPathChanged );

    e_HasChanged.RemoveMethod( document, &Document::OnObjectChanged );
}

void Project::OnDocumentOpened( const DocumentEventArgs& args )
{
    const Document* document = static_cast< const Document* >( args.m_Document );
    HELIUM_ASSERT( document );

    std::set< Path >::iterator findPath = m_Paths.find( document->GetPath() );
    if ( findPath != m_Paths.end() )
    {
        document->e_PathChanged.AddMethod( this, &Project::OnChildDocumentPathChanged );
    }
}

void Project::OnDocumenClosed( const DocumentEventArgs& args )
{
    const Document* document = static_cast< const Document* >( args.m_Document );
    HELIUM_ASSERT( document );

    std::set< Path >::iterator findPath = m_Paths.find( document->GetPath() );
    if ( findPath != m_Paths.end() )
    {
        document->e_PathChanged.RemoveMethod( this, &Project::OnChildDocumentPathChanged );
    }
}

void Project::OnDocumentSave( const DocumentEventArgs& args )
{
    const Document* document = static_cast< const Document* >( args.m_Document );
    HELIUM_ASSERT( document );
    HELIUM_ASSERT( !a_Path.Get().empty() && document->GetPath() == a_Path.Get() );

    //for ( std::set< Path >::iterator itr = m_Paths.begin(), end = m_Paths.end(); itr != end; ++itr )
    //{

    //}

    args.m_Result = Serialize();
}

void Project::OnDocumentPathChanged( const DocumentPathChangedArgs& args )
{
    a_Path.Set( args.m_Document->GetPath() );
    e_HasChanged.Raise( DocumentObjectChangedArgs( true ) );
}

void Project::OnChildDocumentPathChanged( const DocumentPathChangedArgs& args )
{
    const Document* document = static_cast< const Document* >( args.m_Document );
    HELIUM_ASSERT( document );

    m_Paths.erase( args.m_OldPath );

    HELIUM_ASSERT( a_Path.Get().IsAbsolute() );
    m_Paths.insert( document->GetPath().GetRelativePath( a_Path.Get() ) );

    e_HasChanged.Raise( DocumentObjectChangedArgs( true ) );
}

bool Project::Serialize() const 
{
    HELIUM_ASSERT( !a_Path.Get().empty() );
#pragma TODO( "Fix const correctness." )
    return Reflect::ToArchive( a_Path.Get(), const_cast< Project* >( this ) );
}
