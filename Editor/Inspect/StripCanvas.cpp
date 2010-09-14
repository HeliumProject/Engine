#include "Precompile.h"
#include "StripCanvas.h"

#include "Editor/FileIconsTable.h"
#include "Editor/Inspect/StripCanvasWidget.h"

using namespace Helium;
using namespace Helium::Editor;

StripCanvas::StripCanvas( wxPanel* panel )
: Canvas( panel )
, m_Panel( panel )
{
    SetWidgetCreator< StripCanvasWidget, Container >();
}

void StripCanvas::Realize( Inspect::Canvas* canvas )
{
    HELIUM_ASSERT( canvas == this || canvas == NULL );

    SmartPtr< StripCanvasWidget > widget = new StripCanvasWidget( this );
    widget->SetPanel( m_Panel );
    SetWidget( widget );

    int spacing = GetBorder();

    wxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
    m_Panel->SetSizer( sizer );
    sizer->AddSpacer( spacing );

    Inspect::V_Control::const_iterator itr = m_Children.begin();
    Inspect::V_Control::const_iterator end = m_Children.end();
    for( ; itr != end; ++itr )
    {
        Inspect::Control* c = *itr;
        c->Realize( this );
        sizer->Add( Reflect::AssertCast< Widget >( c->GetWidget() )->GetWindow(), 0, 0 );
        sizer->AddSpacer( spacing );
    }

    m_Panel->Freeze();
    {
        Populate();
        Read();
        m_Panel->Layout();
    }    
    m_Panel->Thaw();
}