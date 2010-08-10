#include "MeshAssetFactory.h"

#include "Foundation/File/Path.h"

#include "Core/Asset/AssetClass.h"
#include "Core/Asset/Classes/Entity.h"

#include "Core/Asset/Components/MeshProcessingComponent.h"

using namespace Helium;
using namespace Helium::Asset;

AssetClassPtr MeshAssetFactory::Create( const Helium::Path& path )
{
    Helium::Path assetPath = path;
    assetPath.ReplaceExtension( TXT( "entity.nrb" ) );

    if ( assetPath.Exists() )
    {
        return AssetClass::LoadAssetClass( assetPath );
    }

    EntityPtr entity = new Entity();
    entity->SetPath( path.Filename() ); // we're dropping this guy relative to the data file

    MeshProcessingComponentPtr meshProcessingComponent = new MeshProcessingComponent();
    entity->SetComponent( meshProcessingComponent );

    try
    {
        Reflect::Archive::ToFile( entity, assetPath );
        entity->SetSerializationPath( assetPath );
    }
    catch( Helium::Exception& )
    {
        delete entity;
        return NULL;
    }

    return entity;
}