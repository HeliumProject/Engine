#include "EditorPch.h"
#include "StripCanvas.h"

#include "Editor/FileIconsTable.h"
#include "Editor/Inspect/StripCanvasWidget.h"

HELIUM_DEFINE_CLASS( Helium::Editor::StripCanvas );

using namespace Helium;
using namespace Helium::Editor;

StripCanvas::StripCanvas( int orientation )
: m_Orientation( orientation )
, m_Panel( NULL )
{
    SetWidgetCreator< StripCanvasWidget, Container >();
}

StripCanvas::~StripCanvas()
{
}

wxPanel* StripCanvas::GetPanel() const
{
    return m_Panel;
}

void StripCanvas::SetPanel( wxPanel* panel )
{
    m_Panel = panel;
}

void StripCanvas::Realize( Inspect::Canvas* canvas )
{
    HELIUM_ASSERT( canvas == this || canvas == NULL );

    StrongPtr< StripCanvasWidget > widget = new StripCanvasWidget( this, m_Orientation );
    widget->SetPanel( m_Panel );
    SetWidget( widget );

    int spacing = GetBorder();

    wxSizer* sizer = new wxBoxSizer( m_Orientation );
    m_Panel->SetSizer( sizer );
    sizer->AddSpacer( spacing );

    Inspect::V_Control::const_iterator itr = m_Children.begin();
    Inspect::V_Control::const_iterator end = m_Children.end();
    for( ; itr != end; ++itr )
    {
        Inspect::Control* c = *itr;
        c->Realize( this );
        sizer->Add( Reflect::AssertCast< Widget >( c->GetWidget() )->GetWindow(), 0, wxALIGN_CENTER );
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