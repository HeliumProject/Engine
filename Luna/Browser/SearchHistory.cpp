#include "Precompile.h"
#include "SearchHistory.h"

#include "BrowserSearch.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
REFLECT_DEFINE_CLASS( SearchHistory );
void SearchHistory::EnumerateClass( Reflect::Compositor<SearchHistory>& comp )
{
  Reflect::Field* fieldMaxMRUQueries = comp.AddField( &SearchHistory::m_MaxMRUQueries, "m_MaxMRUQueries" );
  Reflect::Field* fieldSavedMRUQueries = comp.AddField( &SearchHistory::m_SavedMRUQueries, "m_SavedMRUQueries", Reflect::FieldFlags::Hide );
}

///////////////////////////////////////////////////////////////////////////////
SearchHistory::SearchHistory()
: m_BrowserSearch( NULL )
, m_Current( NULL )
, m_Active( false )
, m_MaxHistory( -1 )
, m_MaxMRUQueries( 15 )
{
}

SearchHistory::~SearchHistory()
{
}

///////////////////////////////////////////////////////////////////////////////
void SearchHistory::PreSerialize()
{
  __super::PreSerialize();

  LimitMRUQueries();

  m_SavedMRUQueries.clear();
  for ( OS_SearchQuery::Iterator itr = m_MRUQueries.Begin(), end = m_MRUQueries.End(); itr != end; ++itr )
  {
    m_SavedMRUQueries.push_back( *itr );
  }
}

///////////////////////////////////////////////////////////////////////////////
void SearchHistory::PostDeserialize()
{
  __super::PostDeserialize();

  m_MRUQueries.Clear();
  for ( V_SearchQuery::const_iterator itr = m_SavedMRUQueries.begin(), end = m_SavedMRUQueries.end(); itr != end; ++itr )
  {
    m_MRUQueries.Append( *itr );
  }
  LimitMRUQueries();
}

///////////////////////////////////////////////////////////////////////////////
void SearchHistory::SetBrowserSearch( BrowserSearch* browserSearch )
{
  m_BrowserSearch = browserSearch;
}

///////////////////////////////////////////////////////////////////////////////
void SearchHistory::RunNewQuery( const tstring& queryString, const AssetCollection* collection )
{
  NOC_ASSERT( m_BrowserSearch );

  // parse the query string
  SearchQueryPtr query = FindQuery( queryString );
  if ( !query )
  {
    query = new SearchQuery();
    query->SetQueryString( queryString );
  }

  if ( collection )
  {
    query->SetCollection( collection );
  }
  
  PushHistory( query );
  PushMRU( query );

  if ( m_BrowserSearch )
  {
    m_BrowserSearch->RequestSearch( query );
  }
}

///////////////////////////////////////////////////////////////////////////////
bool SearchHistory::RunCurrentQuery()
{
  SearchQuery* query = GetCurrentQuery();
  if ( query )
  {
    PushMRU( query );

    if ( m_BrowserSearch )
    {
      m_BrowserSearch->RequestSearch( query );
    }
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
/// History Functions
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
void SearchHistory::PushHistory( const SearchQuery* query )
{
  // we should always have a query here
  NOC_ASSERT( query );

  if ( m_Active || !query )
  {
    return;
  }

  if ( m_Current && (*m_Current) == (*query) )
  {
    return;
  }

  //if ( m_BatchState > 0 )
  //{
  //  m_Batch->Push( query );
  //  return;
  //}

  // we have a new query, so delete all subsequent queries from our current position
  m_Forward.clear();

  // if we have a finite length and we are full, remove the oldest query
  while ( m_MaxHistory > 0 && GetLength() >= m_MaxHistory )
  {
    m_Back.erase( m_Back.begin() );
  }

  // if we have a current, move that to the Back list
  if ( m_Current )
  {
    // append our query to the queue
    m_Back.push_back( m_Current );

    AssetCollection* oldCollection = m_Current->GetCollection();
    if ( oldCollection )
    {
      oldCollection->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &SearchHistory::OnCollectionChanged ) );
    }
  }

  m_Current = query;

  AssetCollection* collection = m_Current->GetCollection();
  if ( collection )
  {
    collection->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &SearchHistory::OnCollectionChanged ) );
  }
  
  // fire an event to interested listeners
  m_Changed.Raise( SearchHistoryChangeArgs( this, query ) );
}

///////////////////////////////////////////////////////////////////////////////
bool SearchHistory::CanGoBack() const
{
  return m_Back.size() > 0;
}

///////////////////////////////////////////////////////////////////////////////
bool SearchHistory::CanGoForward() const
{
  return m_Forward.size() > 0;
}

///////////////////////////////////////////////////////////////////////////////
void SearchHistory::Back( int historyIndex )
{
  m_Active = true;

  int index = historyIndex < 0 ? (int)m_Back.size() - 1 : historyIndex;

  // if the back stack is not empty
  if ( (int)m_Back.size() > index )
  {
    // get the query at the current position
    SearchQueryPtr query = NULL;
    
    while ( m_Back.size() && (int)m_Back.size() > index )
    {
      query = m_Back.back();
      m_Back.pop_back();

      //if there's something in Current, move it to the forward list
      if ( m_Current )
      {
        m_Forward.push_back( m_Current );

        AssetCollection* oldCollection = m_Current->GetCollection();
        if ( oldCollection )
        {
          oldCollection->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &SearchHistory::OnCollectionChanged ) );
        }
      }

      m_Current = query;

      AssetCollection* collection = m_Current->GetCollection();
      if ( collection )
      {
        collection->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &SearchHistory::OnCollectionChanged ) );
      }
    }

    if ( query )
    {
      PushMRU( query );

      if ( m_BrowserSearch )
      {
        m_BrowserSearch->RequestSearch( query );
      }

      m_GoBack.Raise( SearchHistoryChangeArgs( this, query.Ptr() ) );
    }
  }

  m_Active = false;
}

///////////////////////////////////////////////////////////////////////////////
void SearchHistory::Forward( int historyIndex )
{
  m_Active = true;

  int index = historyIndex < 0 ? (int)m_Forward.size() - 1 : historyIndex;

  // if the forward stack is not empty
  if ( (int)m_Forward.size() > index )
  {
    // get the query at the next position
    SearchQueryPtr query = NULL;
    
    while ( m_Forward.size() && (int)m_Forward.size() > index )
    {
      query = m_Forward.back();
      m_Forward.pop_back();

      //if there's something in Current, move it to the back list
      if ( m_Current )
      {
        m_Back.push_back( m_Current );

        AssetCollection* oldCollection = m_Current->GetCollection();
        if ( oldCollection )
        {
          oldCollection->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &SearchHistory::OnCollectionChanged ) );
        }
      }

      m_Current = query;

      AssetCollection* collection = m_Current->GetCollection();
      if ( collection )
      {
        collection->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &SearchHistory::OnCollectionChanged ) );
      }
    }

    if ( query )
    {
      PushMRU( query );

      if ( m_BrowserSearch )
      {
        m_BrowserSearch->RequestSearch( query );
      }

      m_GoForward.Raise( SearchHistoryChangeArgs( this, query.Ptr() ) );
    }
  }

  m_Active = false;
}

///////////////////////////////////////////////////////////////////////////////
void SearchHistory::Reset()
{
  m_Current = NULL;
  m_Back.clear();
  m_Forward.clear();
  m_Active = false;
  m_Reset.Raise( SearchHistoryChangeArgs( this, NULL ) );
}


//
// Accessors
//

///////////////////////////////////////////////////////////////////////////////
bool SearchHistory::IsActive() const
{
  return m_Active;
}

///////////////////////////////////////////////////////////////////////////////
int SearchHistory::GetLength() const
{
  return static_cast< int >( m_Back.size() + m_Forward.size() );
}



/////////////////////////////////////////////////////////////////////////////////
/// MRU Functions
/////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
const OS_SearchQuery& SearchHistory::GetMRUQueries() const
{
  return m_MRUQueries;
}

///////////////////////////////////////////////////////////////////////////////
void SearchHistory::PushMRU( const SearchQuery* query )
{
  if ( !query )
    return;

  while ( m_MRUQueries.Contains( query ) )
  {
    m_MRUQueries.Remove( query );
  }

  m_MRUQueries.Append( query );
  LimitMRUQueries();

  m_MRUQueriesChangedListeners.Raise( GetMRUQueries() );
}

///////////////////////////////////////////////////////////////////////////////
SearchQueryPtr SearchHistory::FindQuery( const tstring& queryString )
{
  for ( OS_SearchQuery::Iterator itr = m_MRUQueries.Begin(), end = m_MRUQueries.End(); itr != end; ++itr )
  {
    if ( (*itr)->GetQueryString() == queryString )
    {
      return (*itr);
    }
  }

  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
void SearchHistory::LimitMRUQueries()
{
  const u32 maxQueries = GetMaxMRUQueries();
  while ( m_MRUQueries.Size() && ( m_MRUQueries.Size() > maxQueries ) )
  {
    m_MRUQueries.Remove( m_MRUQueries.Front() );
  }
}

///////////////////////////////////////////////////////////////////////////////
void SearchHistory::OnCollectionChanged( const Reflect::ElementChangeArgs& args )
{
  const AssetCollection* collection = Reflect::ConstObjectCast< AssetCollection >( args.m_Element );
  if ( collection && m_Current && m_Current->GetCollection() == collection )
  {
    RunCurrentQuery();
  }
}
