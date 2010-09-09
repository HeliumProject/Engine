///////////////////////////////////////////////////////////////////////////////
// Name:        TreeWndCtrlItem.cpp
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
#include "TreeWndCtrlItem.h"
#include "TreeWndCtrl.h"

using namespace Helium;

// ----------------------------------------------------------------------------
// TreeWndCtrlItem
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(TreeWndCtrlItem, wxPanel)

BEGIN_EVENT_TABLE( TreeWndCtrlItem, wxPanel )
EVT_LEFT_DCLICK( TreeWndCtrlItem::OnDoubleClick )
END_EVENT_TABLE()

TreeWndCtrlItem::TreeWndCtrlItem()
: wxPanel(),
m_image(-1),
m_stateImage(-1),
m_spacing(WXTWC_DEFAULT_ITEM_SPACING),
m_item(TreeWndCtrlItemIdInvalid),
m_staticBitmap(NULL),
m_spacingItem(NULL),
m_staticText(NULL),
m_treeWndCtrl(NULL)
{
}

TreeWndCtrlItem::TreeWndCtrlItem(TreeWndCtrl *parent,
                                 const wxString& text,
                                 int image,
                                 int stateImage)
                                 : wxPanel(parent),
                                 m_image(image),
                                 m_stateImage(stateImage),
                                 m_spacing(WXTWC_DEFAULT_ITEM_SPACING),
                                 m_item(TreeWndCtrlItemIdInvalid),
                                 m_staticBitmap(NULL),
                                 m_spacingItem(NULL),
                                 m_staticText(NULL),
                                 m_treeWndCtrl(parent)
{
    SetSizer( new wxBoxSizer( wxHORIZONTAL ) );
    wxSizer* sizer = GetSizer();

    m_staticBitmap = new wxStaticBitmap( this, wxID_ANY, GetBitmap() );
    m_staticBitmap->Connect( m_staticBitmap->GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler( TreeWndCtrlItem::OnDoubleClick ), NULL, this );
    sizer->Add( m_staticBitmap, 0, wxALIGN_CENTER, 0);

    m_spacingItem = sizer->AddSpacer( m_spacing );

    m_staticText = new wxStaticText( this, wxID_ANY, text );
    m_staticText->Connect( m_staticText->GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler( TreeWndCtrlItem::OnDoubleClick ), NULL, this );
    sizer->Add( m_staticText, 0, wxALIGN_CENTER, 0);

    SetMinSize( wxSize( wxDefaultSize.GetX(), m_treeWndCtrl->GetColumnSize() ) );
}

void TreeWndCtrlItem::OnDoubleClick(wxMouseEvent& e)
{
    if ( m_item == TreeWndCtrlItemIdInvalid )
        return;

    m_treeWndCtrl->Toggle(m_item);
}

void TreeWndCtrlItem::SetSpacing(int spacing)
{
    m_spacing = spacing;
    m_spacingItem->AssignSpacer( wxSize( m_spacing, -1 ) );
    Layout();
}

void TreeWndCtrlItem::SetItem(const wxTreeItemId& item)
{
    m_item = item;
    m_staticBitmap->SetBitmap( GetBitmap() );
    Layout();
}

void TreeWndCtrlItem::SetText(const wxString& text)
{
    m_staticText->SetLabel( text );
    Layout();
}    

void TreeWndCtrlItem::SetImage(int image)
{
    m_image = image;
    m_staticBitmap->SetBitmap( GetBitmap() );
    Layout();
}

void TreeWndCtrlItem::SetStateImage(int stateImage)
{
    m_stateImage = stateImage;
    m_staticBitmap->SetBitmap( GetBitmap() );
    Layout();
}

wxBitmap TreeWndCtrlItem::GetBitmap()
{
    wxBitmap bitmap;
    if ( m_item == TreeWndCtrlItemIdInvalid )
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
