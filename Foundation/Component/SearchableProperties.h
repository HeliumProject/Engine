#pragma once

#include <map>

#include "Platform/Types.h"

#include "Foundation/API.h" 

namespace Helium
{
    class HELIUM_FOUNDATION_API SearchableProperties
    {
    private:
        std::multimap< tstring, tstring > m_StringProperties;

    public:
        SearchableProperties();
        virtual ~SearchableProperties();

        //bool HasData();

        //void SetStringProperties( const std::multimap< tstring, tstring >& stringProperties );
        const std::multimap< tstring, tstring >& GetStringProperties() const;

        void Insert( const tstring& propName, const tstring& value );
        //bool Find( const tstring& propName, tstring& value );
        //bool Find( const tstring& propName, std::set< tstring >& value );
        //void Erase( const tstring& propName );
    };
}
