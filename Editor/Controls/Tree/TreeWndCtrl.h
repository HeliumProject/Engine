/////////////////////////////////////////////////////////////////////////////
// Name:        wx/TreeWndCtrl.h
// Purpose:     Tree-like class that can have wxWindows as nodes.
// Author:      Insomniac Games
// Modified by:
// Created:     03/23/09
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#pragma once

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wx.h"
#include "wx/scrolwin.h"
#include "wx/treebase.h"

#include "TreeWndCtrlApi.h"

// ----------------------------------------------------------------------------
// defines
// ----------------------------------------------------------------------------

#define wxTR_ALL_LINES                    0x0000
#define wxTR_LINES_TOP                    0x1000
#define wxTR_LINES_CENTER                 0x2000
#define wxTR_LINES_BOTTOM                 0x4000
#define wxTR_USE_PEN_DASHES               0x8000

#define WXTWC_DEFAULT_CLICK_TOLERANCE     4
#define WXTWC_DEFAULT_COLUMN_SIZE         23

enum TreeWndCtrlDashMode
{
    wxTWC_DASH_DEFAULT,
    wxTWC_DASH_CUSTOM,
};

enum TreeWndCtrlLineMode
{
    wxTWC_LINE_TOP,
    wxTWC_LINE_CENTER,
    wxTWC_LINE_BOTTOM,
};

enum TreeWndCtrlLineDrawMode
{
    wxTWC_LINEDRAW_NONE,
    wxTWC_LINEDRAW_ROOT_ONLY,
    wxTWC_LINEDRAW_ALL,
};

enum TreeWndCtrlToggleMode
{
    wxTWC_TOGGLE_AUTOMATIC,
    wxTWC_TOGGLE_DISABLED,
    wxTWC_TOGGLE_ENABLED,
};

namespace Helium
{

    // ----------------------------------------------------------------------------
    // declarations
    // ----------------------------------------------------------------------------

    class TreeWndCtrlItem;

    // ----------------------------------------------------------------------------
    // globals
    // ----------------------------------------------------------------------------

    WXDLLIMPEXP_TWC extern const wxTreeItemId TreeWndCtrlItemIdInvalid;
    WXDLLIMPEXP_TWC extern wxBitmap TreeWndCtrlDefaultExpand;
    WXDLLIMPEXP_TWC extern wxBitmap TreeWndCtrlDefaultCollapse;
    WXDLLIMPEXP_TWC extern wxPen TreeWndCtrlDefaultPen;

    // ----------------------------------------------------------------------------
    // TreeWndCtrl
    // ----------------------------------------------------------------------------

    class WXDLLIMPEXP_TWC TreeWndCtrl : public wxScrolledWindow
    {
        DECLARE_DYNAMIC_CLASS(TreeWndCtrl)

    public:
        TreeWndCtrl();

        TreeWndCtrl(wxWindow *parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint &pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            long style = wxScrolledWindowStyle,
            const wxString &name = wxPanelNameStr,
            int treeStyle = wxTR_DEFAULT_STYLE,
            unsigned int columnSize = WXTWC_DEFAULT_COLUMN_SIZE,
            wxBitmap expandedBitmap = TreeWndCtrlDefaultExpand,
            wxBitmap collapsedBitmap = TreeWndCtrlDefaultCollapse,
            wxPen pen = TreeWndCtrlDefaultPen,
            unsigned int clickTolerance = WXTWC_DEFAULT_CLICK_TOLERANCE);

        virtual ~TreeWndCtrl();

        virtual bool Layout();

        virtual size_t GetCount() const;

        virtual int GetClickTolerance() const
        { return m_clickTolerance; }

        virtual void SetClickTolerance(int clickTolerance)
        { m_clickTolerance = clickTolerance; }

        virtual TreeWndCtrlDashMode GetDashMode() const
        { return m_dashMode; }

        virtual void SetDashMode(TreeWndCtrlDashMode dashMode);

        virtual TreeWndCtrlLineMode GetLineMode() const
        { return m_lineMode; }

        virtual void SetLineMode(TreeWndCtrlLineMode lineMode);

        virtual TreeWndCtrlLineDrawMode GetLineDrawMode() const
        { return m_lineDrawMode; }

        virtual void SetLineDrawMode(TreeWndCtrlLineDrawMode lineDrawMode);

        virtual TreeWndCtrlToggleMode GetToggleMode() const
        { return m_toggleMode; }

        virtual void SetToggleMode(TreeWndCtrlToggleMode toggleMode);

        virtual wxBitmap GetExpandedBitmap() const
        { return m_expandedBitmap; }

        virtual void SetExpandedBitmap(wxBitmap expandedBitmap)
        { m_expandedBitmap = expandedBitmap; }

        virtual wxBitmap GetCollapsedBitmap() const
        { return m_collapsedBitmap; }

        virtual void SetCollapsedBitmap(wxBitmap collapsedBitmap)
        { m_collapsedBitmap = collapsedBitmap; }

        virtual wxPen GetPen() const
        { return m_pen; }

        virtual void SetPen(wxPen pen)
        { m_pen = pen; }

        virtual unsigned int GetColumnSize() const
        { return m_columnSize; }

        virtual void SetColumnSize(unsigned int columnSize);

        virtual bool GetHideRoot() const
        { return m_hideRoot; }

        virtual void SetHideRoot(bool hideRoot);

        virtual wxTreeItemId AddRoot(wxWindow *window,
            wxTreeItemData *data = NULL);

        virtual wxTreeItemId AddRoot(const wxString& text,
            int image = -1,
            int selImage = -1,
            wxTreeItemData *data = NULL);

        virtual wxTreeItemId GetRootItem() const
        { return m_root; }

        virtual wxWindow* GetItemWindow(const wxTreeItemId &item) const;

        virtual wxTreeItemId GetItemParent(const wxTreeItemId &item) const;
        virtual wxTreeItemId GetFirstChild(const wxTreeItemId &item) const;
        virtual wxTreeItemId GetNextChild(const wxTreeItemId &item,
            const wxTreeItemId &lastChild) const;
        virtual wxTreeItemId GetPrevChild(const wxTreeItemId &item,
            const wxTreeItemId &lastChild) const;
        virtual wxTreeItemId GetLastChild(const wxTreeItemId &item) const;
        virtual wxTreeItemId GetNextSibling(const wxTreeItemId &item) const;
        virtual wxTreeItemId GetPrevSibling(const wxTreeItemId &item) const;

        virtual wxTreeItemId GetFirstVisibleItem();
        virtual wxTreeItemId GetNextVisible(const wxTreeItemId &item);
        virtual wxTreeItemId GetPrevVisible(const wxTreeItemId &item);

        virtual size_t GetChildrenCount(const wxTreeItemId &item,
            bool recursively) const;

        virtual wxBitmap GetToggleBitmap(const wxTreeItemId &item);

        virtual bool IsToggleVisible(const wxTreeItemId &item);

        bool ToggleVisible(const wxTreeItemId &item)
        { return IsToggleVisible(item); }

        wxTreeItemId PrependItem(const wxTreeItemId &parent,
            wxWindow *window,
            wxTreeItemData *data = NULL)
        { return DoInsertItem(parent, (size_t) 0, window, data); }

        wxTreeItemId InsertItem(const wxTreeItemId &parent,
            const wxTreeItemId &previous,
            wxWindow *window,
            wxTreeItemData *data = NULL)
        { return DoInsertAfter(parent, previous, window, data); }

        wxTreeItemId InsertItem(const wxTreeItemId &parent,
            size_t pos,
            wxWindow *window,
            wxTreeItemData *data = NULL)
        { return DoInsertItem(parent, pos, window, data); }

        wxTreeItemId AppendItem(const wxTreeItemId &parent,
            wxWindow *window,
            wxTreeItemData *data = NULL)
        { return DoInsertItem(parent, (size_t) -1, window, data); }

        wxTreeItemId PrependItem(const wxTreeItemId &parent,
            const wxString &text,
            int image = -1,
            int selImage = -1,
            wxTreeItemData *data = NULL)
        { return DoInsertItem(parent, 0u, text, image, selImage, data); }

        wxTreeItemId InsertItem(const wxTreeItemId &parent,
            const wxTreeItemId &previous,
            const wxString &text,
            int image = -1,
            int selImage = -1,
            wxTreeItemData *data = NULL)
        { return DoInsertAfter(parent, previous, text, image, selImage, data); }

        wxTreeItemId InsertItem(const wxTreeItemId &parent,
            size_t pos,
            const wxString &text,
            int image = -1,
            int selImage = -1,
            wxTreeItemData *data = NULL)
        { return DoInsertItem(parent, pos, text, image, selImage, data); }

        wxTreeItemId AppendItem(const wxTreeItemId &parent,
            const wxString &text,
            int image = -1,
            int selImage = -1,
            wxTreeItemData *data = NULL)
        { return DoInsertItem(parent, (size_t) -1, text, image, selImage, data); }


        virtual void Delete(const wxTreeItemId &item);
        virtual void DeleteChildren(const wxTreeItemId &item);
        virtual void DeleteAllItems();

        virtual wxTreeItemId FindItem(const wxWindow* window) const;

        virtual wxTreeItemId FindItem(const wxTreeItemId &item, const wxWindow* window) const;

        virtual wxString GetItemText(const wxTreeItemId &item) const;

        virtual int GetItemImage(const wxTreeItemId &item,
            wxTreeItemIcon which = wxTreeItemIcon_Normal) const;

        virtual wxTreeItemData *GetItemData(const wxTreeItemId &item) const;

        virtual wxColour GetItemTextColour(const wxTreeItemId &item) const;

        virtual wxColour GetItemBackgroundColour(const wxTreeItemId &item) const;

        virtual wxFont GetItemFont(const wxTreeItemId &item) const;

        virtual void SetItemText(const wxTreeItemId &item, const wxString &text);

        virtual void SetItemImage(const wxTreeItemId &item,
            int image,
            wxTreeItemIcon which = wxTreeItemIcon_Normal);

        virtual void SetItemData(const wxTreeItemId &item, wxTreeItemData *data);

        virtual void SetItemTextColour(const wxTreeItemId &item,
            const wxColour &col);

        virtual void SetItemBackgroundColour(const wxTreeItemId &item,
            const wxColour &col);

        virtual void SetItemFont(const wxTreeItemId &item,
            const wxFont &font);

        virtual bool IsExpanded(const wxTreeItemId &item) const;
        virtual bool IsVisible(const wxTreeItemId &item) const;
        virtual bool ItemHasChildren(const wxTreeItemId &item) const
        { return GetChildrenCount(item, false) > 0; }

        bool HasChildren(const wxTreeItemId &item) const
        { return ItemHasChildren(item); }

        virtual void Expand(const wxTreeItemId &item);
        void ExpandAllChildren(const wxTreeItemId &item);
        void ExpandAll();

        virtual void Collapse(const wxTreeItemId &item);
        void CollapseAllChildren(const wxTreeItemId &item);
        void CollapseAll();

        virtual void Toggle(const wxTreeItemId &item);
        virtual void SetExpanded(const wxTreeItemId &item, bool expanded);

        wxImageList *GetImageList() const { return m_imageListNormal; }
        wxImageList *GetStateImageList() const { return m_imageListState; }

        virtual void SetImageList(wxImageList *imageList);
        virtual void SetStateImageList(wxImageList *imageList);

        void AssignImageList(wxImageList *imageList)
        {
            SetImageList(imageList);
            m_ownsImageListNormal = true;
        }

        void AssignStateImageList(wxImageList *imageList)
        {
            SetStateImageList(imageList);
            m_ownsImageListState = true;
        }

        virtual void AdjustTreeScrollbars();

    protected:
        virtual wxTreeItemId DoInsertItem(const wxTreeItemId &parent,
            size_t pos,
            const wxString &text,
            int image = -1,
            int selImage = -1,
            wxTreeItemData *data = NULL);

        virtual wxTreeItemId DoInsertAfter(const wxTreeItemId &parent,
            const wxTreeItemId &previous,
            const wxString &text,
            int image = -1,
            int selImage = -1,
            wxTreeItemData *data = NULL);

        virtual wxTreeItemId DoInsertItem(const wxTreeItemId &parent,
            size_t pos,
            wxWindow *window,
            wxTreeItemData *data = NULL);

        virtual wxTreeItemId DoInsertAfter(const wxTreeItemId &parent,
            const wxTreeItemId &previous,
            wxWindow *window,
            wxTreeItemData *data = NULL);

        TreeWndCtrlItem* CreateTreeWndCtrlItem(const wxString &text, int image = -1, int selImage = -1);

        void AddNodeToLayout(wxSizer* treeSizer, const wxTreeItemId &item, bool parentExpanded = true);

        void UpdateNumColumns(const wxTreeItemId &item, unsigned int numColumns);

        void GetVisibleItems(wxArrayTreeItemIds &visibleNodeList, const wxTreeItemId &item);

        wxRect GetItemBounds(const wxTreeItemId& item, bool recursive = true) const;

    protected:
        int m_clickTolerance;

        TreeWndCtrlDashMode m_dashMode;
        TreeWndCtrlLineMode m_lineMode;
        TreeWndCtrlLineDrawMode m_lineDrawMode;
        TreeWndCtrlToggleMode m_toggleMode;

        wxBitmap m_expandedBitmap;
        wxBitmap m_collapsedBitmap;
        wxPen m_pen;

        wxWindowList m_allocatedWindows;

        wxImageList *m_imageListNormal;
        bool m_ownsImageListNormal;

        wxImageList *m_imageListState;
        bool m_ownsImageListState;

        unsigned int m_columnSize;

        bool m_hideRoot;

        bool m_dirty;

        wxTreeItemId m_lastToggledItem;
        wxTreeItemId m_root;

        DECLARE_NO_COPY_CLASS(TreeWndCtrl)
    };

}
