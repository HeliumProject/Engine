#include "Foundation/Inspect/Controls/ListControl.h"
#include "Foundation/Inspect/Canvas.h"

#include "Foundation/String/Tokenize.h"

using namespace Helium;
using namespace Helium::Inspect;

List::List()
: a_IsSorted( false )
{
    a_IsFixedHeight.Set( true );
}

bool List::Process(const tstring& key, const tstring& value)
{
    if ( Base::Process(key, value) )
    {
        return true;
    }

    if ( key == LIST_ATTR_SORTED )
    {
        if ( value == ATTR_VALUE_TRUE )
        {
            a_IsSorted.Set( true );
            return true;
        }
        else if ( value == ATTR_VALUE_FALSE )
        {
            a_IsSorted.Set( false );
            return true;
        }
    }

    return false;
}
