#include "FoundationPch.h"
#include "Foundation/Inspect/Controls/FileDialogButtonControl.h"

REFLECT_DEFINE_OBJECT( Helium::Inspect::FileDialogButton );

using namespace Helium;
using namespace Helium::Inspect;

bool FileDialogButton::Process(const tstring& key, const tstring& value)
{
    bool wasHandled = false;

    if ( key == BUTTON_FILEDIALOG_ATTR_FILTER )
    {
        a_Filter.Set( value );
        wasHandled = true;
    }
    else if ( key == BUTTON_FILEDIALOG_ATTR_TITLE )
    {
        a_Caption.Set( value );
        wasHandled = true;
    }
    else
    {
        wasHandled = Base::Process( key, value );
    }

    return wasHandled;
}
