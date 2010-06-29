#pragma once

#include "Luna/API.h"
#include "SearchQuery.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Luna
{
  class BrowserSearch;

  /////////////////////////////////////////////////////////////////////////////
  class SearchHistory;
  typedef Nocturnal::SmartPtr< SearchHistory > SearchHistoryPtr;

  struct SearchHistoryChangeArgs
  {
    const SearchHistory* m_SearchHistory;
    const SearchQuery* m_SearchQuery;

    SearchHistoryChangeArgs( const SearchHistory* history, const SearchQuery* query )
      : m_SearchHistory( history )
      , m_SearchQuery( query )
    {
    }
  };
  typedef Nocturnal::Signature< void, const SearchHistoryChangeArgs& > SearchHistoryChangedSignature;
  typedef Nocturnal::Signature< bool, const SearchHistoryChangeArgs& > SearchHistoryChangingSignature;


  //////////////////////////////////////////////////////////////
  struct MRUQueriesChangedArgs
  {
    const OS_SearchQuery& m_MRUQueries;
    MRUQueriesChangedArgs( const OS_SearchQuery& mruQueries ) : m_MRUQueries( mruQueries ) {}
  };
  typedef Nocturnal::Signature< void, const MRUQueriesChangedArgs& > MRUQueriesChangedSignature;


  /////////////////////////////////////////////////////////////////////////////
  class SearchHistory : public Reflect::Element
  {
  public:
    SearchHistory();
    ~SearchHistory();

    void SetBrowserSearch( BrowserSearch* browserSearch );

    void RunNewQuery( const tstring& queryString, const AssetCollection* collection = NULL );
    bool RunCurrentQuery();

    //
    // History API
    //

    void PushHistory( const SearchQuery* query );
    
    bool CanGoBack() const;
    bool CanGoForward() const;
    
    void Back( int historyIndex = -1 );
    void Forward( int historyIndex = -1 );

    void Reset();
    bool IsActive() const;

    SearchQuery* GetCurrentQuery() const { return m_Current; }
    const V_SearchQuery& GetBackQuery() const { return m_Back; }
    const V_SearchQuery& GetForwardQuery() const { return m_Forward; }

    int GetLength() const;
    int GetMaxLength() const;
    void SetMaxLength( int value );

    //
    // MRU API
    //
    
    u32 GetMaxMRUQueries() const { return m_MaxMRUQueries; }

    const OS_SearchQuery& GetMRUQueries() const;
    void PushMRU( const SearchQuery* query );
    SearchQueryPtr FindQuery( const tstring& queryString );

  public:
    REFLECT_DECLARE_CLASS( SearchHistory, Reflect::Element );
    static void EnumerateClass( Reflect::Compositor<SearchHistory>& comp );
    virtual void PreSerialize() NOC_OVERRIDE;
    virtual void PostDeserialize() NOC_OVERRIDE;

  private:
    void LimitMRUQueries();

    void OnCollectionChanged( const Reflect::ElementChangeArgs& args );

  private:
    BrowserSearch* m_BrowserSearch;

    //
    // History Members
    //

    SearchQueryPtr m_Current;
    V_SearchQuery  m_Back;
    V_SearchQuery  m_Forward;
    bool m_Active; // is the queue active, we don't want to modify the queue while we are commiting a change   
    int m_MaxHistory; // max allowed length of the queue

    //
    // MRU Members
    //
    u32                   m_MaxMRUQueries;
    V_SearchQuery         m_SavedMRUQueries;
    OS_SearchQuery        m_MRUQueries;       // Cache


    //
    // Events
    //

  private:
    SearchHistoryChangedSignature::Event m_Reset;
  public:
    void AddResetListener( const SearchHistoryChangedSignature::Delegate& listener )
    {
      m_Reset.Add( listener );
    }
    void RemoveResetListener( const SearchHistoryChangedSignature::Delegate& listener )
    {
      m_Reset.Remove( listener );
    }

  private:
    SearchHistoryChangedSignature::Event m_Changed;
  public:
    void AddHistoryChangedListener( const SearchHistoryChangedSignature::Delegate& listener )
    {
      m_Changed.Add( listener );
    }
    void RemoveHistoryChangedListener( const SearchHistoryChangedSignature::Delegate& listener )
    {
      m_Changed.Remove( listener );
    }

  private:
    SearchHistoryChangedSignature::Event m_GoBack;
  public:
    void AddGoBackListener( const SearchHistoryChangedSignature::Delegate& listener )
    {
      m_GoBack.Add( listener );
    }
    void RemoveGoBackListener( const SearchHistoryChangedSignature::Delegate& listener )
    {
      m_GoBack.Remove( listener );
    }

  private:
    SearchHistoryChangedSignature::Event m_GoForward;
  public:
    void AddGoForwardListener( const SearchHistoryChangedSignature::Delegate& listener )
    {
      m_GoForward.Add( listener );
    }
    void RemoveGoForwardListener( const SearchHistoryChangedSignature::Delegate& listener )
    {
      m_GoForward.Remove( listener );
    }

  private:
    MRUQueriesChangedSignature::Event m_MRUQueriesChangedListeners;
  public:
    void AddMRUQueriesChangedListener( const MRUQueriesChangedSignature::Delegate& listener )
    {
      m_MRUQueriesChangedListeners.Add( listener );
    }
    void RemoveMRUQueriesChangedListener( const MRUQueriesChangedSignature::Delegate& listener )
    {
      m_MRUQueriesChangedListeners.Remove( listener );
    }
  };
  typedef Nocturnal::SmartPtr< SearchHistory > SearchHistoryPtr;
}