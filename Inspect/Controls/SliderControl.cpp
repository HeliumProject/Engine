#include "InspectPch.h"
#include "Inspect/Controls/SliderControl.h"
#include "Inspect/Canvas.h"

REFLECT_DEFINE_OBJECT( Helium::Inspect::Slider );

using namespace Helium;
using namespace Helium::Inspect;

Slider::Slider()
: a_Min( 0.0f )
, a_Max( 100.0f )
, a_AutoAdjustMinMax( true )
{
    m_ContextMenu = new ContextMenu (this);
    m_ContextMenu->AddItem( TXT( "Set To Default" ), ContextMenuSignature::Delegate(this, &Slider::SetToDefault));
}

bool Slider::Process( const tstring& key, const tstring& value )
{
    if (Base::Process(key, value))
    {
        return true;
    }

    if (key == SLIDER_ATTR_MIN)
    {
        a_Min.Set( static_cast< float >( _tstof( value.c_str() ) ) );
        return true;
    }
    else if (key == SLIDER_ATTR_MAX)
    {
        a_Max.Set( static_cast< float >( _tstof( value.c_str() ) ) );
        return true;
    }

    return false;
}

void Slider::SetToDefault(const ContextMenuEventArgs& event)
{
    event.m_Control->SetDefault();
    event.m_Control->Read();
}
