#include "Precompile.h"

#include "HelpPanel.h"

using namespace Editor;

HelpPanel::HelpPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: HelpPanelGenerated( parent, id, pos, size, style )
{
    SetHelpText( TXT( "This is the help window.  It will update based on the control the mouse is over." ) );
}

void HelpPanel::SetText( const tchar* text )
{
    m_HelpRichText->SetValue( text );
}