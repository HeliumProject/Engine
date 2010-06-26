#include "ShaderAsset.h"

#include "Foundation/Log.h"
#include "Foundation/Reflect/ArchiveXML.h"

#include "Pipeline/Asset/AssetExceptions.h" 
#include "Pipeline/Asset/AssetTemplate.h"
#include "Pipeline/Asset/Components/ColorMapComponent.h"
#include "Pipeline/Asset/Components/StandardColorMapComponent.h"
#include "Pipeline/Asset/Components/StandardDetailMapComponent.h"
#include "Pipeline/Asset/Components/StandardExpensiveMapComponent.h"
#include "Pipeline/Asset/Components/StandardNormalMapComponent.h"
#include "Pipeline/Asset/Components/TextureMapComponent.h"
#include "Pipeline/Component/ComponentHandle.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( ShaderAsset );

void ShaderAsset::EnumerateClass( Reflect::Compositor<ShaderAsset>& comp )
{
    comp.GetComposite().m_UIName = TXT( "Shader" );
    comp.GetComposite().SetProperty( AssetProperties::FileFilter, "*.shader.*" );

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

    shaderTemplate->AddRequiredComponent( Reflect::GetType< Asset::StandardColorMapComponent >() );

    shaderTemplate->AddOptionalComponent( Reflect::GetType< Asset::StandardDetailMapComponent >() );
    shaderTemplate->AddOptionalComponent( Reflect::GetType< Asset::StandardExpensiveMapComponent >() );
    shaderTemplate->AddOptionalComponent( Reflect::GetType< Asset::StandardNormalMapComponent >() );

    assetTemplates.push_back( shaderTemplate );

    tstring str;
    Reflect::ArchiveXML::ToString( assetTemplates, str );
    comp.GetComposite().SetProperty( AssetProperties::AssetTemplates, str );
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

bool ShaderAsset::ValidateClass( tstring& error ) const
{
    TextureMapComponent* colorMap = Reflect::ObjectCast< Asset::TextureMapComponent >( GetComponent( Reflect::GetType< ColorMapComponent >() ) );
    if ( !colorMap )
    {
        error = tstring( TXT( "Shader '" ) ) + GetShortName() + TXT( "' does not have a Color Map attribute. This shader will not build!" );
        return false;
    }
    else if ( !colorMap->GetPath().Exists() )
    {
        error = tstring( TXT( "Shader '" ) ) + GetShortName() + TXT( "' does not have a Color Map texture specified. This shader will not build! Click on the Color Map attribute and select a valid file to use as the texture." );
        return false;
    }
    else if ( !colorMap->m_Enabled )
    {
        error = tstring( TXT( "Shader '" ) ) + GetShortName() + TXT( "' has its Color Map attribute disabled.  This shader will not build! Please enable the Color Map on this shader." );
        return false;
    }

    return __super::ValidateClass( error );
}

bool ShaderAsset::ValidateCompatible( const Component::ComponentPtr& attr, tstring& error ) const
{
    if ( attr->HasType( Reflect::GetType<TextureMapComponent>() ) )
    {
        return true;
    }

    return __super::ValidateCompatible( attr, error );
}

void ShaderAsset::SetTextureDirty( const Nocturnal::Path& path, bool dirty )
{
    Component::M_Component::const_iterator attrItr = GetComponents().begin();
    Component::M_Component::const_iterator attrEnd = GetComponents().end();
    for ( ; attrItr != attrEnd; ++attrItr )
    {
        const Component::ComponentPtr& attrib = attrItr->second;
        if ( attrib->HasType( Reflect::GetType< TextureMapComponent >() ) )
        {
            TextureMapComponentPtr texAttrib = Reflect::DangerousCast< TextureMapComponent >( attrib );
            if ( texAttrib->GetPath() == path )
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
