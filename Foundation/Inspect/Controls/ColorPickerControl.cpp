#include "Foundation/Inspect/Controls/ColorPickerControl.h"
#include "Foundation/Inspect/Container.h"

#include <sstream>

using namespace Helium;
using namespace Helium::Inspect;

REFLECT_DEFINE_CLASS( Inspect::ColorPicker );

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
