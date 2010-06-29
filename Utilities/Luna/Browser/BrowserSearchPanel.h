#pragma once

#include "BrowserGenerated.h"
#include "BrowserFrame.h"

#include "Application/UI/FieldMRU.h"

namespace Luna
{
  class CollectionManager;

  /////////////////////////////////////////////////////////////////////////////
  // Small class to manage the filter information.
  class Filter
  {
  private:
    tstring m_Name;
    tstring m_Filters;
    std::set< tstring >    m_Extensions;

  public:
    Filter( const tstring& name, const tstring& filters );

    virtual ~Filter()
    {
      Clear();
    }

    const tstring& GetName() const { return m_Name; }
    const tstring& GetFilters() const { return m_Filters; }
    const std::set< tstring >& GetExtensions() const { return m_Extensions; }

    void Clear()
    {
      m_Filters.clear();
      m_Extensions.clear();
    }

    bool operator<( const Filter& rhs ) const
    {
      return m_Name < rhs.m_Name;
    }
  };

  ///////////////////////////////////////////////////////////////////////////////
  /// Class BrowserSearchPanel
  ///////////////////////////////////////////////////////////////////////////////
  class BrowserSearchPanel : public SearchPanelGenerated 
  {
  public:
    BrowserSearchPanel( BrowserFrame* browserFrame, wxWindow* parent );
    virtual ~BrowserSearchPanel();

  protected:
    virtual bool Show( bool show = true ) NOC_OVERRIDE;

    // Virtual event handlers, overide them in your derived class
    virtual void OnFieldTextEnter( wxCommandEvent& event ) NOC_OVERRIDE;
    virtual void OnFolderBrowseButtonClick( wxCommandEvent& event ) NOC_OVERRIDE;
    virtual void OnFieldText( wxCommandEvent& event ) NOC_OVERRIDE;
    virtual void OnSearchButtonClick( wxCommandEvent& event ) NOC_OVERRIDE;
    virtual void OnCancelButtonClick( wxCommandEvent& event ) NOC_OVERRIDE;

    void OnCollectionManagerChanged( const Reflect::ElementChangeArgs& args );

  private:
    void AddFilter( const tchar* filter );
    void SetFilterIndex( int filterIndex );
    void SetFilterIndex( const tstring& filter );
    tstring GetFilter(); 
    void UpdateFilters();
    const Filter* FindFilter( const tstring& name );

    void PopulateForm();
    void PopulateChoiceControl( wxControlWithItems* control, std::vector< tstring >& contents );
    void PopulateFileTypeChoice( std::vector< tstring >& contents );
    void PopulateCollectionsChoice();

    bool ProcessForm();

    void ConnectListners();
    void DisconnectListners();

  private:
    BrowserFrame* m_BrowserFrame;
    CollectionManager* m_CollectionManager;

    // maps the fields ID to the help text it should be using
    typedef std::map< int, tstring > M_DefaultFieldText;
    M_DefaultFieldText m_DefaultFieldText;

    typedef Nocturnal::OrderedSet< Filter > OS_Filter;
    OS_Filter m_Filters;

    Nocturnal::FieldMRUPtr m_FieldMRU;
  };
}
