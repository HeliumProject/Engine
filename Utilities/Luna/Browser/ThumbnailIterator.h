#pragma once

#include "Common/Compiler.h"
#include "Common/Memory/SmartPtr.h"

namespace Luna
{
  class ThumbnailTile;

  /////////////////////////////////////////////////////////////////////////////
  // Provides an interface for iterating over an arbitrary list of thumbnail
  // tiles.
  // 
  class ThumbnailIterator : public Nocturnal::RefCountBase< ThumbnailIterator >
  {
  public:
    ThumbnailIterator()
    {
    }

    virtual ~ThumbnailIterator()
    {
    }

    virtual void Begin() = 0;
    virtual void End() = 0;
    virtual void Next() = 0;
    virtual ThumbnailTile* GetCurrentTile() = 0;
    virtual ThumbnailTile* GetLastTile() = 0;
    virtual bool IsDone() const = 0;
  };
  typedef Nocturnal::SmartPtr< ThumbnailIterator > ThumbnailIteratorPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Specialization of ThumbnailIterator that operates on a std::set of tiles.
  // 
  template< typename T >
  class ThumbnailSetIterator : public ThumbnailIterator
  {
  public:
    ThumbnailSetIterator( const T* tileSet, ThumbnailTile* start = NULL )
      : m_Set( tileSet )
    {
      if ( start )
      {
        m_Current = m_Set->find( start );
      }
    }

    virtual void Begin() NOC_OVERRIDE
    {
      m_Current = m_Set->begin();
    }

    virtual void End() NOC_OVERRIDE
    {
      m_Current = m_Set->end();
    }

    virtual void Next() NOC_OVERRIDE
    {
      ++m_Current;
    }

    virtual ThumbnailTile* GetCurrentTile() NOC_OVERRIDE
    {
      NOC_ASSERT( !IsDone() );
      return *m_Current;
    }

    virtual ThumbnailTile* GetLastTile() NOC_OVERRIDE
    {
      ThumbnailTile* tile = NULL;
      if ( !m_Set->empty() )
      {
        tile = *( m_Set->rbegin() );
      }
      return tile;
    }

    virtual bool IsDone() const NOC_OVERRIDE
    {
      return m_Current == m_Set->end();
    }

  private:
    const T* m_Set;
    typename T::const_iterator m_Current;
  };
}
