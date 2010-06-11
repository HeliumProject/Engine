#pragma once

// Includes
#include "Foundation/Memory/SmartPtr.h"
#include "Attribute/AttributeCategory.h"

namespace Luna
{
  // Forwards
  class AssetEditor;
  class AttributeCategoriesPanel;
  class AttributeCategoryPanel;

  /////////////////////////////////////////////////////////////////////////////
  // Dialog that displays the available attributes and lets you add the 
  // attributes to any currently selected asset classes.
  // 
  class AttributeChooserDlg : public wxDialog
  {
  private:
    typedef std::map< i32, Attribute::AttributeBase* > M_ListIdToAttrib;
    typedef std::map< size_t, M_ListIdToAttrib > M_TabToAttribList;
    typedef std::map< size_t, Attribute::AttributeCategory* > M_TabToCategory;

  private:
    // Icon to display at bottom of dialog next to message.
    enum Icon
    {
      IconNone,
      IconError
    };

    // ID for each column displayed in the list views
    enum Column
    {
      ColumnName = 0,
      ColumnError
    };

  private:
    AssetEditor* m_Editor;
    AttributeCategoriesPanel* m_Panel;
    wxToolBar* m_Toolbar;
	  wxMenu* m_MenuView;
    wxListCtrl* m_CurrentList;
    M_TabToAttribList m_AttributeLists;
    M_TabToCategory m_Categories;

  public:
    AttributeChooserDlg( AssetEditor* editor, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize );
    virtual ~AttributeChooserDlg();

  private:
    AttributeCategoryPanel* GetCategoryPanel( const Attribute::AttributeCategoryPtr& category, M_ListIdToAttrib& attribList );
    void NextView( wxListCtrl* list );
    void SetDescription( const std::string& desc, Icon icon );
    void SetDescription( Attribute::AttributeBase* attribute );
    void SetDescription( Attribute::AttributeCategory* category );
    void AddAttribute( const Attribute::AttributePtr& attributeToClone );
    Attribute::AttributeBase* FindAttribute( size_t tabNumber, i32 itemId );
    bool ValidateAttribute( const Attribute::AttributePtr& attribute, std::string& message );
    void AutoSizeNameColumn( wxListCtrl* list );
    void ConnectListCallbacks();
    void DisconnectListCallbacks();

    // GUI callbacks
  private:
    void OnAddAttribute( wxCommandEvent& args );
    void OnListItemDoubleClick( wxListEvent& args );
    void OnListItemSelected( wxListEvent& args );
    void OnListItemDeselected( wxListEvent& args );
    void OnChangeView( wxCommandEvent& args );
    void OnNotebookPageChanged( wxNotebookEvent& args );
  };
}
