#include "Precompile.h"
#include "Label.h"

#include <wx/panel.h>
#include <wx/stattext.h>

using namespace Helium;
using namespace Helium::Editor;

BEGIN_EVENT_TABLE(LabelWindow, wxPanel)
EVT_SIZE(LabelWindow::OnSize)
END_EVENT_TABLE()

LabelWindow::LabelWindow(wxWindow* parent, LabelWidget* labelWidget)
: wxPanel (parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER, wxT( "Text" ) )
, m_LabelWidget (labelWidget)
{
    SetSizer( new wxBoxSizer( wxHORIZONTAL ) );
    wxSizer* sizer = GetSizer();

    m_StaticText = new wxStaticText( this, wxID_ANY, wxT( "Temp" ) );
    sizer->Add( m_StaticText, 0, wxALIGN_CENTER_VERTICAL, 0);
}

void LabelWindow::OnSize(wxSizeEvent& event)
{
    m_LabelWidget->GetControl()->Read();

    Layout();
}

wxString LabelWindow::GetLabel() const
{
    return m_StaticText->GetLabelText();
}

void LabelWindow::SetLabel(const wxString& label)
{
    m_StaticText->SetLabel(label);
}

void LabelWindow::UpdateToolTip( const wxString& toolTip )
{
    SetToolTip( toolTip );
    m_StaticText->SetToolTip( toolTip );
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

LabelWidget::LabelWidget( Inspect::Control* control )
: Widget( control )
{
    
}

void LabelWidget::Create( wxWindow* parent )
{
    HELIUM_ASSERT( !m_LabelWindow );

    // allocate window and connect common listeners
    SetWindow( m_LabelWindow = new LabelWindow( parent, this ) );

    // add listeners
    m_LabelControl->a_ToolTip.Changed().AddMethod( this, &LabelWidget::ToolTipChanged );

    // update state of attributes that are not refreshed during Read()
    m_LabelControl->a_ToolTip.RaiseChanged();
}

void LabelWidget::Destroy()
{
    HELIUM_ASSERT( m_LabelWindow );

    SetWindow( NULL );

    // remove listeners
    m_LabelControl->a_ToolTip.Changed().RemoveMethod( this, &LabelWidget::ToolTipChanged );

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

void LabelWidget::ToolTipChanged( const Attribute<tstring>::ChangeArgs& args )
{
    m_LabelWindow->UpdateToolTip( args.m_NewValue );
}