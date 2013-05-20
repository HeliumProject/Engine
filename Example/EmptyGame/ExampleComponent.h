#pragma once

//#include "ExampleGamePch.h"

#include "Reflect/Structure.h"
#include "Engine/Asset.h"
#include "Engine/AssetType.h"
#include "Math/Vector3.h"

namespace EmptyGame
{
	class EMPTY_GAME_API ExampleComponentDefinition : public Helium::Asset
	{
		HELIUM_DECLARE_ASSET( EmptyGame::ExampleComponentDefinition, Helium::Asset );
        static void PopulateStructure( Helium::Reflect::Structure& comp );

		float m_TestValue;
		Helium::StrongPtr<ExampleComponentDefinition> m_TestAssetReference;
	};
    typedef Helium::StrongPtr<ExampleComponentDefinition> ExampleComponentDefinitionPtr;

	class EMPTY_GAME_API ExampleComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( EmptyGame::ExampleComponent, Helium::Component );
        static void PopulateStructure( Helium::Reflect::Structure& comp );
	};
}