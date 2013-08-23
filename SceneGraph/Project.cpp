#include "SceneGraphPch.h"
#include "Project.h"

using namespace Helium;

HELIUM_DEFINE_CLASS( Helium::Project );

void Project::PopulateMetaType( Reflect::MetaStruct& comp )
{
#if REFLECT_REFACTOR
    comp.AddField( &This::a_Path, TXT( "FilePath" ), Reflect::FieldFlags::Discard );
#endif
    comp.AddField( &This::m_Paths, TXT( "m_Paths" ) );
}

Project::Project( const FilePath& path )
{
    a_Path.Set( path );
}

Project::~Project()
{
}

FilePath Project::GetTrackerDB() const
{
    return FilePath( a_Path.Get().Directory() + TXT( ".Helium/" ) + a_Path.Get().Basename() + TXT( ".trackerdb" ) );
}

void Project::AddPath( const FilePath& path )
{
    FilePath relativePath = path.GetRelativePath( a_Path.Get() );
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
    FilePath relativePath = path.GetRelativePath( a_Path.Get() );
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
    HELIUM_ASSERT( !a_Path.Get().empty() && document->GetPath() == a_Path.Get() );

    //for ( std::set< FilePath >::iterator itr = m_Paths.begin(), end = m_Paths.end(); itr != end; ++itr )
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
	bool success = false;
#if REFLECT_REFACTOR
    success = Reflect::ToArchive( a_Path.Get(), const_cast< Project* >( this ) );
#endif
	return success;
}
