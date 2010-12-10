#include "Precompile.h"

#include "VaultSearchResults.h"

#include "Pipeline/Asset/AssetClass.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
// Ctor/Dtor
VaultSearchResults::VaultSearchResults( uint32_t vaultSearchID )
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
const std::map< uint64_t, Helium::Path >& VaultSearchResults::GetPathsMap() const
{
    return m_Paths;
}

///////////////////////////////////////////////////////////////////////////////
const Helium::Path* VaultSearchResults::Find( const uint64_t& hash ) const
{
    std::map< uint64_t, Helium::Path >::const_iterator found = m_Paths.find( hash );
    if ( found != m_Paths.end() )
    {
        return &( found->second );
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
bool VaultSearchResults::AddPath( const Helium::Path& path )
{
    Helium::StdInsert< std::map< uint64_t, Helium::Path > >::Result inserted = m_Paths.insert( std::map< uint64_t, Helium::Path >::value_type( path.Hash(), path ) );
    return inserted.second;
}

///////////////////////////////////////////////////////////////////////////////
bool VaultSearchResults::RemovePath( const Helium::Path& path )
{
    return m_Paths.erase( path.Hash() ) > 0;
}
