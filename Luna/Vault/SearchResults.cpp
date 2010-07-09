#include "Precompile.h"

#include "SearchResults.h"

#include "Pipeline/Asset/AssetClass.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Ctor/Dtor
SearchResults::SearchResults( u32 browserSearchID )
: m_VaultSearchID( browserSearchID )
{
}

SearchResults::SearchResults( const SearchResults* results )
{
    NOC_ASSERT( results );
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
const std::map< u64, Nocturnal::Path >& SearchResults::GetPathsMap() const
{
    return m_Paths;
}

///////////////////////////////////////////////////////////////////////////////
const Nocturnal::Path* SearchResults::Find( const u64& hash ) const
{
    std::map< u64, Nocturnal::Path >::const_iterator found = m_Paths.find( hash );
    if ( found != m_Paths.end() )
    {
        return &( found->second );
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
bool SearchResults::AddPath( const Nocturnal::Path& path )
{
    Nocturnal::Insert< std::map< u64, Nocturnal::Path > >::Result inserted = m_Paths.insert( std::map< u64, Nocturnal::Path >::value_type( path.Hash(), path ) );
    return inserted.second;
}

///////////////////////////////////////////////////////////////////////////////
bool SearchResults::RemovePath( const Nocturnal::Path& path )
{
    return m_Paths.erase( path.Hash() ) > 0;
}
