#include "Precompile.h"
#include "StripCanvasWidget.h"

#include "Editor/FileIconsTable.h"

using namespace Helium;
using namespace Helium::Editor;

StripCanvasWidget::StripCanvasWidget( Inspect::Container* container )
: m_ContainerControl( container )
, m_ContainerWindow( NULL )
, m_StaticText( NULL )
{
    SetControl( container );
}

void StripCanvasWidget::Create( wxWindow* parent )
{
    SetWindow( m_ContainerWindow = new wxPanel( parent, wxID_ANY ) );

    wxSizer* sizer;

    bool childrenAreContainers = true;
    {
        Inspect::V_Control::const_iterator itr = m_ContainerControl->GetChildren().begin();
        Inspect::V_Control::const_iterator end = m_ContainerControl->GetChildren().end();
        for ( ; itr != end; ++itr )
        {
            if ( !(*itr)->HasType( Reflect::GetType< Inspect::Container >() ) )
            {
                childrenAreContainers = false;
                break;
            }
        }
    }

    int spacing = m_ContainerControl->GetCanvas()->GetBorder();

    if ( childrenAreContainers )
    {
        sizer = new wxStaticBoxSizer( wxHORIZONTAL, m_ContainerWindow, m_ContainerControl->a_Name.Get() );
    }
    else
    {
        sizer = new wxBoxSizer( wxHORIZONTAL );
        m_StaticText = new wxStaticText( m_ContainerWindow, wxID_ANY, wxT( "Temp" ) );
        sizer->Add( m_StaticText, 0, wxALIGN_CENTER, 0);
        sizer->AddSpacer( spacing );
    }

    m_ContainerWindow->SetSizer( sizer );
    m_ContainerWindow->Freeze();

    Inspect::V_Control::const_iterator itr = m_ContainerControl->GetChildren().begin();
    Inspect::V_Control::const_iterator end = m_ContainerControl->GetChildren().end();
    for( ; itr != end; ++itr )
    {
        Inspect::Control* c = *itr;

        Inspect::Label* label = Reflect::ObjectCast< Inspect::Label >( c );
        if ( label )
        {
            label->a_Ellipsize.Set( false );
        }

        c->Realize( m_ContainerControl->GetCanvas() );
        sizer->Add( Reflect::AssertCast< Widget >( c->GetWidget() )->GetWindow(), 0, wxALIGN_CENTER );
        sizer->AddSpacer( spacing );
    }

    m_ContainerWindow->SetHelpText( m_ContainerControl->a_HelpText.Get() );

    m_ContainerControl->a_Name.Changed().AddMethod( this, &StripCanvasWidget::NameChanged );
    m_ContainerControl->a_Name.RaiseChanged();

    m_ContainerWindow->Thaw();
}

void StripCanvasWidget::Destroy()
{
    HELIUM_ASSERT( m_ContainerWindow );

    SetWindow( NULL );

    // remove listeners
    m_ContainerControl->a_Name.Changed().RemoveMethod( this, &StripCanvasWidget::NameChanged );

    // destroy window
    m_ContainerWindow->Destroy();
    m_ContainerWindow = NULL;
}

void StripCanvasWidget::NameChanged( const Attribute<tstring>::ChangeArgs& text)
{
    if ( m_StaticText )
    {
        m_StaticText->SetLabel( text.m_NewValue );
        m_StaticText->Layout();
    }
}