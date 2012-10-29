#include "InspectPch.h"
#include "Inspect/Controls/ButtonControl.h"
#include "Inspect/Container.h"
#include "Inspect/Canvas.h"

REFLECT_DEFINE_OBJECT( Helium::Inspect::Button );

using namespace Helium;
using namespace Helium::Inspect;

Button::Button()
{
}

bool Button::Process( const tstring& key, const tstring& value )
{
    if ( Base::Process(key, value) )
    {
        return true;
    }

    if ( key == BUTTON_ATTR_TEXT )
    {
        a_Label.Set( value );
        return true;
    }

    if ( key == BUTTON_ATTR_ICON )
    {
        a_Icon.Set( value );
        return true;
    }

    return false;
}
