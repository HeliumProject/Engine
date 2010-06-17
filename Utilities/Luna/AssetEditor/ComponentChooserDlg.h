#pragma once

// Includes
#include "Foundation/Memory/SmartPtr.h"
#include "Pipeline/Component/ComponentCategory.h"

namespace Luna
{
  // Forwards
  class AssetEditor;
  class ComponentCategoriesPanel;
  class ComponentCategoryPanel;

  /////////////////////////////////////////////////////////////////////////////
  // Dialog that displays the available attributes and lets you add the 
  // attributes to any currently selected asset classes.
  // 
  class ComponentChooserDlg : public wxDialog
  {
  private:
    typedef std::map< i32, Component::ComponentBase* > M_ListIdToAttrib;
    typedef std::map< size_t, M_ListIdToAttrib > M_TabToAttribList;
    typedef std::map< size_t, Component::ComponentCategory* > M_TabToCategory;

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
    ComponentCategoriesPanel* m_Panel;
    wxToolBar* m_Toolbar;
	  wxMenu* m_MenuView;
    wxListCtrl* m_CurrentList;
    M_TabToAttribList m_ComponentLists;
    M_TabToCategory m_Categories;

  public:
    ComponentChooserDlg( AssetEditor* editor, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize );
    virtual ~ComponentChooserDlg();

  private:
    ComponentCategoryPanel* GetCategoryPanel( const Component::ComponentCategoryPtr& category, M_ListIdToAttrib& attribList );
    void NextView( wxListCtrl* list );
    void SetDescription( const std::string& desc, Icon icon );
    void SetDescription( Component::ComponentBase* attribute );
    void SetDescription( Component::ComponentCategory* category );
    void AddComponent( const Component::ComponentPtr& attributeToClone );
    Component::ComponentBase* FindComponent( size_t tabNumber, i32 itemId );
    bool ValidateComponent( const Component::ComponentPtr& attribute, std::string& message );
    void AutoSizeNameColumn( wxListCtrl* list );
    void ConnectListCallbacks();
    void DisconnectListCallbacks();

    // GUI callbacks
  private:
    void OnAddComponent( wxCommandEvent& args );
    void OnListItemDoubleClick( wxListEvent& args );
    void OnListItemSelected( wxListEvent& args );
    void OnListItemDeselected( wxListEvent& args );
    void OnChangeView( wxCommandEvent& args );
    void OnNotebookPageChanged( wxNotebookEvent& args );
  };
}
