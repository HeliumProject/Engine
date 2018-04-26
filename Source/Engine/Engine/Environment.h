
#pragma once

#include "Engine/Engine.h"
#include "Reflect/Object.h"

namespace Helium
{
	class HELIUM_ENGINE_API HeliumEnvironment : public Reflect::Object
	{
		HELIUM_DECLARE_CLASS( Helium::HeliumEnvironment, Reflect::Object);
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		DynamicArray<String> m_Plugins;

		const DynamicArray<String> &GetPlugins() { return m_Plugins; }
	};
}
