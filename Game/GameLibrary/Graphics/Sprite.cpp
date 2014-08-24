#include "GameLibraryPch.h"

#include "Game/GameLibrary/Graphics/Sprite.h"
#include "Reflect/TranslatorDeduction.h"
#include "Framework/ComponentQuery.h"
#include "Graphics/BufferedDrawer.h"
#include "Graphics/GraphicsManagerComponent.h"
#include "Framework/World.h"

using namespace Helium;
using namespace GameLibrary;

//////////////////////////////////////////////////////////////////////////
// PlayerComponent

HELIUM_DEFINE_COMPONENT(GameLibrary::SpriteComponent, 64);

void SpriteComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

GameLibrary::SpriteComponent::SpriteComponent()
	: m_Frame( 0 )
	, m_FlipHorizontal( false )
	, m_FlipVertical( false )
	, m_Dirty( false )
	, m_Rotation( 0.0f )
	, m_Scale( Simd::Vector3::Unit )
	, m_TextureSize( Simd::Vector3::Zero )
{

}

void SpriteComponent::Initialize( const SpriteComponentDefinition &definition )
{
	m_Definition.Set( &definition );
	m_Texture = definition.GetTexture();
	HELIUM_ASSERT( m_Texture.Get() );
	m_TextureSize = Simd::Vector3( static_cast<float>(m_Texture->GetWidth()), static_cast<float>(m_Texture->GetHeight()), 1.0f );
	m_Scale = Simd::Vector3( definition.GetScale().GetX(), definition.GetScale().GetY(), 1.0f );
	m_Rotation = definition.GetRotation();
	m_Dirty = true;
}

void GameLibrary::SpriteComponent::Render( Helium::BufferedDrawer &rBufferedDrawer, Helium::TransformComponent &rTransform )
{
	if ( !m_Texture )
	{
		return;
	}

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
			Helium::Swap( m_UvTopLeft.GetElement(1), m_UvBottomRight.GetElement(1) );
		}

		m_Dirty = false;
	}
	
	// Not really sure why I had to split this into two matrices but it works
	Helium::Simd::Matrix44 matrix(
		Helium::Simd::Matrix44::INIT_ROTATION_TRANSLATION, 
		rTransform.GetRotation() * Simd::Quat(0.0f, 0.0f, m_Rotation),
		rTransform.GetPosition());
	
	Helium::Simd::Matrix44 scaling(
		Helium::Simd::Matrix44::INIT_SCALING, 
		m_TextureSize * m_Scale);
	
	Helium::Simd::Matrix44 composite =
		scaling * matrix;

	rBufferedDrawer.DrawTexturedQuad(
		m_Texture->GetRenderResource2d(),
		composite,
		m_UvTopLeft,
		m_UvBottomRight);
}

HELIUM_DEFINE_CLASS(GameLibrary::SpriteComponentDefinition);

void GameLibrary::SpriteComponentDefinition::PopulateMetaType( Helium::Reflect::MetaStruct& comp )
{
	comp.AddField( &SpriteComponentDefinition::m_Rotation, "m_Rotation" );
	comp.AddField( &SpriteComponentDefinition::m_Scale, "m_Scale" );
	comp.AddField( &SpriteComponentDefinition::m_TopLeftPixel, "m_TopLeftPixel" );
	comp.AddField( &SpriteComponentDefinition::m_FrameSize, "m_FrameSize" );
	comp.AddField( &SpriteComponentDefinition::m_FramesPerColumn, "m_FramesPerColumn" );
	comp.AddField( &SpriteComponentDefinition::m_FrameCount, "m_FrameCount" );
	comp.AddField( &SpriteComponentDefinition::m_Texture, "m_Texture" );
}

Helium::Point GameLibrary::SpriteComponentDefinition::GetPixelCoordinates( uint32_t frame ) const
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
			m_TopLeftPixel.x + m_FrameSize.x * columnIndex,
			m_TopLeftPixel.y + m_FrameSize.y * rowIndex);
	}
}

void GameLibrary::SpriteComponentDefinition::GetUVCoordinates( uint32_t frame, Simd::Vector2 &topLeft, Simd::Vector2 &bottomRight ) const
{
	Helium::Point topLeftPixel = GetPixelCoordinates(frame);

	if (m_FrameSize.x == 0 || m_FrameSize.y == 0)
	{
		topLeft = Simd::Vector2::Zero;
		bottomRight = Simd::Vector2::Unit;
		return;
	}

	Helium::Point bottomRightPixel  = topLeftPixel + m_FrameSize;

	Texture2d *t2d = Reflect::AssertCast<Texture2d>( m_Texture );

	float textureWidth = static_cast<float>( t2d->GetWidth() );
	float textureHeight = static_cast<float>( t2d->GetHeight() );

	topLeft.SetX( static_cast<float>( topLeftPixel.x ) / textureWidth );
	topLeft.SetY( static_cast<float>( topLeftPixel.y ) / textureHeight );
	bottomRight.SetX( static_cast<float>( bottomRightPixel.x ) / textureWidth );
	bottomRight.SetY( static_cast<float>( bottomRightPixel.y ) / textureHeight );
}

GameLibrary::SpriteComponentDefinition::SpriteComponentDefinition()
	: m_Scale(Simd::Vector2::Unit)
	, m_TopLeftPixel(Point::Zero)
	, m_FrameSize(Point::Zero)
	, m_Rotation(0.0f)
	, m_FramesPerColumn(1)
	, m_FrameCount(1)
{

}

static BufferedDrawer *g_pBufferedDrawer;

void DrawSprite( SpriteComponent *pShaderComponent, Helium::TransformComponent *pTransformComponent )
{
	// TODO: Make this not use buffered drawer
	pShaderComponent->Render( *g_pBufferedDrawer, *pTransformComponent );
};

void DrawSprites( World *pWorld )
{
#if !GRAPHICS_SCENE_BUFFERED_DRAWER
	HELIUM_ASSERT( 0 );
#else // GRAPHICS_SCENE_BUFFERED_DRAWER

	GraphicsManagerComponent *pGraphicsManager = pWorld->GetComponents().GetFirst<GraphicsManagerComponent>();
	HELIUM_ASSERT( pGraphicsManager );

	g_pBufferedDrawer = &pGraphicsManager->GetBufferedDrawer();
	QueryComponents< SpriteComponent, TransformComponent, DrawSprite >( pWorld );
#endif
}


HELIUM_DEFINE_TASK( DrawSpritesTask, (ForEachWorld< DrawSprites >), TickTypes::Render )

void GameLibrary::DrawSpritesTask::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecutesWithin<Helium::StandardDependencies::Render>();
}
