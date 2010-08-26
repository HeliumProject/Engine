#include "Precompile.h"
#include "Button.h"

#include <wx/button.h>
#include <wx/panel.h>

using namespace Helium;
using namespace Helium::Editor;

ButtonWindow::ButtonWindow( wxWindow* parent, ButtonWidget* buttonWidget )
: m_ButtonWidget( buttonWidget )
, m_Button( NULL )
{
    Create( parent, wxID_ANY, wxDefaultPosition );

    Inspect::Button* buttonControl = Reflect::ObjectCast< Inspect::Button >( m_ButtonWidget->GetControl() );
    HELIUM_ASSERT( buttonControl );

    if ( !buttonControl->a_Icon.Get().empty() )
    {
        SetIcon( buttonControl->a_Icon.Get().c_str() );
    }
    else
    {
        SetLabel( buttonControl->a_Label.Get().c_str() );
    }

    SetSizer( new wxBoxSizer( wxHORIZONTAL ) );
    wxSizer* sizer = GetSizer();
    sizer->Add( m_Button, 0, wxALIGN_CENTER_VERTICAL );
    sizer->Add( 1, 0, 1, wxEXPAND );

    Connect( wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ButtonWindow::OnClicked ) );

    Layout();
}

void ButtonWindow::OnClicked( wxCommandEvent& )
{
    m_ButtonWidget->GetControl()->Write();
}

void ButtonWindow::SetIcon( const tstring& icon )
{
    if ( m_Button )
    {
        m_Button->Destroy();
        m_Button = NULL;
    }

    Inspect::Button* buttonControl = Reflect::ObjectCast< Inspect::Button >( m_ButtonWidget->GetControl() );
    HELIUM_ASSERT( buttonControl );
    m_Button = new wxBitmapButton( this, wxID_ANY, wxArtProvider::GetIcon( (wxArtID)buttonControl->a_Icon.Get().c_str() ) );
}

void ButtonWindow::SetLabel( const tstring& label )
{
    if ( m_Button )
    {
        m_Button->Destroy();
        m_Button = NULL;
    }

    Inspect::Button* buttonControl = Reflect::ObjectCast< Inspect::Button >( m_ButtonWidget->GetControl() );
    HELIUM_ASSERT( buttonControl );
    m_Button = new wxButton( this, wxID_ANY, buttonControl->a_Label.Get().c_str() );
}

ButtonWidget::ButtonWidget( Inspect::Button* control )
: Widget( control )
, m_ButtonControl( control )
, m_ButtonWindow( NULL )
{

}

void ButtonWidget::Create( wxWindow* parent )
{
    HELIUM_ASSERT( m_ButtonWindow );

    // allocate window and connect common listeners
    SetWindow( m_ButtonWindow = new ButtonWindow( parent, this ) );

    // init layout metrics
    wxSize size( -1, m_ButtonControl->GetCanvas()->GetStdSize( Math::SingleAxes::Y ) );
    m_ButtonWindow->SetSize( size );
    m_ButtonWindow->SetMinSize( size );
    m_ButtonWindow->SetMaxSize( size );

    // add listeners
    m_ButtonControl->a_Icon.Changed().AddMethod( this, &ButtonWidget::OnIconChanged );
    m_ButtonControl->a_Label.Changed().AddMethod( this, &ButtonWidget::OnLabelChanged );
}

void ButtonWidget::Destroy()
{
    HELIUM_ASSERT( m_ButtonWindow );

    SetWindow( NULL );

    // remove listeners
    m_ButtonControl->a_Icon.Changed().RemoveMethod( this, &ButtonWidget::OnIconChanged );
    m_ButtonControl->a_Label.Changed().RemoveMethod( this, &ButtonWidget::OnLabelChanged );

    // destroy window
    m_ButtonWindow->Destroy();
    m_ButtonWindow = NULL;
}

void ButtonWidget::OnIconChanged( const Attribute< tstring >::ChangeArgs& args )
{
    SetIcon( args.m_NewValue );
}

void ButtonWidget::OnLabelChanged( const Attribute< tstring >::ChangeArgs& args )
{
    SetLabel( args.m_NewValue );
}

void ButtonWidget::SetIcon( const tstring& icon )
{
    m_ButtonWindow->SetIcon( icon );
}

void ButtonWidget::SetLabel( const tstring& label )
{
    m_ButtonWindow->SetLabel( label );
}
 