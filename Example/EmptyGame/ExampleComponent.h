#pragma once

#include "Reflect/Structure.h"
#include "Engine/Asset.h"
#include "Math/Vector3.h"
#include "Framework/ComponentDefinition.h"

namespace EmptyGame
{
	class ExampleComponentDefinition;

	class EMPTY_GAME_API ExampleComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( EmptyGame::ExampleComponent, Helium::Component );
        static void PopulateStructure( Helium::Reflect::Structure& comp );
		
        void Finalize( const ExampleComponentDefinition *pDefinition);
	};
	
	class EMPTY_GAME_API ExampleComponentDefinition : public Helium::ComponentDefinitionHelper<ExampleComponent, ExampleComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( EmptyGame::ExampleComponentDefinition, Helium::ComponentDefinition );
        static void PopulateStructure( Helium::Reflect::Structure& comp );

		float m_ExampleValue;
	};
    typedef Helium::StrongPtr<ExampleComponentDefinition> ExampleComponentDefinitionPtr;
}