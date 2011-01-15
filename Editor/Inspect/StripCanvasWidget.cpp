#include "Precompile.h"
#include "StripCanvasWidget.h"

#include "Editor/FileIconsTable.h"
#include "Foundation/Flags.h"

#include "Editor/Controls/Drawer/Drawer.h"

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

void StripCanvasWidget::CreateWindow( wxWindow* parent )
{
    if ( HasFlags<Inspect::UIHints>( m_ContainerControl->GetUIHints(), Inspect::UIHint::Popup ) )
    {
        //sizer = new wxBoxSizer( wxHORIZONTAL );
        // add the DrawerWidget
        // get that sizer, 
        // add all childred to the Drawer's sizer
        //DrawerWidget
        SetWindow( m_ContainerWindow = new Drawer( parent, wxID_ANY ) );

        Drawer* drawer = dynamic_cast< Drawer* >( m_ContainerWindow );
        HELIUM_ASSERT( drawer );
        wxSizer* sizer = drawer->GetPanel()->GetSizer();
    }
    else
    {
        SetWindow( m_ContainerWindow = new wxPanel( parent, wxID_ANY ) );

        wxSizer* sizer;

        int spacing = m_ContainerControl->GetCanvas()->GetBorder();
        if ( !m_ContainerControl->a_Name.Get().empty() )
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
            Inspect::Control* control = *itr;

            Inspect::Label* label = Reflect::SafeCast< Inspect::Label >( control );
            if ( label )
            {
                label->a_Ellipsize.Set( false );
            }

            control->Realize( m_ContainerControl->GetCanvas() );
            sizer->Add( Reflect::AssertCast< Widget >( control->GetWidget() )->GetWindow(), 0, wxALIGN_CENTER );
            sizer->AddSpacer( spacing );


            StripCanvasWidget* stripCanvasWidget = Reflect::SafeCast< StripCanvasWidget >( control->GetWidget() );
            if ( stripCanvasWidget )
            {
                Drawer* drawer = dynamic_cast< Drawer* >( stripCanvasWidget->GetWindow() );
                if ( drawer )
                {
                    //    DrawerManager* drawerManager = dynamic_cast< DrawerManager* >( parent );
                    //    if ( drawerManager )
                    //    {
                    //        drawerManager->AddDrawer( drawer );
                    //    }
                    //}
                }
            }

            m_ContainerWindow->SetHelpText( m_ContainerControl->a_HelpText.Get() );

            m_ContainerControl->a_Name.Changed().AddMethod( this, &StripCanvasWidget::NameChanged );
            m_ContainerControl->a_Name.RaiseChanged();

            m_ContainerWindow->Thaw();
        }

    }
}

void StripCanvasWidget::DestroyWindow()
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