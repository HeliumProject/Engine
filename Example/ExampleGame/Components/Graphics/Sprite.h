#pragma once

#include "Reflect/Structure.h"
#include "Math/Point.h"
#include "Engine/Asset.h"
#include "Engine/AssetType.h"
#include "Framework/ComponentDefinition.h"
#include "Foundation/DynamicArray.h"
#include "Framework/TaskScheduler.h"
#include "Framework/Entity.h"

#include "Components/TransformComponent.h"
#include "Graphics/Texture2d.h"
#include "Graphics/BufferedDrawer.h"

namespace ExampleGame
{
	class SpriteComponentDefinition;
	
	typedef Helium::StrongPtr<SpriteComponentDefinition> SpriteComponentDefinitionPtr;	
	typedef Helium::StrongPtr<const SpriteComponentDefinition> ConstSpriteComponentDefinition;
	
	//////////////////////////////////////////////////////////////////////////
	// SpriteComponent
	//
	// - Draws 2D sprite based on transform component's state
	class EXAMPLE_GAME_API SpriteComponent : public Helium::Component
	{
	public:
		HELIUM_DECLARE_COMPONENT( ExampleGame::SpriteComponent, Helium::Component );
		static void PopulateStructure( Helium::Reflect::Structure& comp );

		SpriteComponent();
		
		void Finalize( const SpriteComponentDefinition *pDefinition);

		void Render( Helium::BufferedDrawer &rBufferedDrawer, Helium::TransformComponent &rTransform );

		void SetFrame(uint32_t frame) { m_Frame = frame; m_Dirty = true;}
		void SetFlipHorizontal( bool shouldFlip ) { m_FlipHorizontal = shouldFlip; m_Dirty = true; }
		void SetFlipVertical( bool shouldFlip ) { m_FlipVertical = shouldFlip; m_Dirty = true; }
		
	private:
		Helium::Simd::Vector2 m_UvTopLeft;
		Helium::Simd::Vector2 m_UvBottomRight;
		Helium::Simd::Vector3 m_Scale;
		Helium::Simd::Vector3 m_TextureSize; // Local cache of uint values from texture
		ConstSpriteComponentDefinition m_Definition;
		Helium::Texture2dPtr m_Texture;
		float m_Rotation;
		uint32_t m_Frame;
		bool m_FlipHorizontal;
		bool m_FlipVertical;
		bool m_Dirty;
	};
	
	class EXAMPLE_GAME_API SpriteComponentDefinition : public Helium::ComponentDefinitionHelper<SpriteComponent, SpriteComponentDefinition>
	{
	public:
		HELIUM_DECLARE_ASSET( ExampleGame::SpriteComponentDefinition, Helium::ComponentDefinition );
		static void PopulateStructure( Helium::Reflect::Structure& comp );

		SpriteComponentDefinition();

		Helium::Texture2d *GetTexture() const { return m_Texture; }
		const Helium::Point &GetSize() const { return m_FrameSize; }
		uint32_t GetFrameCount() const { return m_FrameCount; }
		float GetRotation() const { return m_Rotation; }
		const Helium::Simd::Vector2 &GetScale() const { return m_Scale; }

		Helium::Point ExampleGame::SpriteComponentDefinition::GetPixelCoordinates( uint32_t frame ) const;
		void GetUVCoordinates(uint32_t frame, Helium::Simd::Vector2 &topLeft, Helium::Simd::Vector2 &bottomRight) const;
	
	private:
		Helium::Simd::Vector2 m_Scale;
		Helium::Texture2dPtr m_Texture;
		Helium::Point m_TopLeftPixel;
		Helium::Point m_FrameSize;
		float m_Rotation;
		uint32_t m_FramesPerColumn;
		uint32_t m_FrameCount;
	};

	struct EXAMPLE_GAME_API DrawSpritesTask : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(DrawSpritesTask)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
