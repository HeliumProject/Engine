#include "Project.h"

using namespace Helium;

Project::Project( const Path& path )
{
    a_Path.Set( path );
}

Project::~Project()
{
}

void Project::OnDocumentSave( const DocumentEventArgs& args )
{
    const Document* document = static_cast< const Document* >( args.m_Document );
    HELIUM_ASSERT( document );
    HELIUM_ASSERT( !a_Path.Get().empty() && document->GetPath() == a_Path.Get() )

    args.m_Result = Serialize();
}

bool Project::Serialize() const 
{
    HELIUM_ASSERT( !a_Path.Get().empty() );
    return Reflect::ToArchive( a_Path.Get(), this );
}
