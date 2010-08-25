#include "Precompile.h"
#include "CheckBox.h"

#include <wx/checkbox.h>
#include <wx/panel.h>

using namespace Helium;
using namespace Helium::Editor;

CheckBoxPanel::CheckBoxPanel( wxWindow* parent, CheckBoxWidget* checkBoxWidget, int width, int height )
: m_CheckBoxWidget( checkBoxWidget )
, m_Override( false )
{
    wxSize size( width, height );

    Create( parent, wxID_ANY, wxDefaultPosition, size );

    m_CheckBox = new wxCheckBox( this, wxID_ANY, wxEmptyString );

    m_CheckBox->SetSize( size );
    m_CheckBox->SetMinSize( size );
    m_CheckBox->SetMaxSize( size );

    SetSizer( new wxBoxSizer( wxHORIZONTAL ) );
    wxSizer* sizer = GetSizer();
    sizer->Add( m_CheckBox, 0, wxALIGN_CENTER_VERTICAL );
    sizer->Add( 1, 0, 1, wxEXPAND );

    Connect( wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CheckBoxPanel::OnChecked ) );

    Layout();
}

void CheckBoxPanel::OnChecked( wxCommandEvent& )
{
    if ( !m_Override )
    {
        m_CheckBoxWidget->GetControl()->Write();
    }
}

CheckBoxWidget::CheckBoxWidget( Inspect::Control* control )
: Widget( control )
{

}

void CheckBoxWidget::Create( wxWindow* parent )
{
    HELIUM_ASSERT( m_CheckBoxPanel );

    // allocate window and connect common listeners
    SetWindow( m_CheckBoxPanel = new CheckBoxPanel( parent, this ) );

    // init layout metrics
    wxSize size( -1, m_CheckBoxControl->GetCanvas()->GetStdSize( Math::SingleAxes::Y ) );
    m_CheckBoxPanel->SetSize( size );
    m_CheckBoxPanel->SetMinSize( size );
    m_CheckBoxPanel->SetMaxSize( size );

    // add listeners
    m_CheckBoxControl->a_Highlight.Changed().AddMethod( this, &CheckBoxWidget::HighlightChanged );

    // update state of attributes that are not refreshed during Read()
    m_CheckBoxControl->a_Highlight.RaiseChanged();
}

void CheckBoxWidget::Destroy()
{
    HELIUM_ASSERT( m_CheckBoxPanel );

    SetWindow( NULL );

    // remove listeners
    m_CheckBoxControl->a_Highlight.Changed().RemoveMethod( this, &CheckBoxWidget::HighlightChanged );

    // destroy window
    m_CheckBoxPanel->Destroy();
    m_CheckBoxPanel = NULL;
}


void CheckBoxWidget::Read()
{
    HELIUM_ASSERT( m_CheckBoxControl->IsBound() );

    tstring text;
    m_CheckBoxControl->ReadStringData( text );
    int value = _tstoi( text.c_str() );

    m_CheckBoxPanel->SetOverride( true );
    m_CheckBoxPanel->SetValue( value == 1 ? true : false );
    m_CheckBoxPanel->SetOverride( false );
}

bool CheckBoxWidget::Write()
{
    HELIUM_ASSERT( m_CheckBoxControl->IsBound() );

    tstring text = m_CheckBoxPanel->GetValue() ? TXT( "1" ) : TXT( "0" );

    m_CheckBoxPanel->SetOverride( true );
    bool result = m_CheckBoxControl->WriteStringData( text );
    m_CheckBoxPanel->SetOverride( false );

    return result;
}

void CheckBoxWidget::IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args )
{
    if ( args.m_NewValue )
    {
        m_CheckBoxPanel->Enable();
    }
    else
    {
        m_CheckBoxPanel->Disable();
    }
}

void CheckBoxWidget::HighlightChanged( const Attribute< bool >::ChangeArgs& args )
{
    if ( args.m_NewValue )
    {
        m_CheckBoxPanel->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_CheckBoxPanel->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
    }
    else
    {
        m_CheckBoxPanel->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_CheckBoxPanel->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    }
}

