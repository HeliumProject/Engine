#include "EditorPch.h"
#include "ButtonWidget.h"

#include <wx/button.h>
#include <wx/panel.h>

HELIUM_DEFINE_CLASS( Helium::Editor::ButtonWidget );

using namespace Helium;
using namespace Helium::Editor;

ButtonWindow::ButtonWindow( wxWindow* parent, ButtonWidget* buttonWidget )
: wxPanel( parent )
, m_Sizer( NULL )
, m_Button( NULL )
, m_ButtonWidget( buttonWidget )
{
    m_Button = new wxButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );

    m_Sizer = new wxBoxSizer( wxHORIZONTAL );
    SetSizer( m_Sizer );
    m_Sizer->Add( m_Button, 1, wxEXPAND | wxALL, 0 );

    m_Button->Connect( wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ButtonWindow::OnClicked ) );
}

void ButtonWindow::OnClicked( wxCommandEvent& )
{
    m_ButtonWidget->GetControl()->Write();
}

void ButtonWindow::SetIcon( const std::string& icon )
{
    HELIUM_ASSERT( m_Sizer );

    m_Button->SetBitmap( wxArtProvider::GetIcon( (wxArtID)icon.c_str(), wxART_OTHER, wxSize( 16, 16 ) ) );
    wxSize buttonSize( m_Button->GetBestSize() );
    m_Button->SetSize( buttonSize );
    m_Button->SetMinSize( buttonSize );

    Layout();
}

void ButtonWindow::SetLabel( const std::string& label )
{
    HELIUM_ASSERT( m_Sizer );

    m_Button->SetLabel( label );
    wxSize buttonSize( m_Button->GetBestSize() );
    m_Button->SetSize( buttonSize );
    m_Button->SetMinSize( buttonSize );

    Layout();
}

ButtonWidget::ButtonWidget( Inspect::Button* button )
: m_ButtonControl( button )
, m_ButtonWindow( NULL )
{
    SetControl( button );
}

void ButtonWidget::CreateWindow( wxWindow* parent )
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
    //wxSize size( m_Control->GetCanvas()->GetDefaultSize( SingleAxes::X ), m_Control->GetCanvas()->GetDefaultSize( SingleAxes::Y ) );
    //m_ButtonWindow->SetSize( size );
    //m_ButtonWindow->SetMinSize( size );
    //wxSize buttonSize( m_ButtonWindow->GetBestSize() );
    //m_ButtonWindow->SetSize( buttonSize );
    //m_ButtonWindow->SetMinSize( buttonSize );

    // add listeners
    m_ButtonControl->a_Icon.Changed().AddMethod( this, &ButtonWidget::OnIconChanged );
    m_ButtonControl->a_Label.Changed().AddMethod( this, &ButtonWidget::OnLabelChanged );
}

void ButtonWidget::DestroyWindow()
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

void ButtonWidget::OnIconChanged( const Attribute< std::string >::ChangeArgs& args )
{
    SetIcon( args.m_NewValue );
}

void ButtonWidget::OnLabelChanged( const Attribute< std::string >::ChangeArgs& args )
{
    SetLabel( args.m_NewValue );
}

void ButtonWidget::SetIcon( const std::string& icon )
{
    m_ButtonWindow->SetIcon( icon );
}

void ButtonWidget::SetLabel( const std::string& label )
{
    m_ButtonWindow->SetLabel( label );
}
 