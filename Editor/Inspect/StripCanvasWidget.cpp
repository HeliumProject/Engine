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
    m_ContainerWindow->Freeze();

    wxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
    m_ContainerWindow->SetSizer( sizer );

    m_StaticText = new wxStaticText( m_ContainerWindow, wxID_ANY, wxT( "Temp" ), wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END );
    sizer->Add( m_StaticText, 1, wxALIGN_CENTER, 0);

    m_ContainerControl->a_Name.Changed().AddMethod( this, &StripCanvasWidget::NameChanged );
    m_ContainerControl->a_Name.RaiseChanged();

    int spacing = m_ContainerControl->GetCanvas()->GetPad();

    Inspect::V_Control::const_iterator itr = m_ContainerControl->GetChildren().begin();
    Inspect::V_Control::const_iterator end = m_ContainerControl->GetChildren().end();
    for( ; itr != end; ++itr )
    {
        Inspect::Control* c = *itr;
        c->Realize( m_ContainerControl->GetCanvas() );

        int proportion = 0;
        int flags = wxALIGN_CENTER_VERTICAL;

        if ( !c->a_IsFixedWidth.Get() )
        {
            proportion = 1;
            flags |= wxEXPAND;
        }

        if ( !c->a_IsFixedHeight.Get() )
        {
            flags |= wxEXPAND;
        }

        sizer->Add( spacing, 0, 0 );
        sizer->Add( Reflect::AssertCast< Widget >( c->GetWidget() )->GetWindow(), proportion, flags );
    }
    sizer->Add(spacing, 0, 0);

    m_Window->SetHelpText( m_ContainerControl->a_HelpText.Get() );

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
    m_StaticText->SetLabel( text.m_NewValue );
}