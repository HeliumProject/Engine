#include "Precompile.h"
#include "DrawerWidget.h"

using namespace Helium;
using namespace Helium::Editor;

REFLECT_DEFINE_OBJECT( DrawerWidget );

DrawerWidget::DrawerWidget( Inspect::Container* container )
: m_ContainerControl( container )
, m_Drawer( NULL )
, m_StripCanvas( wxVERTICAL )
{
    if ( m_ContainerControl )
    {
        // move children out of the container and into the new vertical strip canvas
        for( Inspect::V_Control::const_iterator itr = m_ContainerControl->GetChildren().begin(), end = m_ContainerControl->GetChildren().end(); itr != end; ++itr )
        {
            m_StripCanvas.AddChild( *itr );
        }
        //container->Clear();

        SetControl( m_ContainerControl );
    }
}

void DrawerWidget::CreateWindow( wxWindow* parent )
{
    HELIUM_ASSERT( !m_Drawer );

    SetWindow( m_Drawer = new Drawer( parent, wxID_ANY ) );

    // init state
    if ( !m_ContainerControl->a_Icon.Get().empty() )
    {
        m_Drawer->SetIcon( m_ContainerControl->a_Icon.Get() );
    }
    else
    {
        m_Drawer->SetLabel( m_ContainerControl->a_Name.Get() );
    }

    // Add the m_StripCanvas to the Drawer's panel
    m_StripCanvas.SetPanel( m_Drawer->GetPanel() );
    m_StripCanvas.Realize( NULL );

    wxSize drawerSize( m_Drawer->GetPanel()->GetBestSize() );
    m_Drawer->GetPanel()->SetSize( drawerSize );
    m_Drawer->GetPanel()->SetMinSize( drawerSize );
    m_Drawer->GetPanel()->Layout();

    // add listeners
    m_ContainerControl->a_Icon.Changed().AddMethod( this, &DrawerWidget::OnIconChanged );
    m_ContainerControl->a_Name.Changed().AddMethod( this, &DrawerWidget::OnLabelChanged );
}

void DrawerWidget::DestroyWindow()
{
    HELIUM_ASSERT( m_Drawer );

    SetWindow( NULL );

    // remove listeners
    m_ContainerControl->a_Icon.Changed().RemoveMethod( this, &DrawerWidget::OnIconChanged );
    m_ContainerControl->a_Name.Changed().RemoveMethod( this, &DrawerWidget::OnLabelChanged );

    // destroy window
    delete m_Drawer;
    m_Drawer = NULL;
}

Drawer* DrawerWidget::GetDrawer() const
{
    return m_Drawer;
}

Canvas* DrawerWidget::GetCanvas() const
{
    return (Canvas*)&m_StripCanvas;
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
