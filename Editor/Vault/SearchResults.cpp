#include "Precompile.h"

#include "SearchResults.h"

#include "Core/Asset/AssetClass.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
// Ctor/Dtor
SearchResults::SearchResults( u32 vaultSearchID )
: m_VaultSearchID( vaultSearchID )
{
}

SearchResults::SearchResults( const SearchResults* results )
{
    HELIUM_ASSERT( results );
    m_VaultSearchID = results->m_VaultSearchID;
    m_Paths = results->m_Paths;
}

SearchResults::~SearchResults()
{
    Clear();
}

///////////////////////////////////////////////////////////////////////////////
void SearchResults::Clear()
{
    m_Paths.clear();
}

///////////////////////////////////////////////////////////////////////////////
bool SearchResults::HasResults() const
{
    return !m_Paths.empty();
}

///////////////////////////////////////////////////////////////////////////////
const std::map< u64, Helium::Path >& SearchResults::GetPathsMap() const
{
    return m_Paths;
}

///////////////////////////////////////////////////////////////////////////////
const Helium::Path* SearchResults::Find( const u64& hash ) const
{
    std::map< u64, Helium::Path >::const_iterator found = m_Paths.find( hash );
    if ( found != m_Paths.end() )
    {
        return &( found->second );
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
bool SearchResults::AddPath( const Helium::Path& path )
{
    Helium::Insert< std::map< u64, Helium::Path > >::Result inserted = m_Paths.insert( std::map< u64, Helium::Path >::value_type( path.Hash(), path ) );
    return inserted.second;
}

///////////////////////////////////////////////////////////////////////////////
bool SearchResults::RemovePath( const Helium::Path& path )
{
    return m_Paths.erase( path.Hash() ) > 0;
}
