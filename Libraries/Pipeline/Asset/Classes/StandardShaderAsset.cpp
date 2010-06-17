#include "StandardShaderAsset.h"

#include "Pipeline/Asset/Attributes/StandardColorMapAttribute.h"
#include "Pipeline/Asset/Attributes/StandardNormalMapAttribute.h"
#include "Pipeline/Asset/Attributes/StandardExpensiveMapAttribute.h"
#include "Pipeline/Asset/Attributes/StandardDetailMapAttribute.h"

#include "Attribute/AttributeHandle.h"
#include "Finder/AssetSpecs.h"

using namespace Asset;

  REFLECT_DEFINE_CLASS( StandardShaderAsset );

void StandardShaderAsset::EnumerateClass( Reflect::Compositor<StandardShaderAsset>& comp )
{
  comp.GetComposite().m_UIName = "Shader (Standard)";
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "A shader manages a group of textures that can be applied to an entity." );
  comp.GetComposite().SetProperty( AssetProperties::ModifierSpec, FinderSpecs::Asset::SHADER_DECORATION.GetName() );

  Reflect::Field* fieldCinematicShader = comp.AddField( &StandardShaderAsset::m_CinematicShader, "m_CinematicShader" );
}


  void StandardShaderAsset::MakeDefault()
  {
    Clear();

    StandardColorMapAttributePtr colorMap = new StandardColorMapAttribute();

    SetAttribute( colorMap );
  }

  void StandardShaderAsset::PostDeserialize()
  {
    __super::PostDeserialize();

#pragma TODO( "Remove legacy shader support" )
    // Legacy support.  Migrate StandardColorMapAttribute::m_AlphaMode to
    // StandardShaderAsset::m_AlphaMode.
    StandardColorMapAttributePtr colorMap = Reflect::ObjectCast< StandardColorMapAttribute >( GetAttribute( Reflect::GetType< StandardColorMapAttribute >() ) );
    if ( colorMap.ReferencesObject() )
    {
      if ( colorMap->m_AlphaMode != static_cast< AlphaType >( -1 ) )
      {
        m_AlphaMode = colorMap->m_AlphaMode;
        colorMap->m_AlphaMode = static_cast< AlphaType >( -1 );
      }
    }
  }

  bool StandardShaderAsset::ValidateCompatible( const Attribute::AttributePtr& attr, std::string& error ) const
  {
    bool isValid = false;
  
    if ( __super::ValidateCompatible( attr, error ) )
    {
      isValid = ( attr->GetType() == Reflect::GetType< StandardColorMapAttribute >() ||
        attr->GetType() == Reflect::GetType< StandardNormalMapAttribute >() ||
        attr->GetType() == Reflect::GetType< StandardDetailMapAttribute >() ||
        attr->GetType() == Reflect::GetType< StandardExpensiveMapAttribute >() );
      
      if ( !isValid )
      {
        error = "Only standard texture maps are allowed on a standard shader (";
        error += attr->GetClass()->m_UIName + " is not valid).";
      }
    }

    return isValid;
  }

  bool StandardShaderAsset::IsCinematicShader() const
  {
    return m_CinematicShader;
}