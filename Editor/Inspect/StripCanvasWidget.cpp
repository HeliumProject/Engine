#include "EditorPch.h"
#include "StripCanvasWidget.h"

#include "Editor/FileIconsTable.h"
#include "Editor/Inspect/StripCanvas.h"

HELIUM_DEFINE_CLASS( Helium::Editor::StripCanvasWidget );

using namespace Helium;
using namespace Helium::Editor;

StripCanvasWidget::StripCanvasWidget( Inspect::Container* container, int orientation )
: m_ContainerControl( container )
, m_Orientation( orientation )
, m_ContainerWindow( NULL )
, m_StaticText( NULL )
{
    SetControl( container );
}

wxPanel* StripCanvasWidget::GetPanel() const
{
    return m_ContainerWindow;
}

void StripCanvasWidget::SetPanel( wxPanel* panel )
{
    SetWindow( m_ContainerWindow = panel );
}

void StripCanvasWidget::CreateWindow( wxWindow* parent )
{
    SetWindow( m_ContainerWindow = new wxPanel( parent, wxID_ANY ) );

    int spacing = m_ContainerControl->GetCanvas()->GetBorder();
    wxSizer* sizer = NULL;
    if ( !m_ContainerControl->a_Name.Get().empty() )
    {
        sizer = new wxStaticBoxSizer( m_Orientation, m_ContainerWindow, m_ContainerControl->a_Name.Get() );
    }
    else
    {
        sizer = new wxBoxSizer( m_Orientation );
        m_StaticText = new wxStaticText( m_ContainerWindow, wxID_ANY, m_ContainerControl->a_Name.Get() );
        sizer->Add( m_StaticText, 0, wxALIGN_CENTER, 0);
        sizer->AddSpacer( spacing );
    }

    // Add all of the child controls to the container
    m_ContainerWindow->SetSizer( sizer );
    m_ContainerWindow->Freeze();

    std::vector< Inspect::ControlPtr >::const_iterator itr = m_ContainerControl->GetChildren().begin();
    std::vector< Inspect::ControlPtr >::const_iterator end = m_ContainerControl->GetChildren().end();
    for( ; itr != end; ++itr )
    {
        Inspect::Control* control = *itr;

        Inspect::Label* label = Reflect::SafeCast< Inspect::Label >( control );
        if ( label )
        {
            label->a_Ellipsize.Set( false );
        }

        control->Realize( m_ContainerControl->GetCanvas() );
        sizer->Add( Reflect::AssertCast< Widget >( control->GetWidget() )->GetWindow(), 0, wxALIGN_CENTER );
        sizer->AddSpacer( spacing );
    }

    m_ContainerControl->a_Name.Changed().AddMethod( this, &StripCanvasWidget::NameChanged );
    m_ContainerControl->a_Icon.Changed().AddMethod( this, &StripCanvasWidget::IconChanged );

    m_ContainerWindow->SetHelpText( m_ContainerControl->a_HelpText.Get() );

    m_ContainerWindow->Thaw();

    m_ContainerWindow->Layout();
}

void StripCanvasWidget::DestroyWindow()
{
    HELIUM_ASSERT( m_ContainerWindow );

    SetWindow( NULL );

    // remove listeners
    m_ContainerControl->a_Name.Changed().RemoveMethod( this, &StripCanvasWidget::NameChanged );
    m_ContainerControl->a_Icon.Changed().RemoveMethod( this, &StripCanvasWidget::IconChanged );
    
    // destroy window
    m_ContainerWindow->Destroy();
    m_ContainerWindow = NULL;
}

void StripCanvasWidget::NameChanged( const Attribute<std::string>::ChangeArgs& text)
{
    if ( m_StaticText )
    {
        m_StaticText->SetLabel( text.m_NewValue );
        m_StaticText->Layout();
    }
}

void StripCanvasWidget::IconChanged( const Attribute<std::string>::ChangeArgs& icon )
{
}