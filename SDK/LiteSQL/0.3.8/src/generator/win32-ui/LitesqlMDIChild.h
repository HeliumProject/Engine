//////////////////////////////////////////////////////
// SimpleMDIChild.h
//  Declaration of the CLitesqlView and CSimpleMDIChild class

#ifndef MDICHILDVIEW_H
#define MDICHILDVIEW_H

#include "text.h"
#include "mdi.h"
#include "treeview.h"
#include "splitter.h"

#include "litesqldocument.hpp"

class CModelTreeView : public CTreeView
{
public:
  class TreeItemData //A nested class inside CMyTreeView
	{
	public:
		TreeItemData();
    virtual ~TreeItemData();
    
    virtual bool hasChildren() {return !children.empty(); };
    virtual TreeItemData* GetChildren(size_t index) { return index<children.size() ? children[index] : NULL; };
    virtual size_t GetChildrenCount() {return children.size(); };
    virtual LPSTR GetText() {return NULL; };
//  protected:
    std::vector<TreeItemData*> children;
	};  //class TreeItemData

  CModelTreeView() { };
  virtual ~CModelTreeView() {};

  void setObjectModel(litesql::ObjectModel* pModel);

protected:
  void OnInitialUpdate();
  LRESULT OnNotifyReflect(WPARAM /*wParam*/, LPARAM /*lParam*/);
  void DoContextMenu(CPoint& ptScreen);
  void DoItemMenu(HTREEITEM hItem, CPoint& ptScreen);
  
  private:
    HTREEITEM AddItem(HTREEITEM hParent, LPCTSTR szText, int iImage);
    HTREEITEM AddItem(HTREEITEM hParent, LPARAM lParam);

    void loadTree();
    litesql::ObjectModel* m_pModel;
    
    HTREEITEM hRootItem;


};

class CFilesTab : public CTab
{
public:
  CFilesTab();
  virtual ~CFilesTab() {};
};

// Declaration of CLitesqlView
class CLitesqlView : public CSplitter
{
public:
	CLitesqlView(LitesqlDocument* pDocument);
  virtual ~CLitesqlView(){};

private:

  LitesqlDocument* m_pDocument;
  
  CModelTreeView mTree;
  CFilesTab mFiles;
};



// Declaration of CSimpleMDIChild
class CSimpleMDIChild : public CMDIChild
{
public:
	CSimpleMDIChild(LPCTSTR pszFilename=NULL);
  virtual ~CSimpleMDIChild();

  LitesqlDocument* getDocument() { return &m_Document; };
protected:
	virtual void OnInitialUpdate();

private:
  CLitesqlView m_View;
  LitesqlDocument m_Document;
};

#endif  //MDICHILDVIEW_H