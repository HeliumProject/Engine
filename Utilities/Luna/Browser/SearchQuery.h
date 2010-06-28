#pragma once

#include "AssetCollection.h"
#include "DependencyCollection.h"

#include "Foundation/Container/OrderedSet.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Platform/Types.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  namespace SearchTypes
  {
    enum SearchType
    {
      Invalid = -1,
      File = 0,
      Folder,
      ID,
      DBSearch,
    };
    static void SearchTypesEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement( File, "File" );
      info->AddElement( Folder, "Folder" );
      info->AddElement( ID, "ID" );
      info->AddElement( DBSearch, "DBSearch" );      
    }
  }
  typedef SearchTypes::SearchType SearchType;

  ///////////////////////////////////////////////////////////////////////////////
  /// class SearchQuery
  ///////////////////////////////////////////////////////////////////////////////
  class SearchQuery;
  typedef Nocturnal::SmartPtr< SearchQuery > SearchQueryPtr;
  typedef std::vector< SearchQueryPtr > V_SearchQuery;
  typedef Nocturnal::OrderedSet< Nocturnal::SmartPtrComparator< SearchQuery > > OS_SearchQuery;

  class SearchQuery : public Reflect::Element
  {
  protected:
    // Only SearchHistory should ever create SearchQueries
    SearchQuery();
  
  public:
    ~SearchQuery();

    SearchType GetSearchType() const { return m_SearchType; }
    
    void SetQueryString( const std::string& queryString );
    const std::string& GetQueryString() const { return m_QueryString; }
    
    const Nocturnal::Path& GetQueryPath()
    {
        return m_QueryPath;
    }

    void SetCollection( const AssetCollection* collection );
    AssetCollection* GetCollection();
    const Nocturnal::Path& GetCollectionPath() const
    {
        return m_CollectionPath;
    }
    

    bool operator<( const SearchQuery& rhs ) const;
    bool operator==( const SearchQuery& rhs ) const;
    bool operator!=( const SearchQuery& rhs ) const;

    static bool ParseQueryString( const std::string& queryString, std::string& errors, SearchQuery* query = NULL );

  public:
    REFLECT_DECLARE_CLASS( SearchQuery, Reflect::Element );
    static void EnumerateClass( Reflect::Compositor<SearchQuery>& comp );
    virtual void PostDeserialize() NOC_OVERRIDE; 

  private:
    SearchType              m_SearchType;
    std::string             m_QueryString;
    Nocturnal::Path      m_QueryPath;
    Nocturnal::Path      m_CollectionPath;

    friend class SearchHistory;
  };
}
