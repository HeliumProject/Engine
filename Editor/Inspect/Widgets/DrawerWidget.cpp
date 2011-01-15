#include "Precompile.h"
#include "DrawerWidget.h"
/*
using namespace Helium;
using namespace Helium::Editor;

REFLECT_DEFINE_OBJECT( DrawerWidget );

DrawerWidget::DrawerWidget( Inspect::Container* container )
: m_Container( container )
, m_Drawer( NULL )
{
    SetControl( container );
}

void DrawerWidget::CreateWindow( wxWindow* parent )
{
    HELIUM_ASSERT( !m_Drawer );

    m_Panel = new wxPanel( parent );
    m_Panel->Hide();
    m_Drawer = new Drawer( parent, m_Panel, wxT( "Drawer" ) );

    SetWindow( m_Drawer->GetButton() );

    // init state
    if ( !m_Container->a_Icon.Get().empty() )
    {
        m_Drawer->SetIcon( m_Container->a_Icon.Get() );
    }
    else
    {
        m_Drawer->SetLabel( m_Container->a_Name.Get() );
    }

    // init layout metrics
    wxSize size( m_Control->GetCanvas()->GetDefaultSize( SingleAxes::X ), m_Control->GetCanvas()->GetDefaultSize( SingleAxes::Y ) );
    m_Drawer->GetButton()->SetSize( size );
    m_Drawer->GetButton()->SetMinSize( size );

    // add listeners
    m_Container->a_Icon.Changed().AddMethod( this, &DrawerWidget::OnIconChanged );
    m_Container->a_Name.Changed().AddMethod( this, &DrawerWidget::OnLabelChanged );
}

void DrawerWidget::DestroyWindow()
{
    HELIUM_ASSERT( m_Drawer );

    SetWindow( NULL );

    // remove listeners
    m_Container->a_Icon.Changed().RemoveMethod( this, &DrawerWidget::OnIconChanged );
    m_Container->a_Name.Changed().RemoveMethod( this, &DrawerWidget::OnLabelChanged );

    // destroy window
    delete m_Drawer;
    m_Drawer = NULL;

    m_Panel->Destroy();
    m_Panel = NULL;
}

Drawer* DrawerWidget::GetDrawer() const
{
    return m_Drawer;
}

void DrawerWidget::SetLabel( const tstring& label )
{
    m_Drawer->SetLabel( label );
}

void DrawerWidget::SetIcon( const tstring& icon )
{
    m_Drawer->SetIcon( icon );
}

void DrawerWidget::OnLabelChanged( const Attribute< tstring >::ChangeArgs& args )
{
    SetLabel( args.m_NewValue );
}

void DrawerWidget::OnIconChanged( const Attribute< tstring >::ChangeArgs& args )
{
    SetIcon( args.m_NewValue );
}
*/