#include "FoundationPch.h"
#include "Foundation/Inspect/Controls/ListControl.h"
#include "Foundation/Inspect/Canvas.h"

#include "Foundation/String/Tokenize.h"

using namespace Helium;
using namespace Helium::Inspect;

REFLECT_DEFINE_OBJECT( Inspect::List );

List::List()
: a_IsSorted( false )
{
    a_IsFixedHeight.Set( true );

    m_ContextMenu = new ContextMenu (this);
    m_ContextMenu->AddItem( TXT( "Set To Default" ), ContextMenuSignature::Delegate(this, &List::SetToDefault));
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

void List::SetToDefault(const ContextMenuEventArgs& event)
{
  event.m_Control->SetDefault();
  event.m_Control->Read();
}
