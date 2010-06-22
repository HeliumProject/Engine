#include "DependenciesComponent.h"
#include "Pipeline/Asset/Classes/EntityAsset.h"
#include "Pipeline/Asset/Classes/SceneAsset.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( DependenciesComponent );

void DependenciesComponent::EnumerateClass( Reflect::Compositor<DependenciesComponent>& comp )
{
  comp.GetComposite().m_UIName = "Dependencies";
  comp.GetComposite().SetProperty( AssetProperties::ShortDescription, "Allows for specifying other assets which are necessary for the current asset." );
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "This attribute allows for specifying other assets which are necessary for the given asset to function properly.  Often, this can be used on assets like levels to list entities that are needed but that can't be found via normal inclusion rules." );

  Reflect::Field* fieldPaths = comp.AddField( &DependenciesComponent::m_Paths, "m_Paths", Reflect::FieldFlags::Path );
  fieldPaths->SetProperty( Asset::AssetProperties::FileFilter, "*.entity.*;*.font.*;*.movie.*;*.shader.*;*.scene.*" );
}


Component::ComponentCategoryType DependenciesComponent::GetCategoryType() const
{
  return Component::ComponentCategoryTypes::Misc;
}