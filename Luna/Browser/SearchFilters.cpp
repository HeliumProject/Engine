#include "Precompile.h"
#include "SearchFilters.h"

#include "Common/Container/Insert.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
void SearchFilters::SetupFilters( M_SearchFilters& searchFilters )
{
  for ( u32 index = SearchFilters::Null + 1; index < SearchFilters::Count; ++index )
  {
    searchFilters.insert( M_SearchFilters::value_type( index, S_string() ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
void SearchFilters::ClearFilters( M_SearchFilters& searchFilters, u32 filter )
{
  for ( u32 index = SearchFilters::Null + 1; index < SearchFilters::Count; ++index )
  {
    if ( filter == SearchFilters::Null || filter == index )
    {
      if ( searchFilters.find( index ) != searchFilters.end() )
      {
        searchFilters[index].clear();
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
void SearchFilters::AddFilter( M_SearchFilters& searchFilters, u32 filter, const std::string& value )
{
  NOC_ASSERT( filter > SearchFilters::Null && filter < SearchFilters::Count );
  
  Nocturnal::Insert<M_SearchFilters>::Result inserted = searchFilters.insert( M_SearchFilters::value_type( filter, S_string() ) );
  //if ( searchFilters.find( filter ) != searchFilters.end() )
  {
    inserted.first->second.insert( value );
    //searchFilters[filter].insert( value );
  }
}

///////////////////////////////////////////////////////////////////////////////
void SearchFilters::RemoveFilter( M_SearchFilters& searchFilters, u32 filter, const std::string& value )
{
  NOC_ASSERT( filter > SearchFilters::Null && filter < SearchFilters::Count );
  if ( searchFilters.find( filter ) != searchFilters.end() )
  {
    searchFilters[filter].erase( value );
  }
}

void SearchFilters::AppendAssetDBSearchFilters( const M_SearchFilters& searchFilters, Asset::AssetDBSearch* search )
{
  if ( !search )
    return;

  for ( u32 index = SearchFilters::Null + 1; index < SearchFilters::Count; ++index )
  {
    M_SearchFilters::const_iterator findFilters = searchFilters.find( index );
    if ( findFilters != searchFilters.end() )
    {
      S_string::const_iterator itr = findFilters->second.begin();
      S_string::const_iterator end = findFilters->second.end();
      for ( ; itr != end; ++itr )
      {
        if ( (*itr).empty() )
          continue;

        const std::string& phrase = (*itr);

        Nocturnal::Insert<Asset::M_ColumnQueries>::Result inserted = search->m_ColumnQueries.insert( Asset::M_ColumnQueries::value_type( SearchFilters::Column( index ), S_string() ) );
        inserted.first->second.insert( phrase );
      }
    }
  }
}
