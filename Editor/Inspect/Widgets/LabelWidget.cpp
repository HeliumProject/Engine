#include "Precompile.h"
#include "LabelWidget.h"

#include <wx/panel.h>
#include <wx/stattext.h>

using namespace Helium;
using namespace Helium::Editor;

BEGIN_EVENT_TABLE(LabelWindow, wxPanel)
EVT_SIZE(LabelWindow::OnSize)
END_EVENT_TABLE()

LabelWindow::LabelWindow(wxWindow* parent, LabelWidget* labelWidget)
: wxPanel( parent )
, m_LabelWidget( labelWidget )
{
    SetSizer( new wxBoxSizer( wxHORIZONTAL ) );
    wxSizer* sizer = GetSizer();

    m_StaticText = new wxStaticText( this, wxID_ANY, wxT( "" ) );
    sizer->Add( m_StaticText, 0, wxALIGN_CENTER, 0);
}

void LabelWindow::OnSize(wxSizeEvent& event)
{
    m_LabelWidget->GetControl()->Read();

//#ifdef INSPECT_LAYOUT
    Layout();
//#endif
}

wxString LabelWindow::GetLabel() const
{
    return m_StaticText->GetLabelText();
}

void LabelWindow::SetLabel(const wxString& label)
{
    m_StaticText->SetLabel(label);
}

void LabelWindow::UpdateHelpText( const wxString& helpText )
{
    SetHelpText( helpText );
    m_StaticText->SetHelpText( helpText );
}

bool LabelWindow::SetForegroundColour(const wxColour& color)
{
    return wxPanel::SetForegroundColour(color)
        && m_StaticText->SetForegroundColour(color);
}

bool LabelWindow::SetBackgroundColour(const wxColour& color)
{
    return wxPanel::SetBackgroundColour(color)
        && m_StaticText->SetBackgroundColour(color);
}

LabelWidget::LabelWidget( Inspect::Label* label )
: m_LabelControl( label )
, m_LabelWindow( NULL )
{
    SetControl( label );
}

void LabelWidget::Create( wxWindow* parent )
{
    HELIUM_ASSERT( !m_LabelWindow );

    // allocate window and connect common listeners
    SetWindow( m_LabelWindow = new LabelWindow( parent, this ) );

    // add listeners
    m_LabelControl->a_HelpText.Changed().AddMethod( this, &LabelWidget::HelpTextChanged );

    // update state of attributes that are not refreshed during Read()
    m_LabelControl->a_HelpText.RaiseChanged();
}

void LabelWidget::Destroy()
{
    HELIUM_ASSERT( m_LabelWindow );

    SetWindow( NULL );

    // remove listeners
    m_LabelControl->a_HelpText.Changed().RemoveMethod( this, &LabelWidget::HelpTextChanged );

    // destroy window
    m_LabelWindow->Destroy();
    m_LabelWindow = NULL;
}

void LabelWidget::Read()
{
    HELIUM_ASSERT( m_Control->IsBound() );

    tstring text;
    m_LabelControl->ReadStringData( text );
    
    tstring trimmed = text;
    EllipsizeString( trimmed, m_Window->GetSize().GetWidth() );

    if ( trimmed != m_LabelWindow->GetLabel().c_str() )
    {
        m_LabelWindow->SetLabel( trimmed.c_str() );
    }
}

bool LabelWidget::Write()
{
    return true;
}

void LabelWidget::HelpTextChanged( const Attribute<tstring>::ChangeArgs& args )
{
    m_LabelWindow->UpdateHelpText( args.m_NewValue );
}