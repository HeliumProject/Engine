#pragma once

#include "Reflect/MetaStruct.h"
#include "Engine/Asset.h"
#include "Math/Vector3.h"
#include "Framework/ComponentDefinition.h"

namespace EmptyGame
{
	class ExampleComponentDefinition;

	class EMPTY_GAME_API ExampleComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( EmptyGame::ExampleComponent, Helium::Component );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );
		
		void Initialize( const ExampleComponentDefinition &definition);
	};
	
	class EMPTY_GAME_API ExampleComponentDefinition : public Helium::ComponentDefinitionHelper<ExampleComponent, ExampleComponentDefinition>
	{
		HELIUM_DECLARE_CLASS( EmptyGame::ExampleComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		float m_ExampleValue;
	};
	typedef Helium::StrongPtr<ExampleComponentDefinition> ExampleComponentDefinitionPtr;
}