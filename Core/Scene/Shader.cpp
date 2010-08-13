/*#include "Precompile.h"*/
#include "Shader.h"
#include "Render.h"

#include "Core/Content/Nodes/ContentShader.h"
#include "Foundation/Log.h"
#include "Application/UI/ArtProvider.h"

#include "Core/Scene/Scene.h"

using namespace Helium;
using namespace Helium::Core;

Profile::MemoryPoolHandle g_ShaderTextureMemoryPool;

SCENE_DEFINE_TYPE( Shader );

void Shader::InitializeType()
{
  Reflect::RegisterClassType< Shader >( TXT( "Core::Shader" ) );

  g_ShaderTextureMemoryPool = Profile::Memory::CreatePool( TXT( "Direct3D Texture Data" ) );
}

void Shader::CleanupType()
{
  Reflect::UnregisterClassType< Shader >();
}

Shader::Shader( Core::Scene* scene, Content::Shader* shader ) 
: Core::SceneNode( scene, shader )
, m_Alpha ( false )
, m_BaseTextureSize (0)
, m_BaseTexture ( NULL )
{
#pragma TODO( "load up an appropriate texture file" )
}

i32 Shader::GetImageIndex() const
{
  return Helium::GlobalFileIconsTable().GetIconID( TXT( "shader" ) );
}

tstring Shader::GetApplicationTypeName() const
{
  return TXT( "Shader" );
}

void Shader::Create()
{
  __super::Create();

  if ( m_BaseTexture == NULL )
  {
    LoadTexture();
  }
}

void Shader::Delete()
{
  __super::Delete();

  if ( m_BaseTexture != NULL )
  {
    m_BaseTexture->Release();
    m_BaseTexture = NULL;

    Profile::Memory::Deallocate( g_ShaderTextureMemoryPool, m_BaseTextureSize );
  }
}

bool Shader::GetAlpha() const
{
  return m_Alpha;
}

IDirect3DTexture9* Shader::GetBaseTexture() const
{
  return m_BaseTexture;
}

bool Shader::LoadTexture()
{
#pragma TODO( "reimplement" )
  //Content::Shader* shader = GetPackage< Content::Shader >();
  //HELIUM_ASSERT( shader );

  //const tstring& file = shader->GetBaseTextureFilePath();
  //if (!file.empty())
  //{
  //  if ( FileSystem::Exists( file ) )
  //  {
  //    m_BaseTexture = Core::LoadTexture( m_Scene->GetViewport()->GetDevice(), file, &m_BaseTextureSize, &m_Alpha );

  //    Profile::Memory::Allocate( g_ShaderTextureMemoryPool, m_BaseTextureSize );
  //  }
  //}

  //return m_BaseTexture != NULL;
    return false;
}