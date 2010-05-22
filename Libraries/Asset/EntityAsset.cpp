#include "EntityAsset.h"

#include "Attribute/AttributeHandle.h"
#include "Exceptions.h"
#include "Reflect/Archive.h"
#include "Reflect/Element.h"
#include "Reflect/Version.h"

#include "AssetTemplate.h"
#include "ArtFileAttribute.h"
#include "DependenciesAttribute.h"
#include "AllowedDirParser.h"

#include "FileSystem/FileSystem.h"
#include "Finder/ContentSpecs.h"

using namespace Reflect;
using namespace Asset;
using namespace Attribute;

extern AllowedDirParser g_AllowedDirParser;

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
    staticTemplate->m_AboutDirSettings = g_AllowedDirParser.GetAboutDirSettings( staticTemplate->m_Name );
    staticTemplate->m_DirectoryPatterns = g_AllowedDirParser.GetPatterns( staticTemplate->m_Name );

    staticTemplate->AddRequiredAttribute( Reflect::GetType< Asset::ArtFileAttribute >() );
    assetTemplates.push_back( staticTemplate );

    // Dynamic Entity (Moby)
    AssetTemplatePtr dynamicTemplate = new AssetTemplate( &comp.GetComposite() );
    dynamicTemplate->m_Name = "Dynamic Entity";
    dynamicTemplate->m_Description = "A dynamic entity is an animated character or physics-related object.";

    dynamicTemplate->m_DefaultAddSubDir = true;
    dynamicTemplate->m_ShowSubDirCheckbox = true;
    dynamicTemplate->m_AboutDirSettings = g_AllowedDirParser.GetAboutDirSettings( dynamicTemplate->m_Name );
    dynamicTemplate->m_DirectoryPatterns = g_AllowedDirParser.GetPatterns( dynamicTemplate->m_Name );

    dynamicTemplate->AddRequiredAttribute( Reflect::GetType< Asset::ArtFileAttribute >() );
    assetTemplates.push_back( dynamicTemplate );

    // Unique Geometry (UFrag)
    AssetTemplatePtr uniqueTemplate = new AssetTemplate( &comp.GetComposite() );
    uniqueTemplate->m_Name = "Unique Geometry";
    uniqueTemplate->m_Description = "A piece of geometry that is unique to a particular level.";

    uniqueTemplate->m_DefaultAddSubDir = true;
    uniqueTemplate->m_ShowSubDirCheckbox = true;
    uniqueTemplate->m_AboutDirSettings = g_AllowedDirParser.GetAboutDirSettings( uniqueTemplate->m_Name );
    uniqueTemplate->m_DirectoryPatterns = g_AllowedDirParser.GetPatterns( uniqueTemplate->m_Name );
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
        std::string assetManifestFile = FinderSpecs::Content::MANIFEST_DECORATION.GetExportFile( artFileAttribute->GetFileReference().GetPath(), artFileAttribute->m_FragmentNode );

        if (FileSystem::Exists(assetManifestFile))
        {
            try
            {
                m_Manifest = Archive::FromFile<Asset::EntityManifest>(assetManifestFile);
            }
            catch ( const Reflect::Exception& e )
            {
                Console::Error("Error loading %s (%s)\n", assetManifestFile.c_str(), e.what());
            }
        }
    }
    return m_Manifest;
}