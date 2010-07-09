#pragma once

#include <vector>

#include "Luna/API.h"
#include "Foundation/File/Path.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Luna
{
  class SearchResults : public Nocturnal::RefCountBase< SearchResults >
  {
  public:
    SearchResults( u32 browserSearchID = 0 );
    SearchResults( const SearchResults* results );
    virtual ~SearchResults();

    void Clear();
    bool HasResults() const;

    const std::map< u64, Nocturnal::Path >& GetPathsMap() const;
    bool AddPath( const Nocturnal::Path& path );
    bool RemovePath( const Nocturnal::Path& path );

    i32 GetSearchID() { return m_BrowserSearchID; }
  private:
    i32 m_BrowserSearchID; // This is the ID of the BrowserSearch that created these results, for easy of debugging
    std::map< u64, Nocturnal::Path >  m_Paths;

    const Nocturnal::Path* Find( const u64& hash ) const;
  };
  typedef Nocturnal::SmartPtr< SearchResults > SearchResultsPtr;
}
