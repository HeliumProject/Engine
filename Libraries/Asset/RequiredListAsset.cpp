#include "RequiredListAsset.h"

#include "Finder/Finder.h"
using namespace Asset;

REFLECT_DEFINE_CLASS( RequiredListAsset );

void RequiredListAsset::EnumerateClass( Reflect::Compositor<RequiredListAsset>& comp )
{
  comp.GetComposite().m_UIName = "Required Asset List";
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "" );
  comp.GetComposite().SetProperty( AssetProperties::ModifierSpec, FinderSpecs::Asset::REQUIREDLIST_DECORATION.GetName() );
  comp.GetComposite().SetProperty( AssetProperties::RootFolderSpec, FinderSpecs::Asset::REQUIREDLIST_FOLDER.GetName() );

  Reflect::Field* fieldAssetIds = comp.AddField( &RequiredListAsset::m_AssetIds, "m_AssetIds", Reflect::FieldFlags::FileID );
  fieldAssetIds->SetProperty( Asset::AssetProperties::FilterSpec, FinderSpecs::Asset::DEPENDENCIES_FILTER.GetName() );
  Reflect::Field* fieldUpdateClasses = comp.AddField( &RequiredListAsset::m_UpdateClasses, "m_UpdateClasses" );
}