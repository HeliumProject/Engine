#pragma once

#include "Platform/Types.h"

#include "Foundation/Container/OrderedSet.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

namespace Helium
{
    namespace Editor
    {
        ///////////////////////////////////////////////////////////////////////////////
        /// class VaultSearchQuery
        ///////////////////////////////////////////////////////////////////////////////
        class VaultSearchQuery;
        typedef Helium::StrongPtr< VaultSearchQuery > VaultSearchQueryPtr;
        typedef std::vector< VaultSearchQueryPtr > V_VaultSearchQuery;
        typedef Helium::OrderedSet< Helium::SmartPtrComparator< VaultSearchQuery > > OS_VaultSearchQuery;

        class VaultSearchQuery : public Reflect::Object
        {
        public:
            VaultSearchQuery();
            ~VaultSearchQuery();

            bool SetQueryString( const tstring& queryString, tstring& errors );
            const tstring& GetQueryString() const { return m_QueryString; }

            const tstring& GetSQLQueryString() const;

            bool operator<( const VaultSearchQuery& rhs ) const;
            bool operator==( const VaultSearchQuery& rhs ) const;
            bool operator!=( const VaultSearchQuery& rhs ) const;

            static bool ParseQueryString( const tstring& queryString, tstring& errors, VaultSearchQuery* query = NULL );

        public:
            REFLECT_DECLARE_OBJECT( VaultSearchQuery, Reflect::Object );
            static void AcceptCompositeVisitor( Reflect::Composite& comp );
            virtual void PostDeserialize( const Reflect::Field* field ) HELIUM_OVERRIDE; 

        private:
            tstring           m_QueryString;
            mutable tstring   m_SQLQueryString;
        };
    }
}