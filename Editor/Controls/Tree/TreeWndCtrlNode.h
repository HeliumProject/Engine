///////////////////////////////////////////////////////////////////////////////
// Name:        wx/TreeWndCtrlNode.h
// Purpose:     Tracks relationships (parent, children) and state (expanded).
// Author:      Insomniac Games
// Modified by:
// Created:     03/23/09
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#pragma once

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wx.h"
#include "wx/treebase.h"

#include "TreeWndCtrlApi.h"

namespace Helium
{

    // ----------------------------------------------------------------------------
    // declarations
    // ----------------------------------------------------------------------------

    class TreeWndCtrl;
    class TreeWndCtrlSpacer;

    // ----------------------------------------------------------------------------
    // TreeWndCtrlNode
    // ----------------------------------------------------------------------------

    class WXDLLIMPEXP_TWC TreeWndCtrlNode
    {
    public:
        TreeWndCtrlNode(TreeWndCtrl* treeWndCtrl,
            const wxTreeItemId& parent,
            wxWindow *window,
            unsigned int numColumns,
            wxTreeItemData *data = NULL,
            bool expanded = true);

        ~TreeWndCtrlNode();

        wxWindow *GetWindow() const
        { return m_window; }

        wxTreeItemId GetParent() const
        { return m_parent; }

        TreeWndCtrlSpacer* GetSpacer() const
        { return m_spacer; }

        wxTreeItemData *GetData() const
        { return m_data; }

        void SetData(wxTreeItemData *data)
        { m_data = data; }

    protected:
        bool m_expanded;

        wxWindow *m_window;
        TreeWndCtrlSpacer *m_spacer;
        wxTreeItemData *m_data;

        wxTreeItemId m_id;
        wxTreeItemId m_parent;
        wxArrayTreeItemIds m_children;

        DECLARE_NO_COPY_CLASS(TreeWndCtrlNode)

        friend class TreeWndCtrl;
    };

}
