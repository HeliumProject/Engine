#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Settings.h"

#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

//TEMP
#include "Project.h"
//TEMP

namespace Helium
{
    struct SettingsManagerLoadedArgs {};
    typedef Helium::Signature< const SettingsManagerLoadedArgs& > SettingsManagerLoadedSignature;

    typedef std::map< Reflect::TypeID, SettingsPtr > M_Settings;

    class PIPELINE_API SettingsManager : public Reflect::Object
    {
    public:
        REFLECT_DECLARE_OBJECT( SettingsManager, Reflect::Object );
        static void PopulateComposite( Reflect::Composite& comp );

        SettingsManager();
        ~SettingsManager();

        const M_Settings& GetSettingsMap()
        {
            return m_SettingsMap;
        }

        template< class Type >
        Type* GetSettings()
        {
            M_Settings::const_iterator itr = m_SettingsMap.find( Reflect::GetClass< Type >() );
            if ( itr != m_SettingsMap.end() )
            {
                return Reflect::ThrowCast< Type >( (*itr).second );
            }
            else
            {
                // if we haven't seen this type of settings object before, just new one up
                Type* newSettings = Reflect::SafeCast< Type >( Reflect::GetClass< Type >()->m_Creator() );
                HELIUM_ASSERT( newSettings );

                m_SettingsMap[ Reflect::GetClass< Type >() ] = newSettings;
                return newSettings;
            }
        }

        template< class Type >
        void UnregisterSettings()
        {
            HELIUM_ASSERT( m_SettingsMap.find( Type::GetType() ) != m_SettingsMap.end() );
            m_SettingsMap.erase( Type::GetType() );
        }

        // removes any unknown settings types
        void Clean();

    private:
        M_Settings m_SettingsMap;

    public:
        TestStructure m_Test;
    };

    typedef Helium::StrongPtr< SettingsManager > SettingsManagerPtr;
}