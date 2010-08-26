#include "Application/Inspect/Controls/InspectButton.h"
#include "Application/Inspect/Controls/InspectContainer.h"
#include "Application/Inspect/Controls/InspectCanvas.h"

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
