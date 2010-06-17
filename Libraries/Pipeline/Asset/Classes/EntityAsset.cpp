#include "EntityAsset.h"

#include "Attribute/AttributeHandle.h"
#include "Pipeline/Asset/AssetExceptions.h"
#include "Finder/AssetSpecs.h"
#include "Reflect/Archive.h"
#include "Reflect/Element.h"
#include "Reflect/Version.h"

#include "Pipeline/Asset/AssetTemplate.h"
#include "Pipeline/Asset/Attributes/ArtFileAttribute.h"
#include "Pipeline/Asset/Attributes/DependenciesAttribute.h"

using namespace Reflect;
using namespace Asset;
using namespace Attribute;

REFLECT_DEFINE_CLASS(EntityAsset)

void EntityAsset::EnumerateClass( Reflect::Compositor<EntityAsset>& comp )
{
    comp.GetComposite().m_UIName = "Entity";
    comp.GetComposite().SetProperty( AssetProperties::LongDescription, "Entities are objects that can be placed in the game engine. This includes pieces of the environment and characters. All entities are associated with a corresponding Maya art file. Instances of entities can be placed in a level with the Scene Editor." );
    comp.GetComposite().SetProperty( AssetProperties::ModifierSpec, FinderSpecs::Asset::ENTITY_DECORATION.GetName() );

    // asset creation template
    Reflect::V_Element assetTemplates;

    // Statis Entity (Tie)
    AssetTemplatePtr staticTemplate = new AssetTemplate( &comp.GetComposite() );
    staticTemplate->m_Name = "Static Entity";
    staticTemplate->m_Description = "Static entities are portions of the environment that can be instanced.";

    staticTemplate->m_DefaultAddSubDir = true;
    staticTemplate->m_ShowSubDirCheckbox = true;

    staticTemplate->AddRequiredAttribute( Reflect::GetType< Asset::ArtFileAttribute >() );
    assetTemplates.push_back( staticTemplate );

    // Dynamic Entity (Moby)
    AssetTemplatePtr dynamicTemplate = new AssetTemplate( &comp.GetComposite() );
    dynamicTemplate->m_Name = "Dynamic Entity";
    dynamicTemplate->m_Description = "A dynamic entity is an animated character or physics-related object.";

    dynamicTemplate->m_DefaultAddSubDir = true;
    dynamicTemplate->m_ShowSubDirCheckbox = true;

    dynamicTemplate->AddRequiredAttribute( Reflect::GetType< Asset::ArtFileAttribute >() );
    assetTemplates.push_back( dynamicTemplate );

    // Unique Geometry (UFrag)
    AssetTemplatePtr uniqueTemplate = new AssetTemplate( &comp.GetComposite() );
    uniqueTemplate->m_Name = "Unique Geometry";
    uniqueTemplate->m_Description = "A piece of geometry that is unique to a particular level.";

    uniqueTemplate->m_DefaultAddSubDir = true;
    uniqueTemplate->m_ShowSubDirCheckbox = true;
    uniqueTemplate->AddRequiredAttribute( Reflect::GetType< Asset::ArtFileAttribute >() );
    assetTemplates.push_back( uniqueTemplate );

    std::stringstream stream;
    Reflect::Archive::ToStream( assetTemplates, stream, Reflect::ArchiveTypes::Binary );
    comp.GetComposite().SetProperty( AssetProperties::AssetTemplates, stream.str() );
}

EntityAsset::EntityAsset()
{
}

bool EntityAsset::ValidateCompatible( const Attribute::AttributePtr& attr, std::string& error ) const
{
    if ( attr->HasType( Reflect::GetType<ArtFileAttribute>() ) )
    {
        return true;
    }
    else if ( attr->HasType( Reflect::GetType<DependenciesAttribute>() ) )
    {
        return true;
    }

    return __super::ValidateCompatible( attr, error );
}

void EntityAsset::MakeDefault()
{
    Clear();

    ArtFileAttributePtr artFile = new ArtFileAttribute();

    SetAttribute( artFile );
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
    AttributeViewer< Asset::ArtFileAttribute > artFileAttribute( this );

    if( artFileAttribute.Valid() )
    {
        if ( artFileAttribute->GetPath().Exists() )
        {
            try
            {
                m_Manifest = Archive::FromFile<Asset::EntityManifest>( artFileAttribute->GetPath() );
            }
            catch ( const Reflect::Exception& e )
            {
                Log::Error("Error loading %s (%s)\n", artFileAttribute->GetPath().c_str(), e.what());
            }
        }
    }
    return m_Manifest;
}