#include "Application/Inspect/Controls/InspectLabel.h"
#include "Application/Inspect/Controls/InspectContainer.h"
#include "Application/Inspect/InspectData.h"

using namespace Helium::Inspect;

Label::Label()
{
    a_ProportionalWidth.Set( 1.f/3.f );
}

bool Label::Process(const tstring& key, const tstring& value)
{
    bool handled = false;

    if ( Base::Process(key, value) )
    {
        return true;
    }

    if ( key == LABEL_ATTR_TEXT )
    {
        Bind( new StringFormatter<tstring>( new tstring( value ), true ) );
        return true;
    }

    return false;
}
