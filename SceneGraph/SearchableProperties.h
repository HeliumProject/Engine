#pragma once

#include <map>

#include "Platform/Types.h"

#include "SceneGraph/API.h"

namespace Helium
{
	class HELIUM_SCENE_GRAPH_API SearchableProperties
	{
	private:
		std::multimap< std::string, std::string > m_StringProperties;

	public:
		SearchableProperties();
		virtual ~SearchableProperties();

		//bool HasData();

		//void SetStringProperties( const std::multimap< std::string, std::string >& stringProperties );
		const std::multimap< std::string, std::string >& GetStringProperties() const;

		void Insert( const std::string& propName, const std::string& value );
		//bool Find( const std::string& propName, std::string& value );
		//bool Find( const std::string& propName, std::set< std::string >& value );
		//void Erase( const std::string& propName );
	};
}
