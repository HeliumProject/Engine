#include "DependenciesAttribute.h"
#include "EntityAsset.h"
#include "SceneAsset.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( DependenciesAttribute );

void DependenciesAttribute::EnumerateClass( Reflect::Compositor<DependenciesAttribute>& comp )
{
  comp.GetComposite().m_UIName = "Dependencies";
  comp.GetComposite().SetProperty( AssetProperties::ShortDescription, "Allows for specifying other assets which are necessary for the current asset." );
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "This attribute allows for specifying other assets which are necessary for the given asset to function properly.  Often, this can be used on assets like levels to list entities that are needed but that can't be found via normal inclusion rules." );

  Reflect::Field* fieldFileReferences = comp.AddField( &DependenciesAttribute::m_FileReferences, "m_FileReferences", Reflect::FieldFlags::FileRef );
  fieldFileReferences->SetProperty( Asset::AssetProperties::FilterSpec, FinderSpecs::Asset::DEPENDENCIES_FILTER.GetName() );
}


Attribute::AttributeCategoryType DependenciesAttribute::GetCategoryType() const
{
  return Attribute::AttributeCategoryTypes::Misc;
}