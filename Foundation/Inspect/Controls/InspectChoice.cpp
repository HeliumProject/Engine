#include "Foundation/Inspect/Controls/InspectChoice.h"
#include "Foundation/Inspect/Controls/InspectContainer.h"

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
    if (__super::Process(key, value))
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
    else if (key == CHOICE_ATTR_ITEM)
    {
        size_t delim = value.find_first_of(CHOICE_ATTR_ITEM_DELIM);

        if (delim != tstring::npos)
        {
            m_Statics.push_back( ChoiceItem (value.substr(0, delim), value.substr(delim+1)) );
        }

        return true;
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
    Clear();

    a_Items.Set( m_Statics );

    e_Populate.Raise( ChoiceArgs (this) );

    e_Enumerate.Raise( ChoiceEnumerateArgs (this, m_Enum) );
}