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

  Reflect::Field* fieldFileReferences = comp.AddField( &RequiredListAsset::m_FileReferences, "m_FileReferences", Reflect::FieldFlags::FileRef );
  fieldFileReferences->SetProperty( Asset::AssetProperties::FilterSpec, FinderSpecs::Asset::DEPENDENCIES_FILTER.GetName() );
}