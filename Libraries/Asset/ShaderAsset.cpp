#include "ShaderAsset.h"
#include "Exceptions.h" 

#include "AssetTemplate.h"
#include "ColorMapAttribute.h"
#include "StandardColorMapAttribute.h"
#include "StandardDetailMapAttribute.h"
#include "StandardExpensiveMapAttribute.h"
#include "StandardNormalMapAttribute.h"
#include "TextureMapAttribute.h"

#include "Attribute/AttributeHandle.h"
#include "Console/Console.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ShaderSpecs.h"
#include "AllowedDirParser.h"

using namespace Asset;

extern AllowedDirParser g_AllowedDirParser;

REFLECT_DEFINE_ABSTRACT( ShaderAsset );

void ShaderAsset::EnumerateClass( Reflect::Compositor<ShaderAsset>& comp )
{
    comp.GetComposite().m_UIName = "Shader";
    comp.GetComposite().SetProperty( AssetProperties::ModifierSpec, FinderSpecs::Asset::SHADER_DECORATION.GetName() );

    Reflect::Field* fieldDoubleSided = comp.AddField( &ShaderAsset::m_DoubleSided, "m_DoubleSided", AssetFlags::RealTimeUpdateable );

    Reflect::EnumerationField* enumWrapModeU = comp.AddEnumerationField( &ShaderAsset::m_WrapModeU, "m_WrapModeU", AssetFlags::RealTimeUpdateable );
    Reflect::EnumerationField* enumWrapModeV = comp.AddEnumerationField( &ShaderAsset::m_WrapModeV, "m_WrapModeV", AssetFlags::RealTimeUpdateable );

    Reflect::EnumerationField* enumAlphaMode = comp.AddEnumerationField( &ShaderAsset::m_AlphaMode, "m_AlphaMode", AssetFlags::RealTimeUpdateable );

    Reflect::EnumerationField* enumWetSurfaceMode = comp.AddEnumerationField( &ShaderAsset::m_WetSurfaceMode, "m_WetSurfaceMode", AssetFlags::RealTimeUpdateable );

    // asset creation template
    Reflect::V_Element assetTemplates;

    AssetTemplatePtr shaderTemplate = new AssetTemplate( &comp.GetComposite() );
    shaderTemplate->m_DefaultAddSubDir = true;
    shaderTemplate->m_ShowSubDirCheckbox = true;
    shaderTemplate->m_AboutDirSettings = g_AllowedDirParser.GetAboutDirSettings( shaderTemplate->m_Name );
    shaderTemplate->m_DefaultRoot = FinderSpecs::Asset::SHADER_FOLDER.GetRelativeFolder();
    shaderTemplate->m_DirectoryPatterns = g_AllowedDirParser.GetPatterns( shaderTemplate->m_Name );

    shaderTemplate->AddRequiredAttribute( Reflect::GetType< Asset::StandardColorMapAttribute >() );

    shaderTemplate->AddOptionalAttribute( Reflect::GetType< Asset::StandardDetailMapAttribute >() );
    shaderTemplate->AddOptionalAttribute( Reflect::GetType< Asset::StandardExpensiveMapAttribute >() );
    shaderTemplate->AddOptionalAttribute( Reflect::GetType< Asset::StandardNormalMapAttribute >() );

    assetTemplates.push_back( shaderTemplate );

    std::stringstream stream;
    Reflect::Archive::ToStream( assetTemplates, stream, Reflect::ArchiveTypes::Binary );
    comp.GetComposite().SetProperty( AssetProperties::AssetTemplates, stream.str() );
}


bool ShaderAsset::ValidateAssetType( AssetTypes::AssetType assetType )
{
    S_AssetType assetTypes;
    GetAllowableAssetTypes( assetTypes );
    return assetTypes.find( assetType ) != assetTypes.end();
}

void ShaderAsset::GetAllowableAssetTypes( S_AssetType& assetTypes ) const
{
//    assetTypes.insert( AssetTypes::Example );
}

bool ShaderAsset::ValidateClass( std::string& error ) const
{
    TextureMapAttribute* colorMap = Reflect::ObjectCast< Asset::TextureMapAttribute >( GetAttribute( Reflect::GetType< ColorMapAttribute >() ) );
    if ( !colorMap )
    {
        error = "Shader '" + GetShortName() + "' does not have a Color Map attribute. This shader will not build!";
        return false;
    }
    else if ( !colorMap->GetFileReference().IsValid() )
    {
        error = "Shader '" + GetShortName() + "' does not have a Color Map texture specified. This shader will not build! Click on the Color Map attribute and select a valid file to use as the texture.";
        return false;
    }
    else if ( !colorMap->m_Enabled )
    {
        error = "Shader '" + GetShortName() + "' has its Color Map attribute disabled.  This shader will not build! Please enable the Color Map on this shader.";
        return false;
    }

    return __super::ValidateClass( error );
}

bool ShaderAsset::ValidateCompatible( const Attribute::AttributePtr& attr, std::string& error ) const
{
    if ( attr->HasType( Reflect::GetType<TextureMapAttribute>() ) )
    {
        return true;
    }

    return __super::ValidateCompatible( attr, error );
}

const Finder::FileSpec& ShaderAsset::GetBuiltFileSpec() const
{
    return FinderSpecs::Shader::BUILT_FILE;
}

void ShaderAsset::SetTextureDirty( File::Reference& fileRef, bool dirty )
{
    Attribute::M_Attribute::const_iterator attrItr = GetAttributes().begin();
    Attribute::M_Attribute::const_iterator attrEnd = GetAttributes().end();
    for ( ; attrItr != attrEnd; ++attrItr )
    {
        const Attribute::AttributePtr& attrib = attrItr->second;
        if ( attrib->HasType( Reflect::GetType< TextureMapAttribute >() ) )
        {
            TextureMapAttributePtr texAttrib = Reflect::DangerousCast< TextureMapAttribute >( attrib );
            if ( texAttrib->GetFileReference().GetFile().GetPath() == fileRef.GetFile().GetPath() )
            {
                texAttrib->SetTextureDirty( dirty );
            }
        }
    }
}

bool ShaderAsset::IsBuildable() const
{
    return true;
}

bool ShaderAsset::IsCinematicShader() const
{
    return false;
}
