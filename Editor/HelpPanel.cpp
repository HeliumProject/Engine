#include "EditorPch.h"

#include "HelpPanel.h"

using namespace Helium;
using namespace Helium::Editor;

HelpPanel::HelpPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: HelpPanelGenerated( parent, id, pos, size, style )
{
    SetHelpText( "This is the help window.  It will update based on the control the mouse is over." );
    
    m_HelpRichText->SetEditable( false );
}

void HelpPanel::SetText( const char* text )
{
    m_HelpRichText->SetValue( text );
}