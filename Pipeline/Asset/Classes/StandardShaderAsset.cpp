#include "StandardShaderAsset.h"

#include "Pipeline/Asset/Components/StandardColorMapComponent.h"
#include "Pipeline/Asset/Components/StandardNormalMapComponent.h"
#include "Pipeline/Asset/Components/StandardExpensiveMapComponent.h"
#include "Pipeline/Asset/Components/StandardDetailMapComponent.h"

#include "Pipeline/Component/ComponentHandle.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( StandardShaderAsset );

void StandardShaderAsset::EnumerateClass( Reflect::Compositor<StandardShaderAsset>& comp )
{
  comp.GetComposite().m_UIName = TXT( "Shader (Standard)" );
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "A shader manages a group of textures that can be applied to an entity." );
  comp.GetComposite().SetProperty( AssetProperties::FileFilter, "*.shader.*" );

  Reflect::Field* fieldCinematicShader = comp.AddField( &StandardShaderAsset::m_CinematicShader, "m_CinematicShader" );
}


  void StandardShaderAsset::MakeDefault()
  {
    Clear();

    StandardColorMapComponentPtr colorMap = new StandardColorMapComponent();

    SetComponent( colorMap );
  }

  void StandardShaderAsset::PostDeserialize()
  {
    __super::PostDeserialize();

#pragma TODO( "Remove legacy shader support" )
    // Legacy support.  Migrate StandardColorMapComponent::m_AlphaMode to
    // StandardShaderAsset::m_AlphaMode.
    StandardColorMapComponentPtr colorMap = Reflect::ObjectCast< StandardColorMapComponent >( GetComponent( Reflect::GetType< StandardColorMapComponent >() ) );
    if ( colorMap.ReferencesObject() )
    {
      if ( colorMap->m_AlphaMode != static_cast< AlphaType >( -1 ) )
      {
        m_AlphaMode = colorMap->m_AlphaMode;
        colorMap->m_AlphaMode = static_cast< AlphaType >( -1 );
      }
    }
  }

  bool StandardShaderAsset::ValidateCompatible( const Component::ComponentPtr& attr, tstring& error ) const
  {
    bool isValid = false;
  
    if ( __super::ValidateCompatible( attr, error ) )
    {
      isValid = ( attr->GetType() == Reflect::GetType< StandardColorMapComponent >() ||
        attr->GetType() == Reflect::GetType< StandardNormalMapComponent >() ||
        attr->GetType() == Reflect::GetType< StandardDetailMapComponent >() ||
        attr->GetType() == Reflect::GetType< StandardExpensiveMapComponent >() );
      
      if ( !isValid )
      {
        error = TXT( "Only standard texture maps are allowed on a standard shader (" );
        error += attr->GetClass()->m_UIName + TXT( " is not valid)." );
      }
    }

    return isValid;
  }

  bool StandardShaderAsset::IsCinematicShader() const
  {
    return m_CinematicShader;
}