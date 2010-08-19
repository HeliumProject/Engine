#pragma once

#include "Core/API.h"

#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Version.h"

namespace Helium
{
    namespace Core
    {
        struct SettingsManagerLoadedArgs {};
        typedef Helium::Signature< void, const SettingsManagerLoadedArgs& > SettingsManagerLoadedSignature;

        class CORE_API SettingsManager : public Reflect::ConcreteInheritor< SettingsManager, Reflect::Element >
        {
        public:
            SettingsManager()
            {
            }

            virtual ~SettingsManager()
            {
            }

            template< class Type >
            Type* GetSettings()
            {
                std::map< i32, Reflect::ElementPtr >::const_iterator itr = m_SettingsMap.find( Type::GetType() );
                if ( itr != m_SettingsMap.end() )
                {
                    return (Type*)(*itr).second;
                }
                else
                {
                    // if we haven't seen this type of settings object before, just new one up
                    Type* newSettings = (Type*) Reflect::Registry::GetInstance()->GetClass( Type::GetType() )->m_Create();
                    HELIUM_ASSERT( newSettings );

                    m_SettingsMap[ type ] = newSettings;
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
            std::map< i32, Reflect::ElementPtr > m_SettingsMap;

        public:
            static void EnumerateClass( Reflect::Compositor< SettingsManager >& comp )
            {
                comp.AddField( &SettingsManager::m_SettingsMap, "m_SettingsMap" );
            }
        };

        typedef Helium::SmartPtr< SettingsManager > SettingsManagerPtr;
    }
}