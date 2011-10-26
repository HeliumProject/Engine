#include "FoundationPch.h"
#include "Foundation/Inspect/Controls/ChoiceControl.h"
#include "Foundation/Inspect/Container.h"

REFLECT_DEFINE_OBJECT( Helium::Inspect::Choice );

using namespace Helium;
using namespace Helium::Inspect;

Choice::Choice()
: a_Highlight( false )
, a_IsSorted( false )
, a_IsDropDown( false )
, a_EnableAdds( false )
{
    m_ContextMenu = new ContextMenu (this);
    m_ContextMenu->AddItem( TXT( "Set To Default" ), ContextMenuSignature::Delegate(this, &Choice::SetToDefault));
}

bool Choice::Process(const tstring& key, const tstring& value)
{
    if (Base::Process(key, value))
        return true;

    if (key == CHOICE_ATTR_ENUM)
    {
        m_Enum = value;
        return true;
    }
    else if (key == CHOICE_ATTR_SORTED)
    {
        if (value == ATTR_VALUE_TRUE)
        {
            a_IsSorted.Set( true );
            return true;
        }
        else if (value == ATTR_VALUE_FALSE)
        {
            a_IsSorted.Set( false );
            return true;
        }
    }
    else if (key == CHOICE_ATTR_DROPDOWN)
    {
        if (value == ATTR_VALUE_TRUE)
        {
            a_IsDropDown.Set( true );
            return true;
        }
        else if (value == ATTR_VALUE_FALSE)
        {
            a_IsDropDown.Set( false );
            return true;
        }
    }
    else if (key == CHOICE_ATTR_PREFIX)
    {
        m_Prefix = value;
        return true;
    }

    return false;
}

void Choice::SetDefaultAppearance(bool def)
{
    a_Highlight.Set( def );
}

void Choice::SetToDefault(const ContextMenuEventArgs& event)
{
    event.m_Control->SetDefault();
    event.m_Control->Read();
}

bool Choice::Contains( const tstring& data )
{
    if ( IsRealized() )
    {
        std::vector< ChoiceItem >::const_iterator itr = a_Items.Get().begin();
        std::vector< ChoiceItem >::const_iterator end = a_Items.Get().end();
        for ( ; itr != end; ++itr )
        {
            if ( itr->m_Data == data )
            {
                return true;
            }
        }
    }

    return false;
}

void Choice::Clear()
{
    a_Items.Set( std::vector< ChoiceItem > () );
}

void Choice::Populate()
{
    e_Populate.Raise( ChoiceArgs (this) );

    e_Enumerate.Raise( ChoiceEnumerateArgs (this, m_Enum) );
}