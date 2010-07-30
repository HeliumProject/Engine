#include "Precompile.h"
#include "ThumbnailSorter.h"
#include "Foundation/File/Path.h"
#include "Foundation/String/Natural.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ThumbnailSorter::ThumbnailSorter()
: m_SortMethod( ThumbnailSortMethods::AlphabeticalByName )
, m_AlphaByName( SortAlphabeticalByName )
, m_AlphaByType( SortAlphabeticalByType )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor.
// 
ThumbnailSorter::~ThumbnailSorter()
{
}

///////////////////////////////////////////////////////////////////////////////
// Sets the current sorting method.
// 
void ThumbnailSorter::SetSortMethod( ThumbnailSortMethod sortMethod )
{
  m_SortMethod = sortMethod;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the current sorting method.
// 
ThumbnailSortMethod ThumbnailSorter::GetSortMethod() const
{
  return m_SortMethod;
}

///////////////////////////////////////////////////////////////////////////////
// Clears the sorted lists.
// 
void ThumbnailSorter::Clear()
{
  m_AlphaByName.clear();
  m_AlphaByType.clear();
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified tile to the sorted lists.
// 
void ThumbnailSorter::Add( ThumbnailTile* tile )
{
  m_AlphaByName.insert( tile );
  m_AlphaByType.insert( tile );
}

void ThumbnailSorter::Add( const M_PathToTilePtr& tiles )
{
  for ( M_PathToTilePtr::const_iterator tileItr = tiles.begin(), tileEnd = tiles.end();
    tileItr != tileEnd; ++tileItr )
  {
    Add( tileItr->second );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns an iterator over the sorted list of tiles, starting at the specified
// tile.
// 
ThumbnailIteratorPtr ThumbnailSorter::GetIterator( ThumbnailTile* startingTile ) const
{
  ThumbnailIteratorPtr iterator;
  switch ( m_SortMethod )
  {
  case ThumbnailSortMethods::AlphabeticalByName:
  default:
    iterator = new ThumbnailSetIterator< S_AlphaByName >( &m_AlphaByName, startingTile );
    break;

  case ThumbnailSortMethods::AlphabeticalByType:
    iterator = new ThumbnailSetIterator< S_AlphaByType >( &m_AlphaByType, startingTile );
    break;
  }

  if ( iterator.ReferencesObject() && startingTile == NULL )
  {
    iterator->Begin();
  }

  return iterator;
}

///////////////////////////////////////////////////////////////////////////////
// Compares the thumbnail tiles using the current sort method.
// Returns -1 if first comes before second.
// Returns +1 if first comes after second.
// Returns 0 if the tiles are equivalent.
// 
i32 ThumbnailSorter::Compare( const ThumbnailTile* first, const ThumbnailTile* second ) const
{
  i32 result = 0;

  bool firstBeforeSecond = false;
  bool secondBeforeFirst = false;

  switch ( m_SortMethod )
  {
  case ThumbnailSortMethods::AlphabeticalByName:
  default:
    firstBeforeSecond = m_AlphaByName.value_comp()( first, second );
    secondBeforeFirst = m_AlphaByName.value_comp()( second, first );
    break;

  case ThumbnailSortMethods::AlphabeticalByType:
    firstBeforeSecond = m_AlphaByType.value_comp()( first, second );
    secondBeforeFirst = m_AlphaByType.value_comp()( second, first );
    break;
  }

  if ( firstBeforeSecond && !secondBeforeFirst )
  {
    result = -1;
  }
  else if ( secondBeforeFirst && !firstBeforeSecond )
  {
    result = 1;
  }

  return result;
}


///////////////////////////////////////////////////////////////////////////////
// Helper function to compare two tiles.  Returns true if first comes before second.
// Folders come before files, labels are compared, then full paths are compared
// if labels are the same.
// 
bool ThumbnailSorter::SortAlphabeticalByName( const ThumbnailTile* first, const ThumbnailTile* second )
{
  // Irreflexivity
  if ( first == second )
  {
    return false;
  }

  // 1. Folders come before files
  if ( first->GetPath().IsDirectory() && second->GetPath().IsFile() )
  {
    return true;
  }
  else if ( first->GetPath().IsFile() && second->GetPath().IsDirectory() )
  {
    return false;
  }
  // else: both are the same type

  // 2. Compare only the tile labels
  i32 result = strinatcmp( first->GetLabel().c_str(), second->GetLabel().c_str() );
  if ( result == 0 )
  {
    // 3. Labels are the same, look at the full path
      return first->GetPath() < second->GetPath();
  }
  return result < 0;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if first should come before second.  Uses the "type label" of 
// the tile and if those match, defaults to sorting alphabetically by name.
// 
bool ThumbnailSorter::SortAlphabeticalByType( const ThumbnailTile* first, const ThumbnailTile* second )
{
  i32 result = _tcsicmp( first->GetTypeLabel().c_str(), second->GetTypeLabel().c_str() );
  if ( result == 0 )
  {
    return SortAlphabeticalByName( first, second );
  }
  return result < 0;
}
