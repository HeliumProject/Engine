#include "MeshAssetFactory.h"

#include "Foundation/File/Path.h"

#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/Classes/Entity.h"

#include "Pipeline/Asset/Components/MeshProcessingComponent.h"

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