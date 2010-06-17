#include "RequiredListAsset.h"

#include "Finder/Finder.h"
using namespace Asset;

REFLECT_DEFINE_CLASS( RequiredListAsset );

void RequiredListAsset::EnumerateClass( Reflect::Compositor<RequiredListAsset>& comp )
{
  comp.GetComposite().m_UIName = "Required Asset List";
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "" );
  comp.GetComposite().SetProperty( AssetProperties::ModifierSpec, FinderSpecs::Asset::REQUIREDLIST_DECORATION.GetName() );

  Reflect::Field* fieldPaths = comp.AddField( &RequiredListAsset::m_Paths, "m_Paths", Reflect::FieldFlags::Path );
  fieldPaths->SetProperty( Asset::AssetProperties::FilterSpec, FinderSpecs::Asset::DEPENDENCIES_FILTER.GetName() );
}