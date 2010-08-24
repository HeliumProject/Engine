#include "Precompile.h"
#include "Canvas.h"

using namespace Helium;
using namespace Helium::Editor;

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