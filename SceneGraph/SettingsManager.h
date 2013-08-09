#pragma once

#include "Reflect/Object.h"
#include "Reflect/TranslatorDeduction.h"

#include "SceneGraph/API.h"

namespace Helium
{
	class HELIUM_SCENE_GRAPH_API Settings : public Reflect::Object
	{
	public:
		Settings()
		{
		}

		virtual ~Settings()
		{
		}

		virtual bool UserVisible()
		{
			return true;
		}

		REFLECT_DECLARE_CLASS( Settings, Reflect::Object );
	};

	typedef Helium::StrongPtr< Settings > SettingsPtr;

	struct SettingsManagerLoadedArgs {};
	typedef Helium::Signature< const SettingsManagerLoadedArgs& > SettingsManagerLoadedSignature;

	typedef std::map< const Reflect::MetaClass*, SettingsPtr > M_Settings;

	class HELIUM_SCENE_GRAPH_API SettingsManager : public Reflect::Object
	{
	public:
		REFLECT_DECLARE_CLASS( SettingsManager, Reflect::Object );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		SettingsManager();
		~SettingsManager();

		const M_Settings& GetSettingsMap()
		{
			return m_SettingsMap;
		}

		template< class Type >
		Type* GetSettings()
		{
			M_Settings::const_iterator itr = m_SettingsMap.find( Reflect::GetMetaClass< Type >() );
			if ( itr != m_SettingsMap.end() )
			{
				return Reflect::ThrowCast< Type >( (*itr).second );
			}
			else
			{
				// if we haven't seen this type of settings object before, just new one up
				Type* newSettings = Reflect::SafeCast< Type >( Reflect::GetMetaClass< Type >()->m_Creator() );
				HELIUM_ASSERT( newSettings );

				m_SettingsMap[ Reflect::GetMetaClass< Type >() ] = newSettings;
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
	};

	typedef Helium::StrongPtr< SettingsManager > SettingsManagerPtr;
}