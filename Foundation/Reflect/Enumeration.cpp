#include "Enumeration.h"

using namespace Helium::Reflect;

EnumerationElement::EnumerationElement( u32 value, const tstring& name, const tstring& label, const tstring& helpText )
: m_Value( value )
, m_Name( name )
, m_Label( label )
, m_HelpText( helpText )
{
    if ( m_Label.empty() )
    {
        m_Label = m_Name;
    }
}

EnumerationElement::~EnumerationElement()
{

}

EnumerationElement* EnumerationElement::Create( u32 value, const tstring& name, const tstring& label, const tstring& helpText )
{
    return new EnumerationElement( value, name, label, helpText );
}

bool Enumeration::Equals(const Enumeration* rhs) const
{
    if ( m_ShortName != rhs->m_ShortName )
    {
        return false;
    }

    if (m_Elements.size() != rhs->m_Elements.size())
    {
        return false;
    }

    V_EnumerationElement::const_iterator itrA = m_Elements.begin();
    V_EnumerationElement::const_iterator endA = m_Elements.end();
    V_EnumerationElement::const_iterator itrB = rhs->m_Elements.begin();
    V_EnumerationElement::const_iterator endB = rhs->m_Elements.end();
    for ( ; itrA != endA && itrB != endB; ++itrA, ++itrB )
    {
        if ((*itrA)->m_Name != (*itrB)->m_Name)
            return false;

        if ((*itrA)->m_Label != (*itrB)->m_Label)
            return false;

        if ((*itrA)->m_Value != (*itrB)->m_Value)
            return false;
    }

    return true;
}

Enumeration::Enumeration()
{

}

Enumeration::~Enumeration()
{

}

Enumeration* Enumeration::Create()
{
    return new Enumeration();
}

void Enumeration::AddElement( u32 value, const tstring& name, const tstring& label, const tstring& helpText )
{
    HELIUM_ASSERT(m_ElementsByName.find(name) == m_ElementsByName.end());
    HELIUM_ASSERT(m_ElementsByLabel.find( label.empty() ? name : label ) == m_ElementsByLabel.end());

    EnumerationElementPtr elem = EnumerationElement::Create( value, name, label, helpText );

    m_Elements.push_back(elem);
    m_ElementsByName[ name ] = elem;
    m_ElementsByLabel[ label.empty() ? name : label ] = elem;
    m_ElementsByValue[ value ] = elem;
}

bool Enumeration::GetElementValue(const tstring& label, u32& value) const
{
    M_StrEnumerationElement::const_iterator found = m_ElementsByLabel.find(label);

    if (found != m_ElementsByLabel.end())
    {
        value = found->second->m_Value;
        return true;
    }

    found = m_ElementsByName.find(label);

    if (found != m_ElementsByName.end())
    {
        value = found->second->m_Value;
        return true;
    }

    return false;
}

bool Enumeration::GetElementLabel(const u32 value, tstring& label) const
{
    M_ValueEnumerationElement::const_iterator found = m_ElementsByValue.find(value);

    if (found == m_ElementsByValue.end())
    {
        return false;
    }

    label = found->second->m_Label;
    return true;
}

bool Enumeration::GetBitfieldValue(const tstring& str, u32& value) const
{
    if ( str.length() >= 1024 )
    {
        HELIUM_BREAK();
        return false;
    }

    static tchar tmp[1024];
    _tcscpy( tmp, str.c_str() );

    std::vector< tstring > strs;

    tchar seps[] = TXT("|");
    tchar *token = _tcstok( tmp, seps );
    while( token != NULL )
    {
        strs.push_back(token);

        /* Get next token: */
        token = _tcstok( NULL, seps );
    }

    return GetBitfieldValue(strs, value);
}

bool Enumeration::GetBitfieldValue(const std::vector< tstring >& strs, u32& value) const
{
    value = 0;

    std::vector< tstring >::const_iterator itr = strs.begin();
    std::vector< tstring >::const_iterator end = strs.end();
    for ( ; itr != end; ++itr )
    {
        u32 flags;
        if (GetElementValue(*itr, flags))
        {
            // set the bitfield value
            SetFlags( value, static_cast<u32>(flags) );
        }
    }

    return value == 0 || !strs.empty();
}

bool Enumeration::GetBitfieldString(const u32 value, tstring& str) const
{
    bool first = true;

    std::vector< tstring > strs;
    if (!GetBitfieldStrings(value, strs))
    {
        return false;
    }

    if ( strs.empty() )
    {
        return true;
    }

    // search the map
    std::vector< tstring >::const_iterator itr = strs.begin();
    std::vector< tstring >::const_iterator end = strs.end();
    for ( ; itr != end; ++itr )
    {
        if ( !first )
        {
            str += TXT("|");
        }

        first = false;

        str += *itr;
    }

    return !first;
}

bool Enumeration::GetBitfieldStrings(const u32 value, std::vector< tstring >& strs) const
{
    // search the map
    V_EnumerationElement::const_iterator itr = m_Elements.begin();
    V_EnumerationElement::const_iterator end = m_Elements.end();
    for ( ; itr != end; ++itr )
    {
        if ( IsFlagSet( value, (*itr)->m_Value ) )
        {
            strs.push_back((*itr)->m_Label);
        }
    }

    return value == 0 || !strs.empty();
}
