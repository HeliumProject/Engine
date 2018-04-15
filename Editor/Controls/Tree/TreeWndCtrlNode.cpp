///////////////////////////////////////////////////////////////////////////////
// Name:        TreeWndCtrlNode.cpp
// Purpose:     Tree control with windows node implementation
// Author:      Insomniac Games
// Created:     03/23/03
// Modified by:
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "Precompile.h"
#include "TreeWndCtrlNode.h"
#include "TreeWndCtrl.h"
#include "TreeWndCtrlSpacer.h"
#include "wx/arrimpl.cpp"

using namespace Helium;

// ----------------------------------------------------------------------------
// TreeWndCtrlNode
// ----------------------------------------------------------------------------

TreeWndCtrlNode::TreeWndCtrlNode(TreeWndCtrl* treeWndCtrl,
                                 const wxTreeItemId& parent,
                                 wxWindow *window,
                                 unsigned int numColumns,
                                 wxTreeItemData *data,
                                 bool expanded)
                                 : m_expanded(expanded),
                                 m_window(window),
                                 m_data(data),
                                 m_id(this),
                                 m_parent(parent)
{
    m_spacer = new TreeWndCtrlSpacer(treeWndCtrl, m_id, numColumns);
}

TreeWndCtrlNode::~TreeWndCtrlNode()
{
}
