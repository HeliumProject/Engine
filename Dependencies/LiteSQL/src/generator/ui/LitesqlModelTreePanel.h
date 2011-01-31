#ifndef LitesqlModelTreePanel_h
#define LitesqlModelTreePanel_h


#include "ui.h"

#include "objectmodel.hpp"

class wxModelItem;

typedef std::vector<wxModelItem*> wxModelItemArray;

class wxModelItem : public wxTreeItemData {
public:
	wxModelItem() : wxTreeItemData() { SetId(this);};
	virtual ~wxModelItem() {}; 
  virtual wxString GetLabel() const =0 ;
	virtual wxWindow* GetEditPanel(wxWindow *parent) {return NULL; };
	virtual bool hasChildren() const      {	return false;	};
	virtual wxModelItemArray* GetChildren()	{	return NULL;	};

	static void RefreshTree(wxTreeCtrl* pTree,wxTreeItemId& baseItem,wxModelItem* item);
};




class wxCompositeModelItem : public wxModelItem {
public:
	wxCompositeModelItem(); 
  virtual ~wxCompositeModelItem(); 

	
	wxModelItemArray* GetChildren();
	
protected:
	virtual void InitChildren()=0;
	wxModelItemArray m_children;
  
private:
  bool m_childrenInitalized;
};

class wxLitesqlModel : public wxCompositeModelItem {
public:
  wxLitesqlModel(litesql::ObjectModel* pModel);
  wxString GetLabel() const;
  wxWindow* GetEditPanel(wxWindow *parent);
  bool hasChildren() const {	return true; };
  litesql::ObjectModel* GetModel() {return m_pModel; };
  
protected:
  void InitChildren();
  
private:
  litesql::ObjectModel* m_pModel;
};

class LitesqlModelTreePanel : public ui::ModelTreePanel
{
public:
	/** Constructor */
  LitesqlModelTreePanel( wxWindow* parent);
  virtual ~LitesqlModelTreePanel();
  
  wxTreeCtrl* GetTreeCtrl()
  {
	  return m_modelTreeCtrl;
  }
  
  wxNotebook* GetDetailNotebook()
  {
    return m_detailNotebook;
  }

  void setObjectModel(litesql::ObjectModel* pModel);

  wxTreeItemId AddObject(xml::ObjectPtr& newObject);
  wxTreeItemId AddField();
  wxTreeItemId AddMethod();
  wxTreeItemId AddRelation();

  bool RemoveObject();
  bool RemoveField();
  bool RemoveMethod();
  bool RemoveRelation();
  
private:
  wxLitesqlModel* m_pModel;
  
  
	void OnTreeDeleteItem( wxTreeEvent& event ){ event.Skip(); }
	void OnTreeItemActivated( wxTreeEvent& event ){ event.Skip(); }
	void OnTreeItemGetTooltip( wxTreeEvent& event ){ event.Skip(); }
	void OnTreeSelChanged( wxTreeEvent& event );
	void OnTreeSelChanging( wxTreeEvent& event );  
};

#endif // #ifndef LitesqlModelTreePanel_h
