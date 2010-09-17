/*#include "Precompile.h"*/
#include "Shader.h"

#include "Foundation/Log.h"

#include "Core/Scene/Render.h"
#include "Core/Scene/Scene.h"

using namespace Helium;
using namespace Helium::Core;

Profile::MemoryPoolHandle g_ShaderTextureMemoryPool;

REFLECT_DEFINE_ABSTRACT( Shader );

#pragma TODO("Data-hide public reflected fields")

void Shader::EnumerateClass( Reflect::Compositor<Shader>& comp )
{
    comp.AddField( &Shader::m_WrapU,      "m_WrapU" );
    comp.AddField( &Shader::m_WrapV,      "m_WrapV" );
    comp.AddField( &Shader::m_RepeatU,    "m_RepeatU" );
    comp.AddField( &Shader::m_RepeatV,    "m_RepeatV" );
    comp.AddField( &Shader::m_BaseColor,  "m_BaseColor" );
    comp.AddField( &Shader::m_AssetPath,  "m_AssetPath" );
}

void Shader::InitializeType()
{
    Reflect::RegisterClassType< Shader >( TXT( "Shader" ) );

    g_ShaderTextureMemoryPool = Profile::Memory::CreatePool( TXT( "Direct3D Texture Data" ) );
}

void Shader::CleanupType()
{
    Reflect::UnregisterClassType< Shader >();
}

Shader::Shader() 
: m_WrapU( true )
, m_WrapV( true )
, m_RepeatU( 1.0 )
, m_RepeatV( 1.0 )
, m_Alpha( false )
, m_BaseTextureSize( 0 )
, m_BaseTexture( NULL )
{
}

Shader::~Shader()
{

}

i32 Shader::GetImageIndex() const
{
    return -1; // Helium::GlobalFileIconsTable().GetIconID( TXT( "shader" ) );
}

tstring Shader::GetApplicationTypeName() const
{
    return TXT( "Shader" );
}

void Shader::Create()
{
    __super::Create();

#pragma TODO("Load texture data from shader asset")
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
