#pragma once

#include "Foundation/API.h" 

#include "Foundation/Reflect/Data/DataDeduction.h"

namespace Helium
{
    class FOUNDATION_API SearchableProperties : public Reflect::Object
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
        
    public:
        REFLECT_DECLARE_ABSTRACT( SearchableProperties, Reflect::Object );
        static void AcceptCompositeVisitor( Reflect::Composite& comp );
    };
}
