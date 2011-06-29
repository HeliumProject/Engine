#include "PipelinePch.h"
#include "MeshAssetFactory.h"

#include "Foundation/File/Path.h"

#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/Classes/Entity.h"

#include "Pipeline/Asset/Components/MeshProcessingComponent.h"

#include "Pipeline/Importers/OBJImporter.h"

using namespace Helium;
using namespace Helium::Asset;

AssetClassPtr MeshAssetFactory::Create( const Helium::Path& path )
{
    Helium::Path assetPath = path;
    assetPath.ReplaceExtension( TXT( "HeliumEntity" ) );

    if ( !assetPath.Exists() )
    {

        EntityPtr entity = new Entity();
        entity->SetContentPath( path.Filename() ); // we're dropping this guy relative to the data file

        MeshProcessingComponentPtr meshProcessingComponent = new MeshProcessingComponent();
        entity->SetComponent( meshProcessingComponent );

        if ( !Reflect::ToArchive( assetPath, entity ) )
        {
            delete entity;
            return NULL;
        }

        entity->SetSourcePath( assetPath );
    }

    Path meshPath = path;
    meshPath.ReplaceExtension( TXT( "HeliumMesh" ) );

    if ( !meshPath.Exists() )
    {
        if ( path.Extension() == TXT( "obj" ) )
        {
            // let's try to cache the mesh
            SceneGraph::Mesh* mesh = Importers::ImportOBJ( path );

            if ( mesh )
            {
                Reflect::ToArchive( meshPath, mesh );
            }
        }
    }

    return AssetClass::LoadAssetClass( assetPath );
}