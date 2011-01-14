#include "Foundation/Inspect/Controls/ValueControl.h"
#include "Foundation/Inspect/Canvas.h"

using namespace Helium;
using namespace Helium::Inspect;

REFLECT_DEFINE_OBJECT( Inspect::Value );

Value::Value()
: a_Justification( Justifications::Left )
, a_Highlight( false )
{
    m_ContextMenu = new ContextMenu (this);
    m_ContextMenu->AddItem( TXT( "Set To Default" ), ContextMenuSignature::Delegate(this, &Value::SetToDefault));
}

bool Value::Process(const tstring& key, const tstring& value)
{
    if ( Base::Process(key, value) )
    {
        return true;
    }

    if ( key == TEXTBOX_ATTR_JUSTIFY )
    {
        if ( value == TEXTBOX_ATTR_JUSTIFY_LEFT )
        {
            a_Justification.Set( Justifications::Left );
            return true;
        }
        else if ( value == TEXTBOX_ATTR_JUSTIFY_RIGHT )
        {
            a_Justification.Set( Justifications::Right );
            return true;
        }
    }

    return false;
}

void Value::SetDefaultAppearance(bool def)
{
    a_Highlight.Set( def );
}

void Value::SetToDefault(const ContextMenuEventArgs& event)
{
    event.m_Control->SetDefault();
    event.m_Control->Read();
}