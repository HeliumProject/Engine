#pragma once

#include "Pipeline/API.h"

#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

namespace Helium
{
    struct SettingsManagerLoadedArgs {};
    typedef Helium::Signature< const SettingsManagerLoadedArgs& > SettingsManagerLoadedSignature;

    typedef std::map< Reflect::TypeID, Reflect::ElementPtr > M_Settings;

    class PIPELINE_API SettingsManager : public Reflect::Element
    {
    public:
        REFLECT_DECLARE_CLASS( SettingsManager, Reflect::Element );

        SettingsManager()
        {
        }

        virtual ~SettingsManager()
        {
        }

        const M_Settings& GetSettingsMap()
        {
            return m_SettingsMap;
        }

        template< class Type >
        Type* GetSettings()
        {
            M_Settings::const_iterator itr = m_SettingsMap.find( Reflect::GetType< Type >() );
            if ( itr != m_SettingsMap.end() )
            {
                return Reflect::TryCast< Type >( (*itr).second );
            }
            else
            {
                // if we haven't seen this type of settings object before, just new one up
                Type* newSettings = Reflect::ObjectCast< Type >( Reflect::Registry::GetInstance()->GetClass( Reflect::GetType< Type >() )->m_Creator() );
                HELIUM_ASSERT( newSettings );

                m_SettingsMap[ Reflect::GetType< Type >() ] = newSettings;
                return newSettings;
            }
        }

        template< class Type >
        void UnregisterSettings()
        {
            HELIUM_ASSERT( m_SettingsMap.find( Type::GetType() ) != m_SettingsMap.end() );
            m_SettingsMap.erase( Type::GetType() );
        }

    private:
        M_Settings m_SettingsMap;

    public:
        static void EnumerateClass( Reflect::Compositor< SettingsManager >& comp )
        {
            comp.AddField( &SettingsManager::m_SettingsMap, "m_SettingsMap" );
        }
    };

    typedef Helium::StrongPtr< SettingsManager > SettingsManagerPtr;
}