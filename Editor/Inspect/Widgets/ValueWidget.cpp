#include "Precompile.h"
#include "ValueWidget.h"

#include <wx/panel.h>
#include <wx/stattext.h>

using namespace Helium;
using namespace Helium::Editor;

BEGIN_EVENT_TABLE(ValueWindow, wxTextCtrl)
EVT_TEXT_ENTER(wxID_ANY, ValueWindow::OnConfirm)
EVT_SET_FOCUS(ValueWindow::OnSetFocus)
EVT_KILL_FOCUS(ValueWindow::OnKillFocus)
EVT_KEY_DOWN(ValueWindow::OnKeyDown)
END_EVENT_TABLE()

ValueWindow::ValueWindow( wxWindow* parent, ValueWidget* valueWidget )
: m_ValueWidget( valueWidget )
, m_Override( false )
{
    i32 flags = wxTE_PROCESS_ENTER;

#pragma TODO("Figure out how to properly support text controls whose read only state can change")
    if ( m_ValueWidget->GetControl()->a_IsReadOnly.Get() )
    {
        flags |= wxTE_READONLY;
    }

    Create( parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, flags );
}

void ValueWindow::OnConfirm(wxCommandEvent& e)
{
    if (!m_Override)
    {
        m_ValueWidget->GetControl()->Write();
    }
}

void ValueWindow::OnSetFocus(wxFocusEvent& e)
{
    SetSelection(0, GetLastPosition());

    e.Skip();
}

void ValueWindow::OnKillFocus(wxFocusEvent& e)
{
    if ( !m_Override && !m_ValueWidget->GetControl()->a_IsReadOnly.Get() )
    {
        m_ValueWidget->Write();
    }
}

void ValueWindow::OnKeyDown(wxKeyEvent& e)
{
    if ( e.ControlDown() && ( ( e.GetKeyCode() == 'A' ) || ( e.GetKeyCode() == 'a' ) ) )
    {
#pragma TODO("Why?")
        SetSelection( -1, -1 );
    }
    else
    {
        e.Skip();
    }
}

ValueWidget::ValueWidget( Inspect::Value* value )
: m_ValueControl( value )
, m_ValueWindow( NULL )
{
    SetControl( value );
}

void ValueWidget::Create( wxWindow* parent )
{
    HELIUM_ASSERT( !m_ValueWindow );

    // allocate window and connect common listeners
    SetWindow( m_ValueWindow = new ValueWindow( parent, this ) );

    // init layout metrics
    wxSize size( m_Control->GetCanvas()->GetDefaultSize( Math::SingleAxes::X ), m_Control->GetCanvas()->GetDefaultSize( Math::SingleAxes::Y ) );
    m_ValueWindow->SetSize( size );
    m_ValueWindow->SetMinSize( size );

    // add listeners
    m_ValueControl->a_Justification.Changed().AddMethod( this, &ValueWidget::JustificationChanged );
    m_ValueControl->a_Highlight.Changed().AddMethod( this, &ValueWidget::HighlightChanged );

    // update state of attributes that are not refreshed during Read()
    m_ValueControl->a_Highlight.RaiseChanged();
}

void ValueWidget::Destroy()
{
    HELIUM_ASSERT( m_ValueWindow );

    SetWindow( NULL );

    // remove listeners
    m_ValueControl->a_Justification.Changed().RemoveMethod( this, &ValueWidget::JustificationChanged );
    m_ValueControl->a_Highlight.Changed().RemoveMethod( this, &ValueWidget::HighlightChanged );

    // destroy window
    m_ValueWindow->Destroy();
    m_ValueWindow = NULL;
}

void ValueWidget::Read()
{
    HELIUM_ASSERT( m_Control->IsBound() );

    tstring text;
    m_ValueControl->ReadStringData( text );

    m_ValueWindow->SetOverride( true );
    m_ValueWindow->SetValue( text.c_str() );
    m_ValueWindow->SetOverride( false );

    switch ( m_ValueControl->a_Justification.Get() )
    {
    case Inspect::Justifications::Left:
        m_ValueWindow->SetInsertionPoint( 0 );
        break;

    case Inspect::Justifications::Right:
        m_ValueWindow->SetInsertionPointEnd();
        break;
    }
}

bool ValueWidget::Write()
{
    HELIUM_ASSERT( m_ValueControl->IsBound() );

    tstring text;
    text = m_ValueWindow->GetValue().c_str();

    m_ValueWindow->SetOverride( true );
    bool result = m_ValueControl->WriteStringData( text );
    m_ValueWindow->SetOverride( false );

    m_ValueWindow->SetSelection( 0, static_cast< long >( text.size() ) );

    return result;
}

void ValueWidget::IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args )
{
#pragma TODO("Can we use the Editable state instead of the read only window style employed above?")
    m_ValueWindow->SetEditable( !args.m_NewValue );
}

void ValueWidget::JustificationChanged( const Attribute<Inspect::Justification>::ChangeArgs& args )
{
    switch ( args.m_NewValue )
    {
    case Inspect::Justifications::Left:
        m_ValueWindow->SetInsertionPoint( 0 );
        break;

    case Inspect::Justifications::Right:
        m_ValueWindow->SetInsertionPointEnd();
        break;
    }
}

void ValueWidget::HighlightChanged( const Attribute<bool>::ChangeArgs& args )
{
    if ( args.m_NewValue )
    {
        m_ValueWindow->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ValueWindow->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
    }
    else
    {
        m_ValueWindow->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

        if ( !m_ValueControl->a_IsReadOnly.Get() )
        {
            m_ValueWindow->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
        }
        else
        {
            m_ValueWindow->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        }
    }
}
