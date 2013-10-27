#include "SceneGraphPch.h"
#include "SearchableProperties.h"

using namespace Helium;

SearchableProperties::SearchableProperties()
{
}

SearchableProperties::~SearchableProperties()
{
}

//void SearchableProperties::SetStringProperties( const std::multimap< std::string, std::string >& stringProperties )
//{
//    m_StringProperties = stringProperties;
//}

const std::multimap< std::string, std::string >& SearchableProperties::GetStringProperties() const
{
	return m_StringProperties;
}

void SearchableProperties::Insert( const std::string& propName, const std::string& value )
{
	m_StringProperties.insert( std::multimap< std::string, std::string >::value_type( propName, value ) );
}

//bool SearchableProperties::Find( const std::string& propName, std::string& value )
//{
//    bool result = false;
//
//    std::multimap< std::string, std::string >::const_iterator lower = m_StringProperties.lower_bound( propName );
//    std::multimap< std::string, std::string >::const_iterator upper = m_StringProperties.upper_bound( propName );
//
//    if ( lower != upper && lower != m_StringProperties.end() )
//    {
//        value = (*lower).second;
//        result = true;
//    }
//
//    return result;
//}

//bool SearchableProperties::Find( const std::string& propName, std::set< std::string >& value )
//{
//    bool result = false;
//
//    std::multimap< std::string, std::string >::const_iterator lower = m_StringProperties.lower_bound( propName );
//    std::multimap< std::string, std::string >::const_iterator upper = m_StringProperties.upper_bound( propName );
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

//void SearchableProperties::Erase( const std::string& propName )
//{
//    m_StringProperties.erase( propName );
//}

//bool SearchableProperties::HasData()
//{
//    return m_StringProperties.size() > 0;
//}

