///////////////////////////////////////////////////////////////////////////////
// Name:        wx/wxTreeWndCtrlNode.h
// Purpose:     Tracks relationships (parent, children) and state (expanded).
// Author:      Insomniac Games
// Modified by:
// Created:     03/23/09
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TREEWNDCTRLNODE_H_
#define _WX_TREEWNDCTRLNODE_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "Luna/API.h"
#include "wx/wx.h"
#include "wx/treebase.h"

namespace Luna
{

// ----------------------------------------------------------------------------
// declarations
// ----------------------------------------------------------------------------

class wxTreeWndCtrl;
class wxTreeWndCtrlSpacer;

// ----------------------------------------------------------------------------
// wxTreeWndCtrlNode
// ----------------------------------------------------------------------------

class LUNA_CORE_API wxTreeWndCtrlNode
{
public:
    wxTreeWndCtrlNode(wxTreeWndCtrl* treeWndCtrl,
                      const wxTreeItemId& parent,
                      wxWindow *window,
                      unsigned int numColumns,
                      wxTreeItemData *data = NULL,
                      bool expanded = true);

    ~wxTreeWndCtrlNode();

    wxWindow *GetWindow() const
        { return m_window; }

    wxTreeItemId GetParent() const
        { return m_parent; }
    
    wxTreeWndCtrlSpacer* GetSpacer() const
        { return m_spacer; }

    wxTreeItemData *GetData() const
        { return m_data; }
    
    void SetData(wxTreeItemData *data)
        { m_data = data; }
        
protected:
    bool m_expanded;

    wxWindow *m_window;
    wxTreeWndCtrlSpacer *m_spacer;
    wxTreeItemData *m_data;

    wxTreeItemId m_id;
    wxTreeItemId m_parent;
    wxArrayTreeItemIds m_children;
    
    DECLARE_NO_COPY_CLASS(wxTreeWndCtrlNode)
    
    friend class wxTreeWndCtrl;
};

}

#endif // _WX_TREEWNDCTRLNODE_H_
