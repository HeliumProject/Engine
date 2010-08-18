#pragma once

#include "Core/API.h"

namespace Helium
{
    class CORE_API SettingsManager
    {
    public:
        SettingsManager()
        {
        }

        virtual ~SettingsManager()
        {
        }

        Settings* GetSettings( i32 type ) const
        {
            std::map< i32, Settings* >::iterator itr = m_SettingsMap.find( type );
            if ( itr != m_SettingsMap.end() )
            {
                return (*itr).second;
            }

            return NULL;
        }

        void RegisterSettings( i32 type, Settings* settings )
        {
            HELIUM_ASSERT( m_SettingsMap.find( type ) == m_SettingsMap.end() );
            m_SettingsMap[ type ] = settings;
        }

        void UnregisterSettings( i32 type )
        {
            HELIUM_ASSERT( m_SettingsMap.find( type ) != m_SettingsMap.end() );
            m_SettingsMap.erase( type );
        }

        void UnregisterSettings( Settings* settings )
        {
            bool found = false;
            for ( std::map< i32, Settings* >::iterator itr = m_SettingsMap.begin(), end = m_SettingsMap.end(); itr != end; ++itr )
            {
                if ( (*itr).second == settings )
                {
                    m_SettingsMap.erase( itr );
                    found = true;
                    break;
                }
            }

            HELIUM_ASSERT( found );
        }

    private:
        std::map< i32, Settings* > m_SettingsMap;
    };
}