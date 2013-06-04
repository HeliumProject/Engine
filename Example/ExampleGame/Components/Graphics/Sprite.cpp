#include "ExampleGamePch.h"

#include "ExampleGame/Components/Graphics/Sprite.h"
#include "Reflect/TranslatorDeduction.h"
#include "Engine/ComponentQuery.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// PlayerComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::SpriteComponent, 64);

void SpriteComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void SpriteComponent::Finalize( const SpriteComponentDefinition *pDefinition )
{
	m_Definition.Set( pDefinition );
	m_Texture = pDefinition->GetTexture();
	m_Dirty = true;
}

void ExampleGame::SpriteComponent::Render( Helium::BufferedDrawer &rBufferedDrawer, Helium::TransformComponent &rTransform )
{
	if ( m_Dirty )
	{
		m_Definition->GetUVCoordinates( m_Frame, m_UvTopLeft, m_UvBottomRight );

		if ( m_FlipHorizontal )
		{
			float32_t x_temp = m_UvTopLeft.GetX();
			m_UvTopLeft.SetX( m_UvBottomRight.GetX() );
			m_UvBottomRight.SetX( x_temp );
		}
		
		if ( m_FlipVertical )
		{
			Helium::Swap( m_UvTopLeft.m_y, m_UvBottomRight.m_y );
		}
	}

	Helium::Simd::Matrix44 matrix;
	matrix.TranslateLocal( rTransform.GetPosition() );

	// Was going to do something with component interface?

	rBufferedDrawer.DrawTexturedQuad(
		m_Texture->GetRenderResource2d(),
		matrix,
		m_UvTopLeft,
		m_UvBottomRight);
}

HELIUM_IMPLEMENT_ASSET(ExampleGame::SpriteComponentDefinition, Components, 0);

void ExampleGame::SpriteComponentDefinition::PopulateStructure( Helium::Reflect::Structure& comp )
{
	comp.AddField( &SpriteComponentDefinition::m_Texture, "m_Texture" );
	comp.AddField( &SpriteComponentDefinition::m_TopLeftPixel, "m_TopLeftPixel" );
	comp.AddField( &SpriteComponentDefinition::m_Size, "m_Size" );
	comp.AddField( &SpriteComponentDefinition::m_FramesPerColumn, "m_FramesPerColumn" );
	comp.AddField( &SpriteComponentDefinition::m_FrameCount, "m_FrameCount" );
}

Helium::Point ExampleGame::SpriteComponentDefinition::GetPixelCoordinates( uint32_t frame ) const
{
	if ( !m_FramesPerColumn && m_FrameCount < 2 )
	{
		return m_TopLeftPixel;
	}
	else
	{
		uint32_t rowIndex = frame / m_FramesPerColumn;
		uint32_t columnIndex = frame % m_FramesPerColumn;

		return Point(
			m_TopLeftPixel.x + m_Size.x * columnIndex,
			m_TopLeftPixel.y + m_Size.y * rowIndex);
	}
}

void ExampleGame::SpriteComponentDefinition::GetUVCoordinates( uint32_t frame, Simd::Vector2 &topLeft, Simd::Vector2 &bottomRight ) const
{
	Helium::Point topLeftPixel = GetPixelCoordinates(frame);
	Helium::Point bottomRightPixel  = topLeftPixel + m_Size;

	float textureWidth = static_cast<float>( m_Texture->GetWidth() );
	float textureHeight = static_cast<float>( m_Texture->GetHeight() );

	topLeft.SetX( static_cast<float>( topLeftPixel.x ) / textureWidth );
	topLeft.SetX( static_cast<float>( topLeftPixel.y ) / textureHeight );
	bottomRight.SetX( static_cast<float>( bottomRightPixel.x ) / textureWidth );
	bottomRight.SetX( static_cast<float>( bottomRightPixel.y ) / textureHeight );
}


//class DrawSpritesComponentQuery : public ComponentQuery<TransformComponent, SpriteComponent>
//{
//	DrawSpritesComponentQuery( BufferedDrawer &rBufferedDrawer )
//		: m_BufferedDrawer( rBufferedDrawer )
//	{
//		
//	}
//
//    virtual void HandleTuple( TransformComponent *pTransform, SpriteComponent *pSpriteComponent )
//    {
//        pSpriteComponent->Render(m_BufferedDrawer, pTransform);
//    }
//
//	Helium::BufferedDrawer &m_BufferedDrawer;
//};
//
//void UpdateMeshComponents()
//{
//    UpdateMeshComponentsQuery updateMeshComponents; 
//    updateMeshComponents.Run();
//}

void DrawSprite( SpriteComponent *pShaderComponent, Helium::TransformComponent *pTransformComponent )
{
	// TODO: Make this not use buffered drawer
	BufferedDrawer &pBufferedDrawer = pShaderComponent->GetWorld()->GetBufferedDrawer();
	pShaderComponent->Render( pBufferedDrawer, *pTransformComponent );
};



HELIUM_DEFINE_TASK( DrawSpritesTask, (ForEachWorld< QueryComponents< SpriteComponent, TransformComponent, DrawSprite > >) )

void ExampleGame::DrawSpritesTask::DefineContract( Helium::TaskContract &rContract )
{
	rContract.Fulfills<Helium::StandardDependencies::Render>();
}
