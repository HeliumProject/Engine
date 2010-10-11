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
  for (std::vector<TreeItemData*>::const_iterator it = children.begin();it !=children.end();it++)
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

  virtual LPSTR GetText() {return  LITESQL_L( "field" ); };
  //  private:
  Field* m_pField;
};  //class TreeItemData

class ObjectTreeItemData : public CModelTreeView::TreeItemData 
{
public:
  ObjectTreeItemData(Object* object) : m_pObject(object) 
  {
    for (std::vector<Field*>::const_iterator fit = object->fields.begin();fit !=object->fields.end();fit++)
    {
      children.push_back(new FieldTreeItemData(*fit));
    }
  };
  
  virtual LPSTR GetText() {return  LITESQL_L( "object" ); };
  //  private:
  Object* m_pObject;
};  //class TreeItemData

class ModelTreeItemData : public CModelTreeView::TreeItemData 
{
public:
  ModelTreeItemData(ObjectModel* model) : m_pModel(model) 
  {
    for (std::vector<Object*>::const_iterator oit = model->objects.begin();oit !=model->objects.end();oit++)
    {
      children.push_back(new ObjectTreeItemData(*oit));
    }
  };

  virtual LPSTR GetText() {return  LITESQL_L( "model" ); };
  //  private:
  ObjectModel* m_pModel;
};  //class TreeItemData

class RootItemData : public CModelTreeView::TreeItemData 
{
public:
  RootItemData():modelItem(NULL) {};
  virtual LPSTR GetText() {return  LITESQL_L( "root" ); };
  
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
CSimpleMDIChild::CSimpleMDIChild(const LITESQL_Char* pszFilename)
: m_Document(pszFilename),m_View(&m_Document)
{

  // Set m_MainView as the view window of the frame
  SetView(m_View);


  // Set the menu for this MDI child
  SetChildMenu(_T( LITESQL_L( "MdiMenuView" )));
}

CSimpleMDIChild::~CSimpleMDIChild()
{
}

void CSimpleMDIChild::OnInitialUpdate()
{
  LITESQL_String title = _T( LITESQL_L( "Litesql Gen Project - " ));
  title.append(m_Document.getFilename());
  // Set the window caption
  ::SetWindowText(m_hWnd, title.c_str());

  // Set the window icons
  SetIconLarge(IDI_VIEW);
  SetIconSmall(IDI_VIEW);
}

CFilesTab::CFilesTab()
{ 
  AddTabPage(new CViewText, LITESQL_L( "Cpp" )); 
  AddTabPage(new CViewText, LITESQL_L( "hpp" ));
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
  AppendMenu(hPopup,MF_STRING,IDM_ADD_OBJECT,_T( LITESQL_L( "Add Object" )));
  AppendMenu(hPopup,MF_STRING,IDM_REMOVE_OBJECT,_T( LITESQL_L( "Remove Object" )));

  UINT idCmd = ::TrackPopupMenu(hPopup, TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON ,
							ptScreen.x, ptScreen.y, 0, m_hWnd, NULL);

  switch (idCmd)
  {
  case IDM_ADD_OBJECT:
    m_pModel->objects.push_back(new xml::Object( LITESQL_L( "newObject" ), LITESQL_L( "" )));
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
        lpdi->item.pszText =  LITESQL_L( "NULL" );
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