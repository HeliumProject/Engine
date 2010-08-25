#include "Application/Inspect/Controls/InspectSlider.h"
#include "Application/Inspect/Controls/InspectCanvas.h"

using namespace Helium;
using namespace Helium::Inspect;

Slider::Slider()
: a_Min( 0.0f )
, a_Max( 100.0f )
, a_AutoAdjustMinMax( true )
{
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
