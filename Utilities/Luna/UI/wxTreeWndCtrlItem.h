///////////////////////////////////////////////////////////////////////////////
// Name:        wx/wxTreeWndCtrlItem.h
// Purpose:     This is a standard item (i.e. icons and text)
// Author:      Insomniac Games
// Modified by:
// Created:     03/23/09
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TREEWNDCTRLITEM_H_
#define _WX_TREEWNDCTRLITEM_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "Luna/API.h"
#include "wx/wx.h"
#include "wx/treebase.h"

// ----------------------------------------------------------------------------
// definitions
// ----------------------------------------------------------------------------

#define WXTWC_DEFAULT_ITEM_SPACING     5

namespace Luna
{

// ----------------------------------------------------------------------------
// declarations
// ----------------------------------------------------------------------------

class wxTreeWndCtrl;

// ----------------------------------------------------------------------------
// wxTreeWndCtrlItem
// ----------------------------------------------------------------------------

class LUNA_CORE_API wxTreeWndCtrlItem : public wxPanel
{
    DECLARE_DYNAMIC_CLASS(wxTreeWndCtrlItem)
    
public:
    wxTreeWndCtrlItem();
    
    wxTreeWndCtrlItem(wxTreeWndCtrl *parent,
                      const wxString& text,
                      int image = -1,
                      int stateImage = -1);

    virtual bool Layout();
    
    void OnPaint(wxPaintEvent& e);
    
    void OnDoubleClick(wxMouseEvent& e);

    void SetSpacing(int spacing);
    
    int GetSpacing() const
        { return m_spacing; }
    
    void SetItem(const wxTreeItemId& item);
    
    wxTreeItemId GetItem() const
        { return m_item; }
    
    void SetText(const wxString& text);
    
    wxString GetText() const
        { return m_text; }
    
    void SetImage(int image);
    
    int GetImage() const
        { return m_image; }
    
    void SetStateImage(int stateImage);

    int GetStateImage() const
        { return m_stateImage; }
    
protected:
    int m_bitmapTextWidth;
    int m_image;
    int m_stateImage;
    int m_spacing;
    bool m_dirty;
    wxPoint m_bitmapPoint;
    wxPoint m_textPoint;
    wxTreeItemId m_item;
    wxString m_text;
    wxTreeWndCtrl* m_treeWndCtrl;
    
    wxBitmap GetBitmap();
    
    DECLARE_NO_COPY_CLASS(wxTreeWndCtrlItem)

    DECLARE_EVENT_TABLE();
};

}

#endif // _WX_TREEWNDCTRLITEM_H_
