#include "Precompile.h"
#include "InspectWidgets.h"

using namespace Helium;
using namespace Helium::Editor;

InspectWindow::InspectWindow( Inspect::Control* control, wxWindow* window )
: m_Control( control )
, m_Window( window )
{
    // the window *owns this object*, we are allocated on realize
    m_Window->SetClientObject( this );

    // when the control goes into the unrealized state, destroy the control (which will free this object)
    m_Control->Unrealized().AddMethod( this, &InspectWindow::Unrealized );

    // configure initial state
    m_Window->Enable( m_Control->IsEnabled() && !m_Control->IsReadOnly() );
    if ( m_Control->IsFrozen() )
    {
        m_Window->Freeze();
    }
    if ( m_Control->IsHidden() )
    {
        m_Window->Hide();
    }   
    m_Window->SetForegroundColour( m_Control->ForegroundColor().Get() );
    m_Window->SetBackgroundColour( m_Control->BackgroundColor().Get() );
    m_Window->SetHelpText( m_Control->ToolTip().Get() );

    // attach listeners
    control->IsEnabled().Changed().AddMethod( this, &InspectWindow::IsEnabledChanged );
    control->IsReadOnly().Changed().AddMethod( this, &InspectWindow::IsReadOnlyChanged );
    control->IsFrozen().Changed().AddMethod( this, &InspectWindow::IsFrozenChanged );
    control->IsHidden().Changed().AddMethod( this, &InspectWindow::IsHiddenChanged );
    control->ForegroundColor().Changed().AddMethod( this, &InspectWindow::ForegroundColorChanged );
    control->BackgroundColor().Changed().AddMethod( this, &InspectWindow::BackgroundColorChanged );
    control->ToolTip().Changed().AddMethod( this, &InspectWindow::ToolTipChanged );
}

void InspectWindow::Unrealized( Inspect::Control* control )
{
    HELIUM_ASSERT( m_Control == control );
    m_Window->Destroy(); // will in turn destroy this object
}

void InspectWindow::IsEnabledChanged( const Attribute<bool>::ChangeArgs& args )
{
    m_Window->Enable(args.m_Value && !m_Control->IsReadOnly().Get());
}

void InspectWindow::IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args )
{
    m_Window->Enable(!args.m_Value && m_Control->IsEnabled().Get());
}

void InspectWindow::IsFrozenChanged( const Attribute<bool>::ChangeArgs& args )
{
    if ( args.m_Value )
    {
        m_Window->Freeze();
    }
    else
    {
        m_Window->Thaw();
    }
}

void InspectWindow::IsHiddenChanged( const Attribute<bool>::ChangeArgs& args )
{
    if ( args.m_Value )
    {
        m_Window->Hide();
    }
    else
    {
        m_Window->Show();
    }
}

void InspectWindow::ForegroundColorChanged( const Attribute<u32>::ChangeArgs& args )
{
    m_Window->SetForegroundColour( args.m_Value );
}

void InspectWindow::BackgroundColorChanged( const Attribute<u32>::ChangeArgs& args )
{
    m_Window->SetBackgroundColour( args.m_Value );
}

void InspectWindow::ToolTipChanged( const Attribute<tstring>::ChangeArgs& args )
{
    m_Window->SetToolTip( args.m_Value );
}