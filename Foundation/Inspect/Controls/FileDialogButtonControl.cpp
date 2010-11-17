#include "Foundation/Inspect/Controls/FileDialogButtonControl.h"

using namespace Helium;
using namespace Helium::Inspect;

REFLECT_DEFINE_CLASS( Inspect::FileDialogButton );

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
        wasHandled = __super::Process( key, value );
    }

    return wasHandled;
}
