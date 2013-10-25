#include "EditorPch.h"
#include "wxCanvas.h"

#include "Platform/Thread.h"

#include "Foundation/Flags.h"

#include "Editor/Inspect/Widgets/ButtonWidget.h"
#include "Editor/Inspect/Widgets/ChoiceWidget.h"
#include "Editor/Inspect/Widgets/CheckBoxWidget.h"
#include "Editor/Inspect/Widgets/ColorPickerWidget.h"
#include "Editor/Inspect/Widgets/DrawerWidget.h"
#include "Editor/Inspect/Widgets/FileDialogButtonWidget.h"
#include "Editor/Inspect/Widgets/LabelWidget.h"
#include "Editor/Inspect/Widgets/ListWidget.h"
#include "Editor/Inspect/Widgets/SliderWidget.h"
#include "Editor/Inspect/Widgets/ValueWidget.h"

HELIUM_DEFINE_CLASS( Helium::Editor::Canvas );

using namespace Helium;
using namespace Helium::Editor;

Canvas::Canvas()
: m_Window( NULL )
, m_DrawerManager( NULL )
{
    SetWidgetCreator< LabelWidget, Inspect::Label >();
    SetWidgetCreator< ValueWidget, Inspect::Value >();
    SetWidgetCreator< SliderWidget, Inspect::Slider >();
    SetWidgetCreator< ChoiceWidget, Inspect::Choice >();
    SetWidgetCreator< CheckBoxWidget, Inspect::CheckBox >();
    SetWidgetCreator< ColorPickerWidget, Inspect::ColorPicker >();
    SetWidgetCreator< ListWidget, Inspect::List >();
    SetWidgetCreator< ButtonWidget, Inspect::Button >();
    SetWidgetCreator< FileDialogButtonWidget, Inspect::FileDialogButton >();
}

Canvas::~Canvas()
{
    Clear();
}

void Canvas::SetWindow( wxWindow* window )
{
    if ( m_Window )
    {
        m_Window->Disconnect( m_Window->GetId(), wxEVT_SHOW, wxShowEventHandler( Canvas::OnShow ), NULL, this );
        m_Window->Disconnect( m_Window->GetId(), wxEVT_LEFT_DOWN, wxMouseEventHandler( Canvas::OnClick ), NULL, this );
        m_Window->Disconnect( m_Window->GetId(), wxEVT_MIDDLE_DOWN, wxMouseEventHandler( Canvas::OnClick ), NULL, this );
        m_Window->Disconnect( m_Window->GetId(), wxEVT_RIGHT_DOWN, wxMouseEventHandler( Canvas::OnClick ), NULL, this );
    }

    m_Window = window;

    if ( m_Window )
    {
        m_Window->Connect( m_Window->GetId(), wxEVT_SHOW, wxShowEventHandler( Canvas::OnShow ), NULL, this );
        m_Window->Connect( m_Window->GetId(), wxEVT_LEFT_DOWN, wxMouseEventHandler( Canvas::OnClick ), NULL, this );
        m_Window->Connect( m_Window->GetId(), wxEVT_MIDDLE_DOWN, wxMouseEventHandler( Canvas::OnClick ), NULL, this );
        m_Window->Connect( m_Window->GetId(), wxEVT_RIGHT_DOWN, wxMouseEventHandler( Canvas::OnClick ), NULL, this );
    }
}

DrawerManager* Canvas::GetDrawerManager() const
{
    return m_DrawerManager;
}

void Canvas::SetDrawerManager( DrawerManager* drawerManager )
{
    m_DrawerManager = drawerManager;
}

void Canvas::OnShow(wxShowEvent& event)
{
    e_Show.Raise( event.IsShown() );
}

void Canvas::OnClick(wxMouseEvent& event)
{
    m_Window->SetFocus();

    event.Skip();
}

void Canvas::RealizeControl( Inspect::Control* control )
{
    HELIUM_ASSERT( Thread::IsMain() );

    if ( this != control )
    {
        WidgetPtr widget;

        if ( control->GetMetaClass() == Reflect::GetMetaClass< Container >()
            && HasFlags<Inspect::UIHints>( Reflect::AssertCast< Container >( control )->GetUIHints(), Inspect::UIHint::Popup ) )
        {
            // Create a drawerWidget
            widget = new DrawerWidget( Reflect::AssertCast< Container >( control ) );
        }
        else
        {
            WidgetCreators::const_iterator found = m_WidgetCreators.find( control->GetMetaClass() );
            HELIUM_ASSERT( found != m_WidgetCreators.end() );
            widget = found->second( control );
        }
        HELIUM_ASSERT( widget );

        // associate the widget with the control
        control->SetWidget( widget );

        // find the window pointer for the parent window
        Inspect::Container* parent = control->GetParent();
        HELIUM_ASSERT( parent );
        Widget* parentWidget = Reflect::AssertCast< Widget >( parent->GetWidget() );
        HELIUM_ASSERT( parentWidget );
        wxWindow* parentWindow = parentWidget->GetWindow();
        HELIUM_ASSERT( parentWindow );

        // this will cause the widget to allocate its corresponding window (since it has the parent pointer)
        widget->CreateWindow( parentWindow );

        DrawerWidget* drawerWidget = Reflect::SafeCast< DrawerWidget >( widget );
        if ( drawerWidget &&  GetDrawerManager() )
        {
            GetDrawerManager()->AddDrawer( drawerWidget->GetDrawer() );
        }
    }
}

void Canvas::UnrealizeControl( Inspect::Control* control )
{
    HELIUM_ASSERT( Thread::IsMain() );

    if ( this != control )
    {
        Inspect::Container* container = Reflect::SafeCast< Inspect::Container >( control );
        if ( container )
        {
            for ( Inspect::V_Control::const_iterator itr = container->GetChildren().begin(), end = container->GetChildren().end(); itr != end; ++itr )
            {
                (*itr)->Unrealize(); // unrealize all our children first
            }
        }

        Widget* widget = Reflect::AssertCast< Widget >( control->GetWidget() );
        HELIUM_ASSERT( widget );

        DrawerWidget* drawerWidget = Reflect::SafeCast< DrawerWidget >( control->GetWidget() );
        if ( drawerWidget &&  GetDrawerManager() )
        {
            GetDrawerManager()->RemoveDrawer( drawerWidget->GetDrawer() );
        }

        widget->DestroyWindow();
        control->SetWidget( NULL );
    }
}
