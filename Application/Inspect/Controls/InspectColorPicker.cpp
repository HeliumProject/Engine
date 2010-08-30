#include "Application/Inspect/Controls/InspectColorPicker.h"
#include "Application/Inspect/Controls/InspectContainer.h"

#include <sstream>

using namespace Helium::Inspect;

ColorPicker::ColorPicker()
: a_Highlight( false )
{
    m_ContextMenu = new ContextMenu( this );
    m_ContextMenu->AddItem( TXT( "Set To Default" ), ContextMenuSignature::Delegate( this, &ColorPicker::SetToDefault ) );
}

void ColorPicker::SetDefaultAppearance( bool def ) 
{
    a_Highlight.Set( def );
}

void ColorPicker::SetToDefault( const ContextMenuEventArgs& event )
{
    event.m_Control->SetDefault();
    event.m_Control->Read();
}
