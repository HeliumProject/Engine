#include "Precompile.h"
#include "CheckBoxWidget.h"

#include <wx/checkbox.h>
#include <wx/panel.h>

using namespace Helium;
using namespace Helium::Editor;

CheckBoxWindow::CheckBoxWindow( wxWindow* parent, CheckBoxWidget* checkBoxWidget )
: wxCheckBox( parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCHK_3STATE )
, m_CheckBoxWidget( checkBoxWidget )
, m_Override( false )
{
    Connect( wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CheckBoxWindow::OnChecked ) );
}

void CheckBoxWindow::OnChecked( wxCommandEvent& )
{
    if ( !m_Override )
    {
        m_CheckBoxWidget->GetControl()->Write();
    }
}

CheckBoxWidget::CheckBoxWidget( Inspect::CheckBox* checkBox )
: m_CheckBoxControl( checkBox )
, m_CheckBoxWindow( NULL )
{
    SetControl( checkBox );
}

void CheckBoxWidget::Create( wxWindow* parent )
{
    HELIUM_ASSERT( !m_CheckBoxWindow );

    // allocate window and connect common listeners
    SetWindow( m_CheckBoxWindow = new CheckBoxWindow( parent, this ) );

    // init layout metrics
    wxSize size( m_Control->GetCanvas()->GetDefaultSize( SingleAxes::X ), m_Control->GetCanvas()->GetDefaultSize( SingleAxes::Y ) );
    m_CheckBoxWindow->SetSize( size );
    m_CheckBoxWindow->SetMinSize( size );

    // add listeners
    m_CheckBoxControl->a_Highlight.Changed().AddMethod( this, &CheckBoxWidget::HighlightChanged );

    // update state of attributes that are not refreshed during Read()
    m_CheckBoxControl->a_Highlight.RaiseChanged();
}

void CheckBoxWidget::Destroy()
{
    HELIUM_ASSERT( m_CheckBoxWindow );

    SetWindow( NULL );

    // remove listeners
    m_CheckBoxControl->a_Highlight.Changed().RemoveMethod( this, &CheckBoxWidget::HighlightChanged );

    // destroy window
    m_CheckBoxWindow->Destroy();
    m_CheckBoxWindow = NULL;
}


void CheckBoxWidget::Read()
{
    HELIUM_ASSERT( m_CheckBoxControl->IsBound() );

    tstring text;
    m_CheckBoxControl->ReadStringData( text );

    m_CheckBoxWindow->SetOverride( true );

    if ( text == Inspect::MULTI_VALUE_STRING || text == Inspect::UNDEF_VALUE_STRING )
    {
        m_CheckBoxWindow->SetUndetermined();
    }
    else
    {
        int value = _tstoi( text.c_str() );
        m_CheckBoxWindow->SetValue( value == 1 ? true : false );
    }

    m_CheckBoxWindow->SetOverride( false );
}

bool CheckBoxWidget::Write()
{
    HELIUM_ASSERT( m_CheckBoxControl->IsBound() );

    tstring text = m_CheckBoxWindow->GetValue() ? TXT( "1" ) : TXT( "0" );

    m_CheckBoxWindow->SetOverride( true );
    bool result = m_CheckBoxControl->WriteStringData( text );
    m_CheckBoxWindow->SetOverride( false );

    return result;
}

void CheckBoxWidget::IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args )
{
    if ( args.m_NewValue )
    {
        m_CheckBoxWindow->Enable();
    }
    else
    {
        m_CheckBoxWindow->Disable();
    }
}

void CheckBoxWidget::HighlightChanged( const Attribute< bool >::ChangeArgs& args )
{
    if ( args.m_NewValue )
    {
        m_CheckBoxWindow->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_CheckBoxWindow->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
    }
    else
    {
        m_CheckBoxWindow->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_CheckBoxWindow->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    }
}

