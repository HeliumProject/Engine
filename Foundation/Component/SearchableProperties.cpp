#include "SearchableProperties.h"

using namespace Helium;

REFLECT_DEFINE_CLASS( SearchableProperties );
void SearchableProperties::EnumerateClass( Reflect::Compositor< SearchableProperties >& comp )
{
    comp.AddField( &SearchableProperties::m_StringProperties, "m_StringProperties" );
}

SearchableProperties::SearchableProperties()
{
}

SearchableProperties::~SearchableProperties()
{
}

//void SearchableProperties::SetStringProperties( const std::multimap< tstring, tstring >& stringProperties )
//{
//    m_StringProperties = stringProperties;
//}

const std::multimap< tstring, tstring > SearchableProperties::GetStringProperties() const
{
    return m_StringProperties;
}

void SearchableProperties::Insert( const tstring& propName, const tstring& value )
{
    m_StringProperties.insert( std::multimap< tstring, tstring >::value_type( propName, value ) );
}

//bool SearchableProperties::Find( const tstring& propName, tstring& value )
//{
//    bool result = false;
//
//    std::multimap< tstring, tstring >::const_iterator lower = m_StringProperties.lower_bound( propName );
//    std::multimap< tstring, tstring >::const_iterator upper = m_StringProperties.upper_bound( propName );
//
//    if ( lower != upper && lower != m_StringProperties.end() )
//    {
//        value = (*lower).second;
//        result = true;
//    }
//
//    return result;
//}

//bool SearchableProperties::Find( const tstring& propName, std::set< tstring >& value )
//{
//    bool result = false;
//
//    std::multimap< tstring, tstring >::const_iterator lower = m_StringProperties.lower_bound( propName );
//    std::multimap< tstring, tstring >::const_iterator upper = m_StringProperties.upper_bound( propName );
//
//    while ( lower != upper && lower != m_StringProperties.end() )
//    {
//        value.insert( (*lower).second );
//        result = true;
//        ++lower;
//    }
//
//    return result;
//}

//void SearchableProperties::Erase( const tstring& propName )
//{
//    m_StringProperties.erase( propName );
//}

//bool SearchableProperties::HasData()
//{
//    return m_StringProperties.size() > 0;
//}

