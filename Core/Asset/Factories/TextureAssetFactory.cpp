#include "TextureAssetFactory.h"

#include "Foundation/File/Path.h"

#include "Core/Asset/AssetClass.h"
#include "Core/Asset/Classes/Texture.h"

#include "Core/Asset/Components/TextureProcessingComponent.h"

using namespace Helium;
using namespace Helium::Asset;

AssetClassPtr TextureAssetFactory::Create( const Helium::Path& path )
{
    Helium::Path assetPath = path;
    assetPath.ReplaceExtension( TXT( "texture.hrb" ) );

    if ( assetPath.Exists() )
    {
        return AssetClass::LoadAssetClass( assetPath );
    }

    TexturePtr texture = new Texture();
    texture->SetContentPath( path.Filename() ); // we're dropping this guy relative to the data file

    TextureProcessingComponentPtr textureProcessingComponent = new TextureProcessingComponent();
    texture->SetComponent( textureProcessingComponent );

    if ( !Reflect::ToArchive( assetPath, texture ) )
    {
        delete texture;
        return NULL;
    }

    texture->SetSourcePath( assetPath );

    return texture;
}