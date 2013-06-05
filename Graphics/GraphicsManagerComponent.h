
#pragma once

#include "Components/Components.h"
#include "Framework/ComponentDefinition.h"
#include "Graphics/GraphicsScene.h"

namespace Helium
{
	class GraphicsManagerComponentDefinition;

	class GraphicsScene;
	class BufferedDrawer;
	typedef Helium::StrongPtr< GraphicsScene > GraphicsScenePtr;

	class HELIUM_GRAPHICS_API GraphicsManagerComponent : public Component
	{
		HELIUM_DECLARE_COMPONENT( Helium::GraphicsManagerComponent, Helium::Component );
		static void PopulateStructure( Reflect::Structure& comp );

		void Finalize( const GraphicsManagerComponentDefinition *pDefinition);

	public:
		inline GraphicsScene*  GetGraphicsScene() const;
		void                   UpdateGraphicsScene();

#if GRAPHICS_SCENE_BUFFERED_DRAWER
		inline BufferedDrawer& GetBufferedDrawer();
#endif // GRAPHICS_SCENE_BUFFERED_DRAWER

	private:
		/// Graphics scene instance.
		GraphicsScenePtr m_spGraphicsScene;
	};

	class HELIUM_GRAPHICS_API GraphicsManagerComponentDefinition : public Helium::ComponentDefinitionHelper<GraphicsManagerComponent, GraphicsManagerComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( Helium::GraphicsManagerComponentDefinition, Helium::ComponentDefinition );
		static void PopulateStructure( Reflect::Structure& comp );

		GraphicsManagerComponentDefinition();
	};
	typedef StrongPtr<GraphicsManagerComponentDefinition> GraphicsManagerComponentDefinitionPtr;
}

#include "Graphics/GraphicsManagerComponent.inl"
