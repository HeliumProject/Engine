//////////////////////////////////////////////
// SimpleMDIChild.cpp
//  Definitions for the CLitesqlView and CSimpleMDIChild classes

#include "LitesqlMDIChild.h"
#include "resource.h"

#include "objectmodel.hpp"

using namespace xml;

CModelTreeView::TreeItemData::TreeItemData()
{}

CModelTreeView::TreeItemData::~TreeItemData()
{
  for (vector<TreeItemData*>::const_iterator it = children.begin();it !=children.end();it++)
    {
      delete(*it);
    }
}

class FieldTreeItemData : public CModelTreeView::TreeItemData 
{
public:
  FieldTreeItemData(Field* field) : m_pField(field) 
  {};
  //		virtual ~ModelTreeItemData();

  virtual LPSTR GetText() {return "field"; };
  //  private:
  Field* m_pField;
};  //class TreeItemData

class ObjectTreeItemData : public CModelTreeView::TreeItemData 
{
public:
  ObjectTreeItemData(Object* object) : m_pObject(object) 
  {
    for (vector<Field*>::const_iterator fit = object->fields.begin();fit !=object->fields.end();fit++)
    {
      children.push_back(new FieldTreeItemData(*fit));
    }
  };
  
  virtual LPSTR GetText() {return "object"; };
  //  private:
  Object* m_pObject;
};  //class TreeItemData

class ModelTreeItemData : public CModelTreeView::TreeItemData 
{
public:
  ModelTreeItemData(ObjectModel* model) : m_pModel(model) 
  {
    for (vector<Object*>::const_iterator oit = model->objects.begin();oit !=model->objects.end();oit++)
    {
      children.push_back(new ObjectTreeItemData(*oit));
    }
  };

  virtual LPSTR GetText() {return "model"; };
  //  private:
  ObjectModel* m_pModel;
};  //class TreeItemData

class RootItemData : public CModelTreeView::TreeItemData 
{
public:
  RootItemData():modelItem(NULL) {};
  virtual LPSTR GetText() {return "root"; };
  
  ModelTreeItemData* modelItem; 
};  //class TreeItemData

// CLitesqlView definitions
CLitesqlView::CLitesqlView(LitesqlDocument* pDoc)
{
  m_pDocument = pDoc;
  mTree.setObjectModel(&m_pDocument->getModel());
  SetPanes(mTree,mFiles);
  SetBarPos(4*50);
}

// CSimpleMDIChild definitions
CSimpleMDIChild::CSimpleMDIChild(const char* pszFilename)
: m_Document(pszFilename),m_View(&m_Document)
{

  // Set m_MainView as the view window of the frame
  SetView(m_View);


  // Set the menu for this MDI child
  SetChildMenu(_T("MdiMenuView"));
}

CSimpleMDIChild::~CSimpleMDIChild()
{
}

void CSimpleMDIChild::OnInitialUpdate()
{
  std::string title = _T("Litesql Gen Project - ");
  title.append(m_Document.getFilename());
  // Set the window caption
  ::SetWindowText(m_hWnd, title.c_str());

  // Set the window icons
  SetIconLarge(IDI_VIEW);
  SetIconSmall(IDI_VIEW);
}

CFilesTab::CFilesTab()
{ 
  AddTabPage(new CViewText,"Cpp"); 
  AddTabPage(new CViewText,"hpp");
};

void CModelTreeView::setObjectModel(ObjectModel* pModel)
{
  m_pModel = pModel;

//  loadTree();
}

void CModelTreeView::OnInitialUpdate()
{
  DWORD dwStyle = (DWORD)GetWindowLongPtr(GWL_STYLE);
  dwStyle |= TVS_HASBUTTONS | TVS_HASLINES |TVS_SINGLEEXPAND;
  SetWindowLongPtr(GWL_STYLE, dwStyle);
  hRootItem = AddItem(NULL,(LPARAM)new RootItemData);
  
//  loadTree();
}

void CModelTreeView::loadTree()
{
  RootItemData* rootData = (RootItemData*)GetItemData(hRootItem);
  AddItem(hRootItem,(LPARAM)new ModelTreeItemData(m_pModel));
  UpdateWindow();
}

void CModelTreeView::DoContextMenu(CPoint& ptScreen)
{
  CPoint m = ptScreen;
  HTREEITEM selected = GetSelection();
      
  //ScreenToClient(m_hWnd, &m);
  //HMENU hPopupMenu = LoadMenu(GetApp()->GetResourceHandle(),MAKEINTRESOURCE(IDM_MODELTREE_VIEW));
  HMENU hPopup = CreatePopupMenu();
  AppendMenu(hPopup,MF_STRING,IDM_ADD_OBJECT,_T("Add Object"));
  AppendMenu(hPopup,MF_STRING,IDM_REMOVE_OBJECT,_T("Remove Object"));

  UINT idCmd = ::TrackPopupMenu(hPopup, TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON ,
							ptScreen.x, ptScreen.y, 0, m_hWnd, NULL);

  switch (idCmd)
  {
  case IDM_ADD_OBJECT:
    m_pModel->objects.push_back(new xml::Object("newObject",""));
    loadTree();
    break;
  case IDM_REMOVE_OBJECT:
    {
      HTREEITEM selected = GetSelection();
      DeleteItem(selected );
    }
    
    break;

  default:
    ;

  }
						//A Treeview control sometimes requires this to end the
						// TrackPopupMenu properly
						::PostMessage(GetParent(), WM_CANCELMODE, 0, 0);
            ::DestroyMenu(hPopup);
}

LRESULT CModelTreeView::OnNotifyReflect(WPARAM /*wParam*/, LPARAM lParam)
{
  LPNMHDR  lpnmh = (LPNMHDR) lParam;

  switch(lpnmh->code)
  {
  case NM_DBLCLK:
    {
      CPoint ptScreen;
      ::GetCursorPos(&ptScreen);
    }
    break;
    
  case NM_RCLICK:
    {
      CPoint ptScreen;
      ::GetCursorPos(&ptScreen);
      DoContextMenu(ptScreen);
    }
    break;
  case TVN_GETDISPINFO:
    {

      LPNMTVDISPINFO lpdi = (LPNMTVDISPINFO)lParam;
      
      TreeItemData* pItem = (TreeItemData*)lpdi->item.lParam;
      
      if (pItem)
      {
      	if(lpdi->item.mask & TVIF_TEXT)
        {
          lpdi->item.pszText = pItem->GetText();
        }
        
        if(lpdi->item.mask & TVIF_CHILDREN)
        {
          lpdi->item.cChildren = pItem->hasChildren() ? 1 : 0;
        }
      }
      else
      {
        lpdi->item.pszText = "NULL";
        lpdi->item.cChildren = I_CHILDRENCALLBACK;
      }

      
 
      //	//do we need to supply the text?
      //	if(lpdi->item.mask & TVIF_TEXT)
      //	{
      //		SHFILEINFO sfi = {0};

      //		//get the display name of the item
      //		if(pItem->GetFullCpidl().SHGetFileInfo(0, sfi, SHGFI_PIDL | SHGFI_DISPLAYNAME))
      //			::lstrcpyn(lpdi->item.pszText, sfi.szDisplayName, lpdi->item.cchTextMax -1);
      //	}

      //	//do we need to supply the unselected image?
      //	if(lpdi->item.mask & TVIF_IMAGE)
      //	{
      //		SHFILEINFO sfi = {0};

      //		//get the unselected image for this item
      //		if(pItem->GetFullCpidl().SHGetFileInfo(0, sfi, SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_LINKOVERLAY))
      //			lpdi->item.iImage = sfi.iIcon;
      //	}

      //	//do we need to supply the selected image?
      //	if(lpdi->item.mask & TVIF_SELECTEDIMAGE)
      //	{
      //		SHFILEINFO sfi = {0};

      //		//get the selected image for this item
      //		if(pItem->GetFullCpidl().SHGetFileInfo(0, sfi, SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON))
      //			lpdi->item.iSelectedImage = sfi.iIcon;
      //	}
    }
    break;

  case TVN_ITEMEXPANDING:
    {
      LPNMTREEVIEW pnmtv = (LPNMTREEVIEW)lParam;

      switch(pnmtv->action)
      {
      case TVE_EXPAND:
        {
          UINT ExpandedOnce = pnmtv->itemNew.state & TVIS_EXPANDEDONCE;
          //if (!ExpandedOnce)
          //	GetChildItems(pnmtv->itemNew.hItem);
        }
        break;
      }
    }
    break;

  case TVN_SELCHANGED:
    {
      LPNMTREEVIEW pnmtv = (LPNMTREEVIEW)lParam;
      TreeItemData* pItem = (TreeItemData*)pnmtv->itemNew.lParam;

      //CMyListView* LeftView = GetExplorerApp().GetMainFrame().GetListView();
      //LeftView->DisplayFolder(pItem->GetParentFolder(), pItem->GetFullCpidl(), pItem->GetRelCpidl());
    }
    break;


//  default:
  }

  // return 0L for unhandled notifications
  return 0L;
}

HTREEITEM CModelTreeView::AddItem(HTREEITEM hParent, LPARAM lParam)
{
  TVITEM tvi = {0};
  tvi.mask = TVIF_PARAM|TVIF_CHILDREN|TVIF_TEXT;
  tvi.lParam = lParam;
  tvi.cChildren = I_CHILDRENCALLBACK;
  tvi.pszText = LPSTR_TEXTCALLBACK;

  TVINSERTSTRUCT tvis = {0};
  tvis.hParent = hParent;
  tvis.item = tvi;

  return InsertItem(tvis);
}
