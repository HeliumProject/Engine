///////////////////////////////////////////////////////////////////////////////
// Name:        wx/wxTreeWndCtrlSpacer.h
// Purpose:     Spacer window (draws the lines, +/-, and so on)
// Author:      Insomniac Games
// Modified by:
// Created:     03/23/09
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TREEWNDCTRLSPACER_H_
#define _WX_TREEWNDCTRLSPACER_H_

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

// ----------------------------------------------------------------------------
// wxTreeWndCtrlSpacer
// ----------------------------------------------------------------------------

class LUNA_CORE_API wxTreeWndCtrlSpacer : public wxPanel
{
    DECLARE_DYNAMIC_CLASS(wxTreeWndCtrlSpacer)

public:
    wxTreeWndCtrlSpacer();
    
    wxTreeWndCtrlSpacer(wxTreeWndCtrl *parent,
                        const wxTreeItemId& node,
                        unsigned int numColumns);

    virtual bool Layout();
    
    wxTreeItemId GetNode() const { return m_node; }
    
    unsigned int GetNumColumns() const { return m_numColumns; }
    void SetNumColumns(unsigned int numColumns);

protected:
    int GetVerticalMidpoint(wxBitmap bitmap);
    wxPoint GetBitmapPosition(wxBitmap bitmap, unsigned int columnIndex);

    void DrawHorizontalLine(wxPaintDC& dc,
                            unsigned int x1,
                            unsigned int x2,
                            unsigned int y);

    void DrawVerticalLine(wxPaintDC& dc,
                          unsigned int y1,
                          unsigned int y2,
                          unsigned int x);

    void DrawLines(wxPaintDC& dc,
                   const wxTreeItemId& node,
                   int columnIndex,
                   bool leaf);

    void DrawToggle(wxPaintDC& dc, unsigned int columnIndex);

    void OnMouseDown(wxMouseEvent& e);
    void OnPaint(wxPaintEvent& e);    

private:
    unsigned int m_numColumns;
    wxTreeItemId m_node;
    wxTreeWndCtrl* m_treeWndCtrl;

    DECLARE_NO_COPY_CLASS(wxTreeWndCtrlSpacer)

    DECLARE_EVENT_TABLE();
};

}

#endif // _WX_TREEWNDCTRLSPACER_H_
