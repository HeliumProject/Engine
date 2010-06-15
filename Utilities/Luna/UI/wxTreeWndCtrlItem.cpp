///////////////////////////////////////////////////////////////////////////////
// Name:        wxTreeWndCtrlItem.cpp
// Purpose:     Implementation for standard item
// Author:      Insomniac Games
// Created:     03/23/03
// Modified by:
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "Precompile.h"
#include "wxTreeWndCtrlItem.h"
#include "wxTreeWndCtrl.h"

using namespace Luna;

// ----------------------------------------------------------------------------
// wxTreeWndCtrlItem
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeWndCtrlItem, wxPanel)

BEGIN_EVENT_TABLE( wxTreeWndCtrlItem, wxPanel )
EVT_PAINT( wxTreeWndCtrlItem::OnPaint )
EVT_LEFT_DCLICK( wxTreeWndCtrlItem::OnDoubleClick )
END_EVENT_TABLE()

wxTreeWndCtrlItem::wxTreeWndCtrlItem()
                 : wxPanel(),
                   m_bitmapTextWidth(-1),
                   m_image(-1),
                   m_stateImage(-1),
                   m_spacing(WXTWC_DEFAULT_ITEM_SPACING),
                   m_dirty(true),
                   m_bitmapPoint(0, 0),
                   m_textPoint(0, 0),
                   m_item(wxTreeWndCtrlItemIdInvalid),
                   m_text(""),
                   m_treeWndCtrl(NULL)
{
}

wxTreeWndCtrlItem::wxTreeWndCtrlItem(wxTreeWndCtrl *parent,
                                     const wxString& text,
                                     int image,
                                     int stateImage)
                 : wxPanel(parent),
                   m_bitmapTextWidth(-1),
                   m_image(image),
                   m_stateImage(stateImage),
                   m_spacing(WXTWC_DEFAULT_ITEM_SPACING),
                   m_dirty(true),
                   m_bitmapPoint(0, 0),
                   m_textPoint(0, 0),
                   m_item(wxTreeWndCtrlItemIdInvalid),
                   m_text(text),
                   m_treeWndCtrl(parent)
{
}

bool wxTreeWndCtrlItem::Layout()
{
    if ( m_dirty )
    {
        Freeze();
        
        m_bitmapPoint = wxPoint(0, 0);
        m_textPoint = wxPoint(0, 0);

        wxCoord width = 0;
        wxCoord height = 0;

        int bitmapWidth = 0;
        int bitmapHeight = 0;
        wxBitmap bitmap = GetBitmap();
        if ( bitmap.Ok() )
        {
          bitmapWidth = bitmap.GetWidth();
          bitmapHeight = bitmap.GetHeight();
        }

        if ( bitmapWidth )
            width = bitmapWidth + m_spacing;
        
        wxCoord textWidth = 0;
        wxCoord textHeight = 0;
            
        wxClientDC dc(this);
        wxFont oldFont = dc.GetFont();
        dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
        dc.GetTextExtent(m_text, &textWidth, &textHeight);
        dc.SetFont(oldFont);

        m_textPoint.x = width;
        width += textWidth;

        if ( bitmapHeight > textHeight )
        {
            height = bitmapHeight;
            m_textPoint.y = (bitmapHeight - textHeight) / 2;
        }
        else
        {
            height = textHeight;
            m_bitmapPoint.y = (textHeight - bitmapHeight) / 2;
        }
        
        SetSize(width, height);
        m_bitmapTextWidth = width;

        Thaw();
        m_dirty = false;
    }
    
    return __super::Layout();
}

void wxTreeWndCtrlItem::OnPaint(wxPaintEvent& e)
{
    wxPaintDC dc(this);
    
    wxBitmap bitmap = GetBitmap();
    if ( bitmap.Ok() )
    {
      dc.DrawBitmap(bitmap, m_bitmapPoint.x, m_bitmapPoint.y, true);
    }

    wxFont oldFont = dc.GetFont();
    dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    dc.DrawText(m_text, m_textPoint.x, m_textPoint.y);
    dc.SetFont(oldFont);
    
    e.Skip();
}

void wxTreeWndCtrlItem::OnDoubleClick(wxMouseEvent& e)
{
    if ( m_item == wxTreeWndCtrlItemIdInvalid )
        return;

    if ( ( m_bitmapTextWidth >= 0 ) && ( e.GetPosition().x > m_bitmapTextWidth ) )
        return;

    m_treeWndCtrl->Toggle(m_item);
}

void wxTreeWndCtrlItem::SetSpacing(int spacing)
{
    m_spacing = spacing;
    m_dirty = true;
    Layout();
}

void wxTreeWndCtrlItem::SetItem(const wxTreeItemId& item)
{
    m_item = item;
    m_dirty = true;
    Layout();
}

void wxTreeWndCtrlItem::SetText(const wxString& text)
{
    m_text = text;
    m_dirty = true;
    Layout();
}    

void wxTreeWndCtrlItem::SetImage(int image)
{
    m_image = image;
    m_dirty = true;
    Layout();
}

void wxTreeWndCtrlItem::SetStateImage(int stateImage)
{
    m_stateImage = stateImage;
    m_dirty = true;
    Layout();
}

wxBitmap wxTreeWndCtrlItem::GetBitmap()
{
    wxBitmap bitmap;
    if ( m_item == wxTreeWndCtrlItemIdInvalid )
        return bitmap;

    int image = -1;
    wxImageList* imageList = NULL;
    if ( m_treeWndCtrl->ToggleVisible(m_item) && m_treeWndCtrl->IsExpanded(m_item) )
    {
        image = m_stateImage;
        imageList = m_treeWndCtrl->GetStateImageList();
    }
    else
    {
        image = m_image;
        imageList = m_treeWndCtrl->GetImageList();
    }
    
    if ( imageList && ( image >= 0 ) )
    {
        bitmap = imageList->GetBitmap(image);
    }
    
    return bitmap;
}
