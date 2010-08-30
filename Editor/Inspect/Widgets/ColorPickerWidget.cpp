#include "Precompile.h"
#include "ColorPickerWidget.h"

#include <wx/panel.h>

using namespace Helium;
using namespace Helium::Editor;

ColorPickerWindow::ColorPickerWindow( wxWindow* parent, ColorPickerWidget* colorPickerWidget )
: m_ColorPickerWidget( colorPickerWidget )
, m_Override( false )
{
    wxSize size( 24, 24 );

    Create( parent, wxID_ANY, wxDefaultPosition, size );

    m_ColorPicker = new ColorPicker( this, wxID_ANY );
    m_ColorPicker->EnableAutoSaveCustomColors();

    m_ColorPicker->SetSize( size );
    m_ColorPicker->SetMinSize( size );
    m_ColorPicker->SetMaxSize( size );

    SetSizer( new wxBoxSizer( wxHORIZONTAL ) );
    wxSizer* sizer = GetSizer();
    sizer->Add( m_ColorPicker );
    sizer->Add( 1, 0, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL );
    Layout();

    Connect( wxID_ANY, wxEVT_COMMAND_COLOURPICKER_CHANGED, wxCommandEventHandler( ColorPickerWindow::OnChanged ) );
}

void ColorPickerWindow::OnChanged( wxCommandEvent& )
{
    m_ColorPickerWidget->GetControl()->Write();
}

ColorPickerWidget::ColorPickerWidget( Inspect::ColorPicker* control )
: Widget( control )
, m_ColorPickerControl( control )
, m_ColorPickerWindow( NULL )
{
}

void ColorPickerWidget::Create( wxWindow* parent )
{
    HELIUM_ASSERT( m_ColorPickerWindow );

    // allocate window and connect common listeners
    SetWindow( m_ColorPickerWindow = new ColorPickerWindow( parent, this ) );

    // init layout metrics
    wxSize size( -1, m_ColorPickerControl->GetCanvas()->GetStdSize( Math::SingleAxes::Y ) );
    m_ColorPickerWindow->SetSize( size );
    m_ColorPickerWindow->SetMinSize( size );
    m_ColorPickerWindow->SetMaxSize( size );

    // add listeners
    m_ColorPickerControl->a_Highlight.Changed().AddMethod( this, &ColorPickerWidget::HighlightChanged );

    // update state of attributes that are not refreshed during Read()
    m_ColorPickerControl->a_Highlight.RaiseChanged();
}

void ColorPickerWidget::Destroy()
{
    HELIUM_ASSERT( m_ColorPickerWindow );

    SetWindow( NULL );

    // remove listeners
    m_ColorPickerControl->a_Highlight.Changed().RemoveMethod( this, &ColorPickerWidget::HighlightChanged );

    // destroy window
    m_ColorPickerWindow->Destroy();
    m_ColorPickerWindow = NULL;
}


void ColorPickerWidget::Read()
{
    HELIUM_ASSERT( m_ColorPickerControl->IsBound() );

    tstring str;
    m_ColorPickerControl->ReadStringData( str );
    tstringstream stream( str );

    if ( m_ColorPickerControl->a_Alpha.Get() )
    {
        Math::Color4 color4;
        stream >> color4;
        m_ColorPickerControl->a_Color4.Set( color4 );
        // Update the UI
        m_ColorPickerWindow->SetColor( m_ColorPickerControl->a_Color4.Get() );
    }
    else
    {
        Math::Color3 color3;
        stream >> color3;
        m_ColorPickerControl->a_Color3.Set( color3 );
        // Update the UI
        m_ColorPickerWindow->SetColor( m_ColorPickerControl->a_Color3.Get() );
    }
}

bool ColorPickerWidget::Write()
{
    HELIUM_ASSERT( m_ColorPickerControl->IsBound() );

    if ( m_ColorPickerControl->a_Alpha.Get() )
    {
        Math::Color4 color;
        m_ColorPickerWindow->GetColor( color );
        m_ColorPickerControl->a_Color4.Set( color );
    }
    else
    {
        Math::Color3 color;
        m_ColorPickerWindow->GetColor( color );
        m_ColorPickerControl->a_Color3.Set( color );
    }

    tstringstream stream;

    if ( m_ColorPickerControl->a_Alpha.Get() )
    {
        stream << m_ColorPickerControl->a_Color4.Get();
    }
    else
    {
        stream << m_ColorPickerControl->a_Color3.Get();
    }

    m_ColorPickerWindow->SetOverride( true );
    bool result = m_ColorPickerControl->WriteStringData( stream.str() );
    m_ColorPickerWindow->SetOverride( false );

    return result;
}

void ColorPickerWidget::IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args )
{
    if ( args.m_NewValue )
    {
        m_ColorPickerWindow->Enable();
    }
    else
    {
        m_ColorPickerWindow->Disable();
    }
}

void ColorPickerWidget::HighlightChanged( const Attribute< bool >::ChangeArgs& args )
{
    if ( args.m_NewValue )
    {
        m_ColorPickerWindow->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ColorPickerWindow->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
    }
    else
    {
        m_ColorPickerWindow->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ColorPickerWindow->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    }
}
