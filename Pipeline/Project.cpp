#include "Project.h"

using namespace Helium;

Project::Project( const Path& path )
{
    a_Path.Set( path );
}

Project::~Project()
{
}

bool Project::Save() const 
{
    HELIUM_ASSERT( !a_Path.Get().empty() );
    return Reflect::ToArchive( a_Path.Get(), this );
}
