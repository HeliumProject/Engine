#pragma once

#include "Platform/Types.h"

#include "Foundation/Container/OrderedSet.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Serializers.h"

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        class SearchType
        {
        public:
            enum Enum
            {
                Invalid = -1,
                File = 0,
                Directory,
                CacheDB,
            };

            REFLECT_DECLARE_ENUMERATION( SearchType );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement( File,      TXT( "File" ) );
                info.AddElement( Directory, TXT( "Directory" ) );
                info.AddElement( CacheDB,   TXT( "CacheDB" ) );
            }
        };

        ///////////////////////////////////////////////////////////////////////////////
        /// class VaultSearchQuery
        ///////////////////////////////////////////////////////////////////////////////
        class VaultSearchQuery;
        typedef Helium::StrongPtr< VaultSearchQuery > VaultSearchQueryPtr;
        typedef std::vector< VaultSearchQueryPtr > V_VaultSearchQuery;
        typedef Helium::OrderedSet< Helium::SmartPtrComparator< VaultSearchQuery > > OS_VaultSearchQuery;

        class VaultSearchQuery : public Reflect::Element
        {
        public:
            VaultSearchQuery();
            ~VaultSearchQuery();

            SearchType GetSearchType() const { return m_SearchType; }

            bool SetQueryString( const tstring& queryString, tstring& errors );
            const tstring& GetQueryString() const { return m_QueryString; }

            const tstring& GetSQLQueryString() const;

            const Helium::Path& GetQueryPath()
            {
                return m_QueryPath;
            }

            bool operator<( const VaultSearchQuery& rhs ) const;
            bool operator==( const VaultSearchQuery& rhs ) const;
            bool operator!=( const VaultSearchQuery& rhs ) const;

            static bool ParseQueryString( const tstring& queryString, tstring& errors, VaultSearchQuery* query = NULL );

        public:
            REFLECT_DECLARE_CLASS( VaultSearchQuery, Reflect::Element );
            static void EnumerateClass( Reflect::Compositor<VaultSearchQuery>& comp );
            virtual void PostDeserialize() HELIUM_OVERRIDE; 

        private:
            SearchType        m_SearchType;
            tstring           m_QueryString;
            mutable tstring   m_SQLQueryString;
            Helium::Path      m_QueryPath;
        };
    }
}