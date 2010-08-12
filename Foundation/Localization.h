#pragma once

#include "API.h"
#include "Types.h"

namespace Helium
{
    namespace Localization
    {
        typedef tstring TableId;
        typedef tstring LanguageId;
        typedef tstring StringId;

        typedef std::map< StringId, tstring > IdToStringMap;
        typedef std::map< LanguageId, IdToStringMap > LanguageToStringTableMap;

        class FOUNDATION_API StringTable
        {
        public:

            void AddString( const StringId& stringId, const tstring& string )
            {
                HELIUM_ASSERT( m_StringMap.find( stringId ) == m_StringMap.end() );
                m_StringMap[ stringId ] = string;
            }
            
            void RemoveString( const StringId& stringId )
            {
                HELIUM_ASSERT( m_StringMap.find( stringId ) != m_StringMap.end() );
                m_StringMap.erase( stringId );
            }

        private:
            TableId m_Id;
            LanguageToStringTableMap m_StringMap;
        };

        class PLATFORM_API Localizer
        {
        public:
            Localizer()
                : m_LanguageId( TXT( "english" ) )
            {
            }

            virtual ~Localizer()
            {
            }

            Localizer& GlobalLocalizer()
            {
                return s_GlobalLocalizer;
            }

            bool Load()
            {
                return true;
            }

            void SetLanguageId( const LanguageId& languageId )
            {
                if ( m_LanguageId != languageId )
                {
                    m_LanguageId = languageId;
                    m_StringMap.clear();
                }
            }

            const tstring& GetString( const StringId& stringId )
            {
                HELIUM_ASSERT( m_StringMap.find( stringId ) != m_StringMap.end() );
                return m_StringMap[ stringId ];
            }


        private:
            static Localizer s_GlobalLocalizer;
        };

    }
}