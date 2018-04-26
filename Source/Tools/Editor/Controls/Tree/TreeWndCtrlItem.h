///////////////////////////////////////////////////////////////////////////////
// Name:        wx/TreeWndCtrlItem.h
// Purpose:     This is a standard item (i.e. icons and text)
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

// ----------------------------------------------------------------------------
// definitions
// ----------------------------------------------------------------------------

#define WXTWC_DEFAULT_ITEM_SPACING     5

namespace Helium
{

    // ----------------------------------------------------------------------------
    // declarations
    // ----------------------------------------------------------------------------

    class TreeWndCtrl;

    // ----------------------------------------------------------------------------
    // TreeWndCtrlItem
    // ----------------------------------------------------------------------------

    class WXDLLIMPEXP_TWC TreeWndCtrlItem : public wxPanel
    {
        DECLARE_DYNAMIC_CLASS(TreeWndCtrlItem)

    public:
        TreeWndCtrlItem();

        TreeWndCtrlItem(TreeWndCtrl *parent,
            const wxString& text,
            int image = -1,
            int stateImage = -1);

        void OnDoubleClick(wxMouseEvent& e);

        void SetSpacing(int spacing);

        int GetSpacing() const
        { return m_spacing; }

        void SetItem(const wxTreeItemId& item);

        wxTreeItemId GetItem() const
        { return m_item; }

        void SetText(const wxString& text);

        wxString GetText() const
        { return m_staticText->GetLabel(); }

        void SetImage(int image);

        int GetImage() const
        { return m_image; }

        void SetStateImage(int stateImage);

        int GetStateImage() const
        { return m_stateImage; }

    protected:
        int m_image;
        int m_stateImage;
        int m_spacing;
        wxTreeItemId m_item;
        wxStaticBitmap* m_staticBitmap;
        wxSizerItem* m_spacingItem;
        wxStaticText* m_staticText;
        TreeWndCtrl* m_treeWndCtrl;

        wxBitmap GetBitmap();

        DECLARE_NO_COPY_CLASS(TreeWndCtrlItem)

        DECLARE_EVENT_TABLE();
    };

}
