#include "Precompile.h"

#include "VaultSearchResults.h"

#include "Core/Asset/AssetClass.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
// Ctor/Dtor
VaultSearchResults::VaultSearchResults( u32 vaultSearchID )
: m_VaultSearchID( vaultSearchID )
{
}

VaultSearchResults::VaultSearchResults( const VaultSearchResults* results )
{
    HELIUM_ASSERT( results );
    m_VaultSearchID = results->m_VaultSearchID;
    m_Paths = results->m_Paths;
}

VaultSearchResults::~VaultSearchResults()
{
    Clear();
}

///////////////////////////////////////////////////////////////////////////////
void VaultSearchResults::Clear()
{
    m_Paths.clear();
}

///////////////////////////////////////////////////////////////////////////////
bool VaultSearchResults::HasResults() const
{
    return !m_Paths.empty();
}

///////////////////////////////////////////////////////////////////////////////
const std::map< u64, Helium::Path >& VaultSearchResults::GetPathsMap() const
{
    return m_Paths;
}

///////////////////////////////////////////////////////////////////////////////
const Helium::Path* VaultSearchResults::Find( const u64& hash ) const
{
    std::map< u64, Helium::Path >::const_iterator found = m_Paths.find( hash );
    if ( found != m_Paths.end() )
    {
        return &( found->second );
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
bool VaultSearchResults::AddPath( const Helium::Path& path )
{
    Helium::Insert< std::map< u64, Helium::Path > >::Result inserted = m_Paths.insert( std::map< u64, Helium::Path >::value_type( path.Hash(), path ) );
    return inserted.second;
}

///////////////////////////////////////////////////////////////////////////////
bool VaultSearchResults::RemovePath( const Helium::Path& path )
{
    return m_Paths.erase( path.Hash() ) > 0;
}
