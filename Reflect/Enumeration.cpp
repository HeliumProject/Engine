#include "ReflectPch.h"
#include "Enumeration.h"

#include "Foundation/Log.h"
#include "Foundation/String.h"

using namespace Helium;
using namespace Helium::Reflect;

EnumerationElement::EnumerationElement()
: m_Value( 0x0 )
{

}

EnumerationElement::EnumerationElement( uint32_t value, const tstring& name, const tstring& helpText )
: m_Value( value )
, m_Name( name )
, m_HelpText( helpText )
{

}

Enumeration::Enumeration()
{

}

Enumeration::~Enumeration()
{

}

void Enumeration::Register() const
{
    Type::Register();

    uint32_t computedSize = 0;
    DynArray< EnumerationElement >::ConstIterator itr = m_Elements.Begin();
    DynArray< EnumerationElement >::ConstIterator end = m_Elements.End();
    for ( ; itr != end; ++itr )
    {
        Log::Debug( TXT( "  Value: %8d, Name: %s\n" ), itr->m_Value, itr->m_Name.c_str() );
    }
}

void Enumeration::Unregister() const
{
    Type::Unregister();
}

void Enumeration::AddElement( uint32_t value, const tstring& name, const tstring& helpText )
{
    EnumerationElement element ( value, name, helpText );

    m_Elements.Add( element );
}

bool Enumeration::IsValid(uint32_t value) const
{
    DynArray< EnumerationElement >::ConstIterator itr = m_Elements.Begin();
    DynArray< EnumerationElement >::ConstIterator end = m_Elements.End();
    for ( ; itr != end; ++itr )
    {
        if ( itr->m_Value == value )
        {
            return true;
        }
    }

    return false;
}

bool Enumeration::GetElementValue(const tstring& name, uint32_t& value) const
{
    DynArray< EnumerationElement >::ConstIterator itr = m_Elements.Begin();
    DynArray< EnumerationElement >::ConstIterator end = m_Elements.End();
    for ( ; itr != end; ++itr )
    {
        if ( itr->m_Name == name )
        {
            value = itr->m_Value;
            return true;
        }
    }

    return false;
}

bool Enumeration::GetElementName(const uint32_t value, tstring& name) const
{
    DynArray< EnumerationElement >::ConstIterator itr = m_Elements.Begin();
    DynArray< EnumerationElement >::ConstIterator end = m_Elements.End();
    for ( ; itr != end; ++itr )
    {
        if ( itr->m_Value == value )
        {
            name = itr->m_Name;
            return true;
        }
    }

    return false;
}

bool Enumeration::GetBitfieldValue(const tstring& str, uint32_t& value) const
{
    if ( str.length() >= 1024 )
    {
        HELIUM_BREAK();
        return false;
    }

    tchar_t tmp[1024];
    CopyString( tmp, str.c_str() );

    std::vector< tstring > strs;

    const tchar_t *token = FindNextToken( tmp, TXT('|') );
    while( token )
    {
        strs.push_back(token);

        /* Get next token: */
        token = FindNextToken( token, TXT('|') );
    }

    return GetBitfieldValue(strs, value);
}

bool Enumeration::GetBitfieldValue(const std::vector< tstring >& strs, uint32_t& value) const
{
    value = 0;

    std::vector< tstring >::const_iterator itr = strs.begin();
    std::vector< tstring >::const_iterator end = strs.end();
    for ( ; itr != end; ++itr )
    {
        uint32_t flags;
        if (GetElementValue(*itr, flags))
        {
            // set the bitfield value
            SetFlags( value, static_cast<uint32_t>(flags) );
        }
    }

    return value == 0 || !strs.empty();
}

bool Enumeration::GetBitfieldString(const uint32_t value, tstring& str) const
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

bool Enumeration::GetBitfieldStrings(const uint32_t value, std::vector< tstring >& strs) const
{
    // search the map
    DynArray< EnumerationElement >::ConstIterator itr = m_Elements.Begin();
    DynArray< EnumerationElement >::ConstIterator end = m_Elements.End();
    for ( ; itr != end; ++itr )
    {
        if ( IsFlagSet( value, itr->m_Value ) )
        {
            strs.push_back( itr->m_Name );
        }
    }

    return value == 0 || !strs.empty();
}
