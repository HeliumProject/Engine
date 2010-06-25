#include "EntityAsset.h"

#include "Pipeline/Component/ComponentHandle.h"
#include "Pipeline/Asset/AssetExceptions.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Version.h"

#include "Pipeline/Asset/AssetTemplate.h"
#include "Pipeline/Asset/Components/ArtFileComponent.h"
#include "Pipeline/Asset/Components/DependenciesComponent.h"

using namespace Reflect;
using namespace Asset;
using namespace Component;

REFLECT_DEFINE_CLASS(EntityAsset)

void EntityAsset::EnumerateClass( Reflect::Compositor<EntityAsset>& comp )
{
    comp.GetComposite().m_UIName = "Entity";
    comp.GetComposite().SetProperty( AssetProperties::LongDescription, "Entities are objects that can be placed in the game engine. This includes pieces of the environment and characters. All entities are associated with a corresponding Maya art file. Instances of entities can be placed in a level with the Scene Editor." );
    comp.GetComposite().SetProperty( AssetProperties::FileFilter, "*.entity.*" );

    // asset creation template
    Reflect::V_Element assetTemplates;

    // Statis Entity (Tie)
    AssetTemplatePtr staticTemplate = new AssetTemplate( &comp.GetComposite() );
    staticTemplate->m_Name = "Static Entity";
    staticTemplate->m_Description = "Static entities are portions of the environment that can be instanced.";

    staticTemplate->m_DefaultAddSubDir = true;
    staticTemplate->m_ShowSubDirCheckbox = true;

    staticTemplate->AddRequiredComponent( Reflect::GetType< Asset::ArtFileComponent >() );
    assetTemplates.push_back( staticTemplate );

    // Dynamic Entity (Moby)
    AssetTemplatePtr dynamicTemplate = new AssetTemplate( &comp.GetComposite() );
    dynamicTemplate->m_Name = "Dynamic Entity";
    dynamicTemplate->m_Description = "A dynamic entity is an animated character or physics-related object.";

    dynamicTemplate->m_DefaultAddSubDir = true;
    dynamicTemplate->m_ShowSubDirCheckbox = true;

    dynamicTemplate->AddRequiredComponent( Reflect::GetType< Asset::ArtFileComponent >() );
    assetTemplates.push_back( dynamicTemplate );

    // Unique Geometry (UFrag)
    AssetTemplatePtr uniqueTemplate = new AssetTemplate( &comp.GetComposite() );
    uniqueTemplate->m_Name = "Unique Geometry";
    uniqueTemplate->m_Description = "A piece of geometry that is unique to a particular level.";

    uniqueTemplate->m_DefaultAddSubDir = true;
    uniqueTemplate->m_ShowSubDirCheckbox = true;
    uniqueTemplate->AddRequiredComponent( Reflect::GetType< Asset::ArtFileComponent >() );
    assetTemplates.push_back( uniqueTemplate );

    std::stringstream stream;
    Reflect::ArchiveBinary::ToStream( assetTemplates, stream );
    comp.GetComposite().SetProperty( AssetProperties::AssetTemplates, stream.str() );
}

EntityAsset::EntityAsset()
{
}

bool EntityAsset::ValidateCompatible( const Component::ComponentPtr& attr, std::string& error ) const
{
    if ( attr->HasType( Reflect::GetType<ArtFileComponent>() ) )
    {
        return true;
    }
    else if ( attr->HasType( Reflect::GetType<DependenciesComponent>() ) )
    {
        return true;
    }

    return __super::ValidateCompatible( attr, error );
}

void EntityAsset::MakeDefault()
{
    Clear();

    ArtFileComponentPtr artFile = new ArtFileComponent();

    SetComponent( artFile );
}

bool EntityAsset::IsBuildable() const
{
    return true;
}

bool EntityAsset::IsViewable() const
{
    return true;
}


const Asset::EntityManifestPtr EntityAsset::GetManifest()
{
    ComponentViewer< Asset::ArtFileComponent > artFileComponent( this );

    if( artFileComponent.Valid() )
    {
        if ( artFileComponent->GetPath().Exists() )
        {
            try
            {
                m_Manifest = Archive::FromFile<Asset::EntityManifest>( artFileComponent->GetPath() );
            }
            catch ( const Reflect::Exception& e )
            {
                Log::Error("Error loading %s (%s)\n", artFileComponent->GetPath().c_str(), e.what());
            }
        }
    }
    return m_Manifest;
}