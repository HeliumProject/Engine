#include "SceneGraphPch.h"
#include "Project.h"

using namespace Helium;

HELIUM_DEFINE_CLASS( Helium::Project );

void Project::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &This::m_Path, TXT( "FilePath" ), Reflect::FieldFlags::Discard );
    comp.AddField( &This::m_Paths, TXT( "m_Paths" ) );
}

Project::Project( const FilePath& path )
{
    m_Path.Set( path );
}

const FilePath& Project::GetPath()
{
	return m_Path;
}

void Project::SetPath( const FilePath& path )
{
	m_Path = path;
}

const std::set< FilePath >& Project::GetPaths()
{
	return m_Paths;
}

void Project::AddPath( const FilePath& path )
{
    FilePath relativePath = path.GetRelativePath( m_Path );
    HELIUM_ASSERT( !relativePath.IsAbsolute() );
    std::pair< std::set< FilePath >::iterator, bool > result = m_Paths.insert( relativePath );
    if ( result.second )
    {
        e_PathAdded.Raise( relativePath );
        e_HasChanged.Raise( DocumentObjectChangedArgs( true ) );
    }
}

void Project::RemovePath( const FilePath& path )
{
    FilePath relativePath = path.GetRelativePath( m_Path );
    std::set< FilePath >::iterator itr = m_Paths.find( relativePath );
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

    std::set< FilePath >::iterator findPath = m_Paths.find( document->GetPath() );
    if ( findPath != m_Paths.end() )
    {
        document->e_PathChanged.AddMethod( this, &Project::OnChildDocumentPathChanged );
    }
}

void Project::OnDocumenClosed( const DocumentEventArgs& args )
{
    const Document* document = static_cast< const Document* >( args.m_Document );
    HELIUM_ASSERT( document );

    std::set< FilePath >::iterator findPath = m_Paths.find( document->GetPath() );
    if ( findPath != m_Paths.end() )
    {
        document->e_PathChanged.RemoveMethod( this, &Project::OnChildDocumentPathChanged );
    }
}

void Project::OnDocumentSave( const DocumentEventArgs& args )
{
    const Document* document = static_cast< const Document* >( args.m_Document );
    HELIUM_ASSERT( document );
    HELIUM_ASSERT( !m_Path.empty() && document->GetPath() == m_Path );

    //for ( std::set< FilePath >::iterator itr = m_Paths.begin(), end = m_Paths.end(); itr != end; ++itr )
    //{

    //}

    args.m_Result = Serialize();
}

void Project::OnDocumentPathChanged( const DocumentPathChangedArgs& args )
{
    m_Path = args.m_Document->GetPath();
    e_HasChanged.Raise( DocumentObjectChangedArgs( true ) );
}

void Project::OnChildDocumentPathChanged( const DocumentPathChangedArgs& args )
{
    const Document* document = static_cast< const Document* >( args.m_Document );
    HELIUM_ASSERT( document );

    m_Paths.erase( args.m_OldPath );

    HELIUM_ASSERT( m_Path.IsAbsolute() );
    m_Paths.insert( document->GetPath().GetRelativePath( m_Path ) );

    e_HasChanged.Raise( DocumentObjectChangedArgs( true ) );
}

bool Project::Serialize() const 
{
    HELIUM_ASSERT( !m_Path.empty() );
#pragma TODO( "Fix const correctness." )
	bool success = false;
#if REFLECT_REFACTOR
    success = Reflect::ToArchive( m_Path, const_cast< Project* >( this ) );
#endif
	return success;
}
