#include "GraphicsPch.h"
#include "Graphics/GraphicsEnumRegistration.h"

#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Enumeration.h"

#include "Graphics/Font.h"
#include "Graphics/Texture.h"
#include "Graphics/GraphicsConfig.h"

void Helium::RegisterGraphicsEnums()
{
    Helium::Reflect::RegisterEnumType< Font::ECompression >( TXT( "Helium::Font::ECompression" ) );
    Helium::Reflect::RegisterEnumType< Texture::ECompression >( TXT( "Helium::Texture::ECompression" ) );
    Helium::Reflect::RegisterEnumType< GraphicsConfig::ETextureFilter >( TXT( "Helium::GraphicsConfig::ETextureFilter" ) );
    Helium::Reflect::RegisterEnumType< GraphicsConfig::EShadowMode >( TXT( "Helium::GraphicsConfig::EShadowMode" ) );
}

void Helium::UnregisterGraphicsEnums()
{
    Helium::Reflect::UnregisterEnumType< Font::ECompression >();
    Helium::Reflect::UnregisterEnumType< Texture::ECompression >();
    Helium::Reflect::UnregisterEnumType< GraphicsConfig::ETextureFilter >();
    Helium::Reflect::UnregisterEnumType< GraphicsConfig::EShadowMode >();
}
