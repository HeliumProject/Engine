#pragma once

#include "ThumbnailTile.h"
#include "ThumbnailIterator.h"

namespace Luna
{
  // Sorting methods for the shown items
  namespace ThumbnailSortMethods
  {
    enum ThumbnailSortMethod
    {
      AlphabeticalByName,
      AlphabeticalByType,
    };
  }
  typedef ThumbnailSortMethods::ThumbnailSortMethod ThumbnailSortMethod;


  /////////////////////////////////////////////////////////////////////////////
  // Tracks all the different ways that the thumbnail tiles can be sorted so
  // that we can quickly display a sorted list in the view.
  // 
  class ThumbnailSorter
  {
  public:
    ThumbnailSorter();
    virtual ~ThumbnailSorter();

    void SetSortMethod( ThumbnailSortMethod sortMethod );
    ThumbnailSortMethod GetSortMethod() const;

    void Clear();

    void Add( ThumbnailTile* tile );
    void Add( const M_FolderToTilePtr& tiles );
    void Add( const M_FileToTilePtr& tiles );

    ThumbnailIteratorPtr GetIterator( ThumbnailTile* startingTile = NULL ) const;

    i32 Compare( const ThumbnailTile* first, const ThumbnailTile* second ) const;

  private:
    typedef bool( *SortFunction )( const ThumbnailTile*, const ThumbnailTile* );
    static bool SortAlphabeticalByName( const ThumbnailTile* first, const ThumbnailTile* second );
    static bool SortAlphabeticalByType( const ThumbnailTile* first, const ThumbnailTile* second );

  private:
    ThumbnailSortMethod m_SortMethod;

    typedef std::set< ThumbnailTile*, SortFunction > S_AlphaByName;
    S_AlphaByName m_AlphaByName;

    typedef std::set< ThumbnailTile*, SortFunction > S_AlphaByType;
    S_AlphaByType m_AlphaByType;
  };
}
