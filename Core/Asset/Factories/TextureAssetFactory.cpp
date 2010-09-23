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
    texture->SetPath( path.Filename() ); // we're dropping this guy relative to the data file

    TextureProcessingComponentPtr textureProcessingComponent = new TextureProcessingComponent();
    texture->SetComponent( textureProcessingComponent );

    try
    {
        Reflect::Archive::ToFile( texture, assetPath );
        texture->SetSerializationPath( assetPath );
    }
    catch( Helium::Exception& )
    {
        delete texture;
        return NULL;
    }

    return texture;
}