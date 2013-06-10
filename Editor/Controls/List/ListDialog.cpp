#include "EditorPch.h"
#include "ListDialog.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// title = Dialog title
// desc = String to display in a static control above the list control.
// msgs = List of strings to display in the list control.
// 
ListDialog::ListDialog( wxWindow* parent, const std::string& title, const std::string& desc, const std::vector< std::string >& msgs ) 
: wxDialog( parent, wxID_ANY, title.c_str(), wxDefaultPosition, wxSize( 360, 260 ), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
, m_Description( desc )
{
    SetSizeHints( wxSize( 245, 200 ), wxDefaultSize );

    wxBoxSizer* mainSizer = new wxBoxSizer( wxVERTICAL );

    m_StaticText = new wxStaticText( this, wxID_ANY, m_Description.c_str(), wxDefaultPosition, wxDefaultSize, 0 );
    m_StaticText->Wrap( GetSize().x - 10 );
    mainSizer->Add( m_StaticText, 0, wxALL, 5 );

    m_MsgList = new SortableListView( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES | wxLC_NO_HEADER | wxLC_REPORT | wxLC_SINGLE_SEL );
    mainSizer->Add( m_MsgList, 1, wxALL | wxEXPAND, 5 );

    wxBoxSizer* buttonSizer = new wxBoxSizer( wxVERTICAL );

    m_OK = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    buttonSizer->Add( m_OK, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5 );

    mainSizer->Add( buttonSizer, 0, wxEXPAND, 5 );

    // Fill out the list control
    m_MsgList->InsertColumn( 0, TXT( "Message" ) );
    int row = 0;
    for each ( const std::string& msg in msgs )
    {
        wxListItem strItem;
        strItem.SetMask( wxLIST_MASK_TEXT );
        strItem.SetText( msg.c_str() );
        strItem.SetId( row++ );
        strItem.SetColumn( 0 );
        m_MsgList->InsertItem( strItem );
    }
    m_MsgList->SetColumnWidth( 0, wxLIST_AUTOSIZE );

    SetSizer( mainSizer );
    Layout();
}
