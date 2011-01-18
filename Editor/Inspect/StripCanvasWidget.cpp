#include "Precompile.h"
#include "StripCanvasWidget.h"

#include "Editor/FileIconsTable.h"
#include "Foundation/Flags.h"
#include "Editor/Inspect/StripCanvas.h"

using namespace Helium;
using namespace Helium::Editor;

REFLECT_DEFINE_OBJECT( StripCanvasWidget );

StripCanvasWidget::StripCanvasWidget( Inspect::Container* container )
: m_ContainerControl( container )
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
    int spacing = m_ContainerControl->GetCanvas()->GetBorder();
    wxPanel* containerWindow = NULL;
    wxSizer* sizer = NULL;

    if ( HasFlags<Inspect::UIHints>( m_ContainerControl->GetUIHints(), Inspect::UIHint::Popup ) )
    {
        // Create a drawerWidget


        //SetWindow( m_ContainerWindow = new Drawer( parent, wxID_ANY ) );

        //Drawer* drawer = dynamic_cast< Drawer* >( m_ContainerWindow );       
        //containerWindow = drawer->GetPanel();
        //sizer = drawer->GetPanel()->GetSizer();


        // make a new strip canvas


        // move children out of this and into the new vertical strip canvas
        //loop over Container::GetChildren
        // add them to the new canvas via AddChild
        // remove them from this container using ReleaseChildren
        
        // put the new canvas on the drawer via SetPanel on the Drawer
        drawer->SetPanel( newCanvas );


        drawer->SetLabel( m_ContainerControl->a_Name.Get() );
    }
    else
    {
        SetWindow( m_ContainerWindow = new wxPanel( parent, wxID_ANY ) );
        containerWindow = m_ContainerWindow;

        if ( !m_ContainerControl->a_Name.Get().empty() )
        {
            sizer = new wxStaticBoxSizer( wxHORIZONTAL, m_ContainerWindow, m_ContainerControl->a_Name.Get() );
        }
        else
        {
            sizer = new wxBoxSizer( wxHORIZONTAL );
            m_StaticText = new wxStaticText( m_ContainerWindow, wxID_ANY, m_ContainerControl->a_Name.Get() );
            sizer->Add( m_StaticText, 0, wxALIGN_CENTER, 0);
            sizer->AddSpacer( spacing );
        }

        m_ContainerWindow->SetSizer( sizer );
    }


    // Add all of the child controls to the container
    containerWindow->Freeze();

    Inspect::V_Control::const_iterator itr = m_ContainerControl->GetChildren().begin();
    Inspect::V_Control::const_iterator end = m_ContainerControl->GetChildren().end();
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

        // Check to see if we just added a drawer, and add it to the DrawerManager
        StripCanvasWidget* controlWidget = Reflect::SafeCast< StripCanvasWidget >( control->GetWidget() );
        if ( controlWidget )
        {
            Drawer* drawer = dynamic_cast< Drawer* >( controlWidget->GetWindow() );
            if ( drawer )
            {
                StripCanvas* parentCanvas = Reflect::SafeCast< StripCanvas >( GetControl()->GetCanvas() );
                HELIUM_ASSERT( parentCanvas && parentCanvas->GetDrawerManager() );

                if ( parentCanvas && parentCanvas->GetDrawerManager() )
                {
                    parentCanvas->GetDrawerManager()->AddDrawer( drawer );
                }
            }
        }
    }

    m_ContainerControl->a_Name.Changed().AddMethod( this, &StripCanvasWidget::NameChanged );
    m_ContainerControl->a_Icon.Changed().AddMethod( this, &StripCanvasWidget::IconChanged );

    containerWindow->SetHelpText( m_ContainerControl->a_HelpText.Get() );

    containerWindow->Thaw();
}

void StripCanvasWidget::DestroyWindow()
{
    HELIUM_ASSERT( m_ContainerWindow );

    SetWindow( NULL );

    // remove listeners
    m_ContainerControl->a_Name.Changed().RemoveMethod( this, &StripCanvasWidget::NameChanged );
    m_ContainerControl->a_Icon.Changed().RemoveMethod( this, &StripCanvasWidget::IconChanged );
    
    // Clean up drawers
    Drawer* drawer = dynamic_cast< Drawer* >( m_ContainerWindow );
    if ( drawer )
    {
        StripCanvas* parentCanvas = Reflect::SafeCast< StripCanvas >( GetControl()->GetCanvas() );
        if ( parentCanvas && parentCanvas->GetDrawerManager() )
        {
            parentCanvas->GetDrawerManager()->RemoveDrawer( drawer );
        }
    }

    // destroy window
    m_ContainerWindow->Destroy();
    m_ContainerWindow = NULL;
}

void StripCanvasWidget::NameChanged( const Attribute<tstring>::ChangeArgs& text)
{
    Drawer* drawer = dynamic_cast< Drawer* >( m_ContainerWindow );
    if ( drawer )
    {
        drawer->SetLabel( text.m_NewValue );
        drawer->Layout();
    }

    if ( m_StaticText )
    {
        m_StaticText->SetLabel( text.m_NewValue );
        m_StaticText->Layout();
    }
}

void StripCanvasWidget::IconChanged( const Attribute<tstring>::ChangeArgs& icon )
{
    Drawer* drawer = dynamic_cast< Drawer* >( m_ContainerWindow );
    if ( drawer )
    {
        drawer->SetIcon( icon.m_NewValue );
        drawer->Layout();
    }
}