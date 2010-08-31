#include "Precompile.h"
#include "Canvas.h"

#include "Editor/Inspect/Widgets/LabelWidget.h"

using namespace Helium;
using namespace Helium::Editor;

Canvas::Canvas( wxWindow* window )
: m_Window( window )
{
    m_Window->Connect( m_Window->GetId(), wxEVT_SHOW, wxShowEventHandler( Canvas::OnShow ), NULL, this );
    m_Window->Connect( m_Window->GetId(), wxEVT_LEFT_DOWN, wxMouseEventHandler( Canvas::OnClick ), NULL, this );
    m_Window->Connect( m_Window->GetId(), wxEVT_MIDDLE_DOWN, wxMouseEventHandler( Canvas::OnClick ), NULL, this );
    m_Window->Connect( m_Window->GetId(), wxEVT_RIGHT_DOWN, wxMouseEventHandler( Canvas::OnClick ), NULL, this );
}

void Canvas::OnShow(wxShowEvent& event)
{
    RaiseShow( event.GetShow() );
}

void Canvas::OnClick(wxMouseEvent& event)
{
    m_Window->SetFocus();

    event.Skip();
}

void Canvas::RealizeControl( Inspect::Control* control, Inspect::Control* parent )
{
    Widget* window = Reflect::AssertCast< Widget >( parent->GetWidget() );

    wxWindow* parentWindow = window->GetWindow();

    if ( control->HasType( Reflect::GetType< Inspect::Label >() ) )
    {
        Helium::SmartPtr< LabelWidget > label = new LabelWidget( control );

        label->Create( parentWindow );

        control->SetWidget( label );
    }
    /*
    else (...)
    {

    }
    */
}