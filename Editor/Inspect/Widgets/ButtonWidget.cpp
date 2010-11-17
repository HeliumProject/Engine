#include "Precompile.h"
#include "ButtonWidget.h"

#include <wx/button.h>
#include <wx/panel.h>

using namespace Helium;
using namespace Helium::Editor;

ButtonWindow::ButtonWindow( wxWindow* parent, ButtonWidget* buttonWidget )
: wxPanel( parent )
, m_Sizer( NULL )
, m_Button( NULL )
, m_ButtonWidget( buttonWidget )
{
    m_Sizer = new wxBoxSizer( wxHORIZONTAL );
    SetSizer( m_Sizer );

    Connect( wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ButtonWindow::OnClicked ) );
}

void ButtonWindow::OnClicked( wxCommandEvent& )
{
    m_ButtonWidget->GetControl()->Write();
}

void ButtonWindow::SetIcon( const tstring& icon )
{
    HELIUM_ASSERT( m_Sizer );

    if ( m_Button )
    {
        m_Sizer->Detach( m_Button );
        m_Button->Destroy();
        m_Button = NULL;
    }

    Inspect::Button* buttonControl = Reflect::ObjectCast< Inspect::Button >( m_ButtonWidget->GetControl() );
    HELIUM_ASSERT( buttonControl );
    m_Button = new wxBitmapButton( this, wxID_ANY, wxArtProvider::GetIcon( (wxArtID)buttonControl->a_Icon.Get().c_str() ) );

    m_Sizer->Insert( 0, m_Button, 0, wxALIGN_CENTER_VERTICAL );

    Layout();
}

void ButtonWindow::SetLabel( const tstring& label )
{
    HELIUM_ASSERT( m_Sizer );

    if ( m_Button )
    {
        m_Sizer->Detach( m_Button );
        m_Button->Destroy();
        m_Button = NULL;
    }

    Inspect::Button* buttonControl = Reflect::ObjectCast< Inspect::Button >( m_ButtonWidget->GetControl() );
    HELIUM_ASSERT( buttonControl );
    m_Button = new wxButton( this, wxID_ANY, buttonControl->a_Label.Get().c_str() );

    m_Sizer->Insert( 0, m_Button, 0, wxALIGN_CENTER_VERTICAL );

    Layout();
}

REFLECT_DEFINE_CLASS( ButtonWidget );

ButtonWidget::ButtonWidget( Inspect::Button* button )
: m_ButtonControl( button )
, m_ButtonWindow( NULL )
{
    SetControl( button );
}

void ButtonWidget::Create( wxWindow* parent )
{
    HELIUM_ASSERT( !m_ButtonWindow );

    // allocate window and connect common listeners
    SetWindow( m_ButtonWindow = new ButtonWindow( parent, this ) );

    // init state
    if ( !m_ButtonControl->a_Icon.Get().empty() )
    {
        m_ButtonWindow->SetIcon( m_ButtonControl->a_Icon.Get().c_str() );
    }
    else
    {
        m_ButtonWindow->SetLabel( m_ButtonControl->a_Label.Get().c_str() );
    }

    // init layout metrics
    wxSize size( m_Control->GetCanvas()->GetDefaultSize( SingleAxes::X ), m_Control->GetCanvas()->GetDefaultSize( SingleAxes::Y ) );
    m_ButtonWindow->SetSize( size );
    m_ButtonWindow->SetMinSize( size );

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
 