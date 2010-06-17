#include "WorldFileComponent.h"

#include "Pipeline/Asset/Classes/SceneAsset.h"

#include "Finder/AssetSpecs.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( WorldFileComponent );

#pragma TODO("Usage of this class is deprecated")

void WorldFileComponent::EnumerateClass( Reflect::Compositor<WorldFileComponent>& comp )
{
  comp.GetComposite().m_UIName = "World File";
  comp.GetComposite().SetProperty( AssetProperties::ShortDescription, "World files contain 3D placement data for placing objects (zones, static and dynamic entities, volumes, etc.) in the game world." );
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "World files organize all the zones that make up a level, and the have the file extension \"world.rb\".  World files can be opened in the Scene Editor." );

//  Reflect::Field* fieldFileID = comp.AddField( &WorldFileComponent::m_FileID, "m_FileID", Reflect::FieldFlags::FileID | Asset::AssetFlags::ManageField | Asset::AssetFlags::PerformOperation );
//  fieldFileID->SetProperty( Asset::AssetProperties::ModifierSpec, s_FileFilter.GetName() );
}

const Finder::FinderSpec& WorldFileComponent::s_FileFilter = FinderSpecs::Asset::WORLD_DECORATION;

Component::ComponentUsage WorldFileComponent::GetComponentUsage() const
{
  return Component::ComponentUsages::Class;
}

Component::ComponentCategoryType WorldFileComponent::GetCategoryType() const
{
  return Component::ComponentCategoryTypes::File;
}

//tuid WorldFileComponent::GetFileID() const
//{
//  return m_FileID;
//}
//
//void WorldFileComponent::SetFileID( const tuid& fileID )
//{
//  if ( m_FileID != fileID )
//  {
//    m_FileID = fileID;
//    RaiseChanged( GetClass()->FindField( &WorldFileComponent::m_FileID ) );
//  }
//}

const Finder::FinderSpec* WorldFileComponent::GetFileFilter() const
{
  return &s_FileFilter;
}