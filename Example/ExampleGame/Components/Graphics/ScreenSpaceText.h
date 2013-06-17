#pragma once

#include "Reflect/Structure.h"
#include "Math/Point.h"
#include "Engine/Asset.h"
#include "Framework/ComponentDefinition.h"
#include "Foundation/DynamicArray.h"
#include "Framework/TaskScheduler.h"
#include "Framework/Entity.h"

#include "Graphics/Texture2d.h"
#include "Graphics/BufferedDrawer.h"
#include "Graphics/GraphicsManagerComponent.h"

namespace ExampleGame
{
	struct ScreenSpaceTextComponentDefinition;
	
	typedef Helium::StrongPtr<ScreenSpaceTextComponentDefinition> ScreenSpaceTextComponentDefinitionPtr;	
	typedef Helium::StrongPtr<const ScreenSpaceTextComponentDefinition> ConstScreenSpaceTextComponentDefinition;
	
	//////////////////////////////////////////////////////////////////////////
	// ScreenSpaceTextComponent
	class EXAMPLE_GAME_API ScreenSpaceTextComponent : public Helium::Component
	{
	public:
		HELIUM_DECLARE_COMPONENT( ExampleGame::ScreenSpaceTextComponent, Helium::Component );
		static void PopulateStructure( Helium::Reflect::Structure& comp );

		ScreenSpaceTextComponent();
		
		void Initialize( const ScreenSpaceTextComponentDefinition &definition);

		void Render( Helium::GraphicsManagerComponent &rGraphicsManager );
		
	private:
		ConstScreenSpaceTextComponentDefinition m_Definition;
	};
	
	struct EXAMPLE_GAME_API ScreenSpaceTextComponentDefinition : public Helium::ComponentDefinitionHelper<ScreenSpaceTextComponent, ScreenSpaceTextComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( ExampleGame::ScreenSpaceTextComponentDefinition, Helium::ComponentDefinition );
		static void PopulateStructure( Helium::Reflect::Structure& comp );

		ScreenSpaceTextComponentDefinition();
	
		Helium::String m_Text;
		Helium::Simd::Vector2 m_Position;
	};

	struct EXAMPLE_GAME_API DrawScreenSpaceTextTask : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(DrawScreenSpaceTextTask)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
