#pragma once

#include "Reflect/Structure.h"
#include "Engine/Asset.h"
#include "Engine/AssetType.h"
#include "Math/Vector3.h"
#include "Framework/ComponentDefinition.h"

namespace ExampleGame
{
	class ExampleComponentDefinition;

	class EXAMPLE_GAME_API ExampleComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::ExampleComponent, Helium::Component );
        static void PopulateStructure( Helium::Reflect::Structure& comp );
		
        void Finalize( const ExampleComponentDefinition *pDefinition);
	};
	
	class EXAMPLE_GAME_API ExampleComponentDefinition : public Helium::ComponentDefinitionHelper<ExampleComponent, ExampleComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( ExampleGame::ExampleComponentDefinition, Helium::ComponentDefinition );
        static void PopulateStructure( Helium::Reflect::Structure& comp );
	};
    typedef Helium::StrongPtr<ExampleComponentDefinition> ExampleComponentDefinitionPtr;
}