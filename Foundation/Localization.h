#pragma once

#include "API.h"

#include "Platform/String.h"
#include "Platform/Assert.h"

#include <map>
#include <algorithm>

namespace Helium
{
    namespace Localization
    {
        typedef std::string TableId;
        typedef std::string LanguageId;
        typedef std::string StringId;

        typedef std::map< StringId, tstring > IdToStringMap;
        typedef std::map< LanguageId, IdToStringMap > LanguageToStringTableMap;

        const FOUNDATION_API tstring& GetString( const TableId& tableId, const StringId& stringId, const LanguageId& languageId = "" );

        class FOUNDATION_API Statement
        {
        public:
            Statement( const TableId& tableId = "", const StringId& stringId = "", const LanguageId& languageId = "" )
            {
                Set( tableId, stringId, languageId );
            }

            void Set( const TableId& tableId, const StringId& stringId, const LanguageId& languageId = "" )
            {
                m_Buffer = Localization::GetString( tableId, stringId, languageId );
            }

            template< class Type >
            void ReplaceKey( const tstring& key, const Type& value )
            {
                // this function isn't very effecient, optimize when we see it's necessary
                tstringstream replacement;
                replacement << value;

                tstring escapedKey = tstring( TXT( "<" ) ) + key + TXT( ">" );
                size_t offset = tstring::npos;
                while ( ( offset = m_Buffer.find( escapedKey ) ) != tstring::npos )
                {
                    m_Buffer.replace( offset, escapedKey.length(), replacement.str() );
                }
            }

            const tstring& Get() const
            {
                return m_Buffer;
            }

        private:

            tstring m_Buffer;
        };

        class FOUNDATION_API StringTable
        {
        public:
            StringTable( const TableId& tableId )
                : m_Id( tableId )
            {
            }

            const TableId& GetId() const
            {
                return m_Id;
            }

            void AddString( const LanguageId& languageId, const StringId& stringId, const tstring& string )
            {
                if ( m_Languages.find( languageId ) == m_Languages.end() )
                {
                    IdToStringMap stringTable;
                    m_Languages[ languageId ] = stringTable;
                }

                HELIUM_ASSERT( m_Languages[ languageId ].find( stringId ) == m_Languages[ languageId ].end() );
                m_Languages[ languageId ][ stringId ] = string;
            }
            
            const tstring& GetString( const LanguageId& languageId, const StringId& stringId ) const
            {
                LanguageToStringTableMap::const_iterator languageTable = m_Languages.find( languageId );
                HELIUM_ASSERT( languageTable != m_Languages.end() );

                IdToStringMap::const_iterator string = (*languageTable).second.find( stringId );
                HELIUM_ASSERT( string != (*languageTable).second.end() );

                return (*string).second;
            }

        private:
            TableId m_Id;
            LanguageToStringTableMap m_Languages;
        };

        class FOUNDATION_API Localizer
        {
        public:
            Localizer()
            {
            }

            virtual ~Localizer()
            {
            }

            bool Load()
            {
                return true;
            }

            void SetLanguageId( const LanguageId& languageId )
            {
                m_LanguageId = languageId;
            }
            const LanguageId& GetLanguageId()
            {
                return m_LanguageId;
            }

            void RegisterTable( const StringTable* table )
            {
                if ( m_Tables.find( table->GetId() ) == m_Tables.end() )
                {
                    m_Tables[ table->GetId() ] = table;
                }
                else
                {
                    if ( m_Tables[ table->GetId() ] != table )
                    {
                        m_Tables[ table->GetId() ] = table;
                    }
                }
            }

            const tstring& GetString( const TableId& tableId, const StringId& stringId, const LanguageId& languageId = "" )
            {
                HELIUM_ASSERT( m_Tables.find( tableId ) != m_Tables.end() );
                const StringTable* table = m_Tables[ tableId ];
                return table->GetString( languageId.empty() ? m_LanguageId : languageId, stringId );
            }

        private:
            LanguageId m_LanguageId;
            std::map< TableId, const StringTable* > m_Tables;
        };

        FOUNDATION_API Localizer& GlobalLocalizer();
        FOUNDATION_API void Initialize();
        FOUNDATION_API void Cleanup();
    }
}