#include "Enumeration.h"

using namespace Reflect;

EnumerationElement::EnumerationElement(u32 value, const std::string& name, const std::string& label)
: m_Value (value)
, m_Name (name)
, m_Label (label)
{

}

EnumerationElement::~EnumerationElement()
{

}

EnumerationElement* EnumerationElement::Create(u32 value, const std::string& name, const std::string& label)
{
    return new EnumerationElement( value, name, label );
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

void Enumeration::AddElement(u32 value, const std::string& name, std::string label)
{
    if (label.empty())
    {
        label = name;
    }

    NOC_ASSERT(m_ElementsByName.find(name) == m_ElementsByName.end());
    NOC_ASSERT(m_ElementsByLabel.find(label) == m_ElementsByLabel.end());

    EnumerationElementPtr elem = EnumerationElement::Create(value, name, label);

    m_Elements.push_back(elem);
    m_ElementsByName[ name ] = elem;
    m_ElementsByLabel[ label ] = elem;
    m_ElementsByValue[ value ] = elem;
}

bool Enumeration::GetElementValue(const std::string& label, u32& value) const
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

bool Enumeration::GetElementLabel(const u32 value, std::string& label) const
{
    M_ValueEnumerationElement::const_iterator found = m_ElementsByValue.find(value);

    if (found == m_ElementsByValue.end())
    {
        return false;
    }

    label = found->second->m_Label;
    return true;
}

bool Enumeration::GetBitfieldValue(const std::string& str, u32& value) const
{
    if ( str.length() >= 1024 )
    {
        NOC_BREAK();
        return false;
    }

    static char tmp[1024];
    strcpy( tmp, str.c_str() );

    std::vector< std::string > strs;

    char seps[] = "|";
    char *token = strtok( tmp, seps );
    while( token != NULL )
    {
        strs.push_back(token);

        /* Get next token: */
        token = strtok( NULL, seps );
    }

    return GetBitfieldValue(strs, value);
}

bool Enumeration::GetBitfieldValue(const std::vector< std::string >& strs, u32& value) const
{
    value = 0;

    std::vector< std::string >::const_iterator itr = strs.begin();
    std::vector< std::string >::const_iterator end = strs.end();
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

bool Enumeration::GetBitfieldString(const u32 value, std::string& str) const
{
    bool first = true;

    std::vector< std::string > strs;
    if (!GetBitfieldStrings(value, strs))
    {
        return false;
    }

    if ( strs.empty() )
    {
        return true;
    }

    // search the map
    std::vector< std::string >::const_iterator itr = strs.begin();
    std::vector< std::string >::const_iterator end = strs.end();
    for ( ; itr != end; ++itr )
    {
        if ( !first )
        {
            str += "|";
        }

        first = false;

        str += *itr;
    }

    return !first;
}

bool Enumeration::GetBitfieldStrings(const u32 value, std::vector< std::string >& strs) const
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
