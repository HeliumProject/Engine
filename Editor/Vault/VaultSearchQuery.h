#pragma once

#include "Platform/Types.h"

#include "Application/OrderedSet.h"
#include "Foundation/SmartPtr.h"
#include "Reflect/Object.h"
#include "Reflect/TranslatorDeduction.h"

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
        typedef Helium::OrderedSet< Helium::DeepCompareSmartPtr< VaultSearchQuery > > OS_VaultSearchQuery;

        class VaultSearchQuery : public Reflect::Object
        {
        public:
            VaultSearchQuery();
            ~VaultSearchQuery();

            bool SetQueryString( const std::string& queryString, std::string& errors );
            const std::string& GetQueryString() const { return m_QueryString; }

            const std::string& GetSQLQueryString() const;

            bool operator<( const VaultSearchQuery& rhs ) const;
            bool operator==( const VaultSearchQuery& rhs ) const;
            bool operator!=( const VaultSearchQuery& rhs ) const;

            static bool ParseQueryString( const std::string& queryString, std::string& errors, VaultSearchQuery* query = NULL );

        public:
            HELIUM_DECLARE_CLASS( VaultSearchQuery, Reflect::Object );
            static void PopulateMetaType( Reflect::MetaStruct& comp );
            virtual void PostDeserialize( const Reflect::Field* field ) HELIUM_OVERRIDE; 

        private:
            std::string           m_QueryString;
            mutable std::string   m_SQLQueryString;
        };
    }
}