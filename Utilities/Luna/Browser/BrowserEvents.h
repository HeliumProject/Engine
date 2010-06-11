#pragma once

#include "SearchResults.h"
#include "SearchQuery.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Platform/Types.h"

namespace Luna
{  
  //////////////////////////////////////////////////////////////
  // RequestSearch:
  // A search has been requested.
  struct RequestSearchArgs
  {
    SearchQueryPtr m_SearchQuery;
    RequestSearchArgs( SearchQuery* searchQuery ) : m_SearchQuery( searchQuery ) {}
  };
  typedef Nocturnal::Signature< void, const RequestSearchArgs& > RequestSearchSignature;


  //////////////////////////////////////////////////////////////
  // SearchError:
  // A search has been requested.
  struct SearchErrorArgs
  {
    std::string m_Errors;
    SearchErrorArgs( const std::string& errors ) : m_Errors( errors ) {}
  };
  typedef Nocturnal::Signature< void, const SearchErrorArgs& > SearchErrorSignature;


  //////////////////////////////////////////////////////////////
  // BeginSearching:
  // Search is about to starting running, contains status update info pointers.
  struct BeginSearchingArgs
  {
    BeginSearchingArgs() { }
  };
  typedef Nocturnal::Signature< void, const BeginSearchingArgs& > BeginSearchingSignature;

  
  //////////////////////////////////////////////////////////////
  // StoppingSearch:
  // Search is done, results are being wrangled.
  struct StoppingSearchArgs
  {
    StoppingSearchArgs() { }
  };
  typedef Nocturnal::Signature< void, const StoppingSearchArgs& > StoppingSearchSignature;

  
  //////////////////////////////////////////////////////////////
  // ResultsAvailable:
  // {Some or all) search results are available, contains SearchResults
  struct ResultsAvailableArgs
  {
    SearchQueryPtr m_SearchQuery;
    SearchResultsPtr m_SearchResults;
    ResultsAvailableArgs( SearchQuery* searchQuery, SearchResults* searchResults )
      : m_SearchQuery( searchQuery )
      , m_SearchResults( searchResults )
    {

    }
  };
  typedef Nocturnal::Signature< void, const ResultsAvailableArgs& > ResultsAvailableSignature;


  //////////////////////////////////////////////////////////////
  // SearchComplete: 
  // Search is complete.
  struct SearchCompleteArgs
  {
    SearchQueryPtr m_SearchQuery;
    SearchCompleteArgs( SearchQuery* searchQuery ) : m_SearchQuery( searchQuery ) {}
  };
  typedef Nocturnal::Signature< void, const SearchCompleteArgs& > SearchCompleteSignature;

} // namespace Luna
