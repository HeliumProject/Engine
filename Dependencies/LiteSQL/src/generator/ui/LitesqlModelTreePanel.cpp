#include "LitesqlModelTreePanel.h"

#include "LitesqlDatabasePanel.h"
#include "LitesqlObjectPanel.h"
#include "LitesqlRelationPanel.h"
#include "LitesqlFieldPanel.h"
#include "LitesqlMethodPanel.h"

using namespace xml;
using namespace litesql;
using namespace ui;

wxCompositeModelItem::wxCompositeModelItem(): wxModelItem(),m_childrenInitalized(false) {};

wxCompositeModelItem::~wxCompositeModelItem() 
{}

wxModelItemArray* wxCompositeModelItem::GetChildren() 
{
  if (!m_childrenInitalized)
  {
    InitChildren();
    m_childrenInitalized=true;
  }
  return &m_children;
}

class wxFieldItem : public wxModelItem 
{
public:
	wxFieldItem(Field::counted_ptr pField) : m_pField(pField) {}
  ~wxFieldItem()  { }
  
	wxString GetLabel() const			{ wxString label(m_pField->name.c_str(),wxConvUTF8);
		return label;	};
	
  wxWindow* GetEditPanel(wxWindow *parent) 			
  {	
    LitesqlFieldPanel* pPanel = new LitesqlFieldPanel(parent,m_pField);
    return pPanel;
  };
  
  Field::counted_ptr& field() { return m_pField; }
private:
	Field::counted_ptr m_pField;

};

class wxMethodItem: public wxModelItem 
{
public:
	wxMethodItem(Method::counted_ptr& pMethod) : m_pMethod(pMethod) {}
	~wxMethodItem()  {}
  
  wxString GetLabel() const			{ wxString label(m_pMethod->name.c_str(),wxConvUTF8);
		return label;	};
  
  wxWindow* GetEditPanel(wxWindow *parent) 			
  {	
    LitesqlMethodPanel* pPanel = new LitesqlMethodPanel(parent,m_pMethod);
    return pPanel;
  };
  Method::counted_ptr& method() {return m_pMethod; };
private:
	Method::counted_ptr m_pMethod;
};

class wxLitesqlObject : public wxCompositeModelItem {
	public:
    wxLitesqlObject(ObjectPtr& pObject,ObjectModel* pModel)
			:	m_pObject(pObject),m_pModel(pModel)	{};
  ~wxLitesqlObject()  { }
  
  wxString GetLabel() const			{ wxString label(m_pObject->name.c_str(),wxConvUTF8);
		return label;	};

  wxWindow* GetEditPanel(wxWindow *parent) 			
  {	
    LitesqlObjectPanel* pPanel = new LitesqlObjectPanel(parent,m_pObject);
    pPanel->setBaseClasses(m_pModel->objects);
    return pPanel;
  };

  void AddField(wxFieldItem* pField)
  {

    m_pObject->fields.push_back(pField->field());
    m_children.push_back(pField);
  }

  void AddMethod(wxMethodItem* pMethod)
  {
    m_pObject->methods.push_back(pMethod->method());
    m_children.push_back(pMethod);
  }

  bool hasChildren() const {	return true; };

		void InitChildren()
		{
			for (Field::sequence::iterator field = m_pObject->fields.begin();
				field != m_pObject->fields.end();
				field++)
			{
				m_children.push_back(new wxFieldItem(*field));
			}

			for (Method::sequence::iterator method = m_pObject->methods.begin();
				method != m_pObject->methods.end();
				method++)
			{
				m_children.push_back(new wxMethodItem(*method));
			}
		}
	
    ObjectPtr& object() {return m_pObject;}
private:
		ObjectPtr m_pObject;	
  ObjectModel* m_pModel;
};

class wxLitesqlRelation : public wxCompositeModelItem {
	public:
    wxLitesqlRelation(Relation::counted_ptr& pRelation,ObjectModel* pModel)
			:	m_pRelation(pRelation),m_pModel(pModel)	{};
  ~wxLitesqlRelation()  { }
  
  wxString GetLabel() const			{ wxString label(m_pRelation->name.c_str(),wxConvUTF8);
		return label;	};

  wxWindow* GetEditPanel(wxWindow *parent) 			
  {	
    LitesqlRelationPanel* pPanel = new LitesqlRelationPanel(parent,m_pRelation);
    return pPanel;
  };

  void AddField(wxFieldItem* pField)
  {
    m_pRelation->fields.push_back(pField->field());
    m_children.push_back(pField);
  }

  bool hasChildren() const {	return true; };

		void InitChildren()
		{
			for (Field::sequence::iterator field = m_pRelation->fields.begin();
				field != m_pRelation->fields.end();
				field++)
			{
				m_children.push_back(new wxFieldItem(*field));
			}
		}
	
private:
  Relation::counted_ptr m_pRelation;	
  ObjectModel* m_pModel;
};


wxLitesqlModel::wxLitesqlModel(ObjectModel* pModel)
:	m_pModel(pModel) { 	}

wxWindow* wxLitesqlModel::GetEditPanel(wxWindow *parent) 			
{	
  LitesqlDatabasePanel* pPanel = new LitesqlDatabasePanel(parent,m_pModel->db);
  return pPanel;
}


wxString wxLitesqlModel::GetLabel() const			{  wxString label(m_pModel->db->name.c_str(),wxConvUTF8);
  return label; };

void   wxLitesqlModel::InitChildren()
{
  for( ObjectSequence::iterator it = m_pModel->objects.begin(); it != m_pModel->objects.end();it++)
  {
    m_children.push_back(new wxLitesqlObject(*it,m_pModel));
    
  }
}

// ////////////////////////////////////////////////////////////////
//

/** Constructor */
LitesqlModelTreePanel::LitesqlModelTreePanel( wxWindow* parent)
: ModelTreePanel( parent )
{
  
}


LitesqlModelTreePanel::~LitesqlModelTreePanel()
{
//  if (m_pModel)
//    delete m_pModel;
}

void LitesqlModelTreePanel::setObjectModel(litesql::ObjectModel* pModel)
{
  m_pModel = new wxLitesqlModel(pModel);
  m_modelTreeCtrl->DeleteAllItems();
  wxTreeItemId rootId = m_modelTreeCtrl->AddRoot(_(LITESQL_L("root")));
  wxModelItem::RefreshTree(m_modelTreeCtrl,rootId,m_pModel);
}

wxTreeItemId LitesqlModelTreePanel::AddObject(ObjectPtr& newObject)
{
  wxLitesqlObject* pObject = new wxLitesqlObject(newObject,m_pModel->GetModel());
  m_pModel->GetModel()->objects.push_back(newObject);
  wxTreeItemId newItem = m_modelTreeCtrl->InsertItem(m_pModel->GetId(), -1, pObject->GetLabel(), -1, -1, pObject);

  m_modelTreeCtrl->SelectItem(newItem);

  return newItem;
}

wxTreeItemId LitesqlModelTreePanel::AddRelation()
{
  wxTreeItemId newItem;
  //wxLitesqlRelation* pRelation = new wxLitesqlRelation(new xml::Relation(,m_pModel->GetModel());
  //m_pModel->GetModel()->relations.push_back(pRelation->relation());
  //wxTreeItemId newItem = m_modelTreeCtrl->InsertItem(m_pModel->GetId(), -1, pRelation->GetLabel(), -1, -1, pRelation);

  //m_modelTreeCtrl->SelectItem(newItem);

  return newItem;
}

wxTreeItemId LitesqlModelTreePanel::AddField()
{
  wxTreeItemId newItem; 
  wxTreeItemId selectedItem = GetTreeCtrl()->GetSelection();
  if (selectedItem)
  {
    wxLitesqlObject* data = dynamic_cast<wxLitesqlObject*>(GetTreeCtrl()->GetItemData(selectedItem));
    if (data)
    {
      Field::counted_ptr newField(new xml::Field(LITESQL_L("newField"),AU_field_type,LITESQL_L(""),AU_field_indexed,AU_field_unique));
      wxFieldItem* field = new wxFieldItem(newField);
      data->AddField(field);
      newItem = m_modelTreeCtrl->InsertItem(selectedItem, -1, field->GetLabel(), -1, -1, field);
      m_modelTreeCtrl->SelectItem(newItem);
    }
  }
  return newItem;
}

wxTreeItemId LitesqlModelTreePanel::AddMethod()
{
  wxTreeItemId newItem; 
  wxTreeItemId selectedItem = GetTreeCtrl()->GetSelection();
  if (selectedItem)
  {
    wxLitesqlObject* data = dynamic_cast<wxLitesqlObject*>(GetTreeCtrl()->GetItemData(selectedItem));
    if (data)
    {
      Method::counted_ptr ptrMethod(new xml::Method(LITESQL_L("newMethod"), LITESQL_L("")));
      wxMethodItem* method = new wxMethodItem(ptrMethod);
      data->AddMethod(method);
      newItem = m_modelTreeCtrl->InsertItem(selectedItem, -1, method->GetLabel(), -1, -1, method);
      m_modelTreeCtrl->SelectItem(newItem);
    }
  }
  return newItem;
}

bool LitesqlModelTreePanel::RemoveObject()
{
  bool success;
  
  wxTreeItemId selectedItem = GetTreeCtrl()->GetSelection();
  if (!selectedItem)
  {
    success = false;
  }
  else
  {
    wxLitesqlObject* data = dynamic_cast<wxLitesqlObject*>(GetTreeCtrl()->GetItemData(selectedItem));
    if (data)
    {
      m_pModel->GetModel()->remove(data->object());
      m_modelTreeCtrl->Delete(selectedItem);
    }
    success = (data!=NULL);
  }
  return success;
}

bool LitesqlModelTreePanel::RemoveField()
{
  bool success;
  
  wxTreeItemId selectedItem = GetTreeCtrl()->GetSelection();
  if (!selectedItem)
  {
    success = false;
  }
  else
  {
    wxTreeItemId selectedItem = GetTreeCtrl()->GetSelection();
    wxFieldItem* data = dynamic_cast<wxFieldItem*>(GetTreeCtrl()->GetItemData(selectedItem));
    if (data)
    {
      m_pModel->GetModel()->remove(data->field());
      m_modelTreeCtrl->Delete(selectedItem);
    }
    success = (data!=NULL);
  }
  return success;
}

bool LitesqlModelTreePanel::RemoveMethod()
{
  bool success;
  
  wxTreeItemId selectedItem = GetTreeCtrl()->GetSelection();
  if (!selectedItem)
  {
    success = false;
  }
  else
  {
    wxMethodItem* data = dynamic_cast<wxMethodItem*>(GetTreeCtrl()->GetItemData(selectedItem));
    if (data)
    {
      m_pModel->GetModel()->remove(data->method());
      m_modelTreeCtrl->Delete(selectedItem);
    }
    success = (data!=NULL);
  }
  return success;
}

bool LitesqlModelTreePanel::RemoveRelation()
{
  return false;
}


void LitesqlModelTreePanel::OnTreeSelChanged( wxTreeEvent& event )
{
  int pageIndex = GetDetailNotebook()->GetSelection();
  
  if (pageIndex!=-1)
  {
    GetDetailNotebook()->GetPage(pageIndex)->TransferDataFromWindow();
    GetDetailNotebook()->DeletePage(pageIndex);
    GetDetailNotebook()->Layout();
  }
    // update the label after getting the data
  if (event.GetOldItem())
  {
    wxModelItem* pOldItem = (wxModelItem*)GetTreeCtrl()->GetItemData(event.GetOldItem());
    if (pOldItem)
    {
      GetTreeCtrl()->SetItemText(event.GetOldItem(), pOldItem->GetLabel());
    }
  }                            
  
  // add new page 
  wxModelItem* pItem = (wxModelItem*)GetTreeCtrl()->GetItemData(event.GetItem());
  if (pItem) {
    wxWindow *pPage = pItem->GetEditPanel(GetDetailNotebook());
    if (pPage)
    {
      GetDetailNotebook()->AddPage(pPage,pItem->GetLabel());
      
      GetDetailNotebook()->Layout();
      pPage->Layout();
      pPage->TransferDataToWindow();
    }
  }
}

void LitesqlModelTreePanel::OnTreeSelChanging( wxTreeEvent& event )
{
    //  wxModelItem* pItem = (wxModelItem*)GetTreeCtrl()->GetItemData(event.GetItem());
}

void wxModelItem::RefreshTree(wxTreeCtrl* pTree,wxTreeItemId& baseItem,wxModelItem* item)
{
	wxTreeItemId itemId = pTree->InsertItem(baseItem,item->GetId(), item->GetLabel());
  pTree->SetItemData(itemId, item);
	if (item->hasChildren())
	{
    wxModelItemArray* pChildren = item->GetChildren();
		for(wxModelItemArray::iterator it =  pChildren->begin(); 
        it != pChildren->end();
        it++)
		{
			RefreshTree(pTree,itemId,(wxModelItem*)(*it));
		}
	}
}

