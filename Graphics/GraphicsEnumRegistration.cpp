#include "GraphicsPch.h"
#include "Graphics/GraphicsEnumRegistration.h"

#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Enumeration.h"

#include "Graphics/Font.h"
#include "Graphics/Texture.h"
#include "Graphics/GraphicsConfig.h"

void Lunar::RegisterGraphicsEnums()
{
    Helium::Reflect::RegisterEnumType< Font::ECompression >( TXT( "Lunar::Font::ECompression" ) );
    Helium::Reflect::RegisterEnumType< Texture::ECompression >( TXT( "Lunar::Texture::ECompression" ) );
    Helium::Reflect::RegisterEnumType< GraphicsConfig::ETextureFilter >( TXT( "Lunar::GraphicsConfig::ETextureFilter" ) );
    Helium::Reflect::RegisterEnumType< GraphicsConfig::EShadowMode >( TXT( "Lunar::GraphicsConfig::EShadowMode" ) );
}

void Lunar::UnregisterGraphicsEnums()
{
    Helium::Reflect::UnregisterEnumType< Font::ECompression >();
    Helium::Reflect::UnregisterEnumType< Texture::ECompression >();
    Helium::Reflect::UnregisterEnumType< GraphicsConfig::ETextureFilter >();
    Helium::Reflect::UnregisterEnumType< GraphicsConfig::EShadowMode >();
}
