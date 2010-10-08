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

    try
    {
        Reflect::ArchivePtr archive = Reflect::GetArchive( assetPath );
        archive->Put( texture );
        archive->Close();
        texture->SetSourcePath( assetPath );
    }
    catch( Helium::Exception& )
    {
        delete texture;
        return NULL;
    }

    return texture;
}