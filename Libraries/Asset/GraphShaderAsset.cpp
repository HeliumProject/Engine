#include "GraphShaderAsset.h"

#include "AssetTemplate.h"
#include "StandardColorMapAttribute.h"
#include "StandardExpensiveMapAttribute.h"
#include "StandardNormalMapAttribute.h"
#include "StandardDetailMapAttribute.h"

#include "Attribute/AttributeHandle.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ShaderSpecs.h"
#include "AllowedDirParser.h"

using namespace Asset;

extern AllowedDirParser g_AllowedDirParser;

REFLECT_DEFINE_CLASS( GraphShaderAsset );

void GraphShaderAsset::EnumerateClass( Reflect::Compositor<GraphShaderAsset>& comp )
{
  comp.GetComposite().m_UIName = "Graph Shader";
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "Custom shader that uses data from the Graph Shader tool." );
  comp.GetComposite().SetProperty( AssetProperties::ModifierSpec, FinderSpecs::Asset::SHADER_DECORATION.GetName() );
  comp.GetComposite().SetProperty( AssetProperties::RootFolderSpec, FinderSpecs::Asset::GRAPH_SHADER_FOLDER.GetName() );

  Reflect::Field* fieldGraphShader = comp.AddField( &GraphShaderAsset::m_GraphFile, "m_GraphFile", AssetFlags::RealTimeUpdateable| Asset::AssetFlags::ManageField | Reflect::FieldFlags::FileID );
  fieldGraphShader->SetProperty( Asset::AssetProperties::ModifierSpec, FinderSpecs::Extension::GRAPH_SHADER.GetName() );

  // asset creation template
  Reflect::V_Element assetTemplates;

  AssetTemplatePtr shaderTemplate = new AssetTemplate( &comp.GetComposite() );
  shaderTemplate->m_DefaultAddSubDir = true;
  shaderTemplate->m_ShowSubDirCheckbox = true;
  shaderTemplate->m_AboutDirSettings = g_AllowedDirParser.GetAboutDirSettings( shaderTemplate->m_Name );
  shaderTemplate->m_DefaultRoot = FinderSpecs::Asset::SHADER_FOLDER.GetRelativeFolder();
  shaderTemplate->m_DirectoryPatterns = g_AllowedDirParser.GetPatterns( shaderTemplate->m_Name );
  shaderTemplate->AddOptionalAttribute( Reflect::GetType< Asset::StandardColorMapAttribute >() );
  shaderTemplate->AddOptionalAttribute( Reflect::GetType< Asset::StandardNormalMapAttribute >() );
  shaderTemplate->AddOptionalAttribute( Reflect::GetType< Asset::StandardExpensiveMapAttribute >() );
  shaderTemplate->AddOptionalAttribute( Reflect::GetType< Asset::StandardDetailMapAttribute >() );
  assetTemplates.push_back( shaderTemplate );

  std::stringstream stream;
  Reflect::Archive::ToStream( assetTemplates, stream, Reflect::ArchiveTypes::Binary );
  comp.GetComposite().SetProperty( AssetProperties::AssetTemplates, stream.str() );
}

void GraphShaderAsset::MakeDefault()
{
  Clear();

  SetAttribute( new StandardColorMapAttribute() );
}

void GraphShaderAsset::GetAllowableEngineTypes( S_EngineType& engineTypes ) const
{
  engineTypes.insert( EngineTypes::Moby );
  engineTypes.insert( EngineTypes::Tie );
  engineTypes.insert( EngineTypes::Ufrag );
}

bool GraphShaderAsset::ValidateCompatible( const Attribute::AttributePtr& attr, std::string& error ) const
{
  bool isValid = false;

  if ( __super::ValidateCompatible( attr, error ) )
  {
    isValid = ( attr->GetType() == Reflect::GetType< StandardColorMapAttribute >()      ||
                attr->GetType() == Reflect::GetType< StandardNormalMapAttribute >()     ||
                attr->GetType() == Reflect::GetType< StandardDetailMapAttribute >()     ||
                attr->GetType() == Reflect::GetType< StandardExpensiveMapAttribute >());

    if ( !isValid )
    {
      error = "Only standard texture maps are allowed on a standard shader (";
      error += attr->GetClass()->m_UIName + " is not valid).";
    }
  }

  return isValid;
}

const Finder::FileSpec& GraphShaderAsset::GetBuiltFileSpec() const
{
  return FinderSpecs::Shader::GRAPH_SHADER_BUILT_FILE;
}