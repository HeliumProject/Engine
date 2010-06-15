#include "WorldFileAttribute.h"

#include "SceneAsset.h"

#include "Finder/AssetSpecs.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( WorldFileAttribute );

#pragma TODO("Usage of this class is deprecated")

void WorldFileAttribute::EnumerateClass( Reflect::Compositor<WorldFileAttribute>& comp )
{
  comp.GetComposite().m_UIName = "World File";
  comp.GetComposite().SetProperty( AssetProperties::ShortDescription, "World files contain 3D placement data for placing objects (zones, static and dynamic entities, volumes, etc.) in the game world." );
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "World files organize all the zones that make up a level, and the have the file extension \"world.rb\".  World files can be opened in the Scene Editor." );

//  Reflect::Field* fieldFileID = comp.AddField( &WorldFileAttribute::m_FileID, "m_FileID", Reflect::FieldFlags::FileID | Asset::AssetFlags::ManageField | Asset::AssetFlags::PerformOperation );
//  fieldFileID->SetProperty( Asset::AssetProperties::ModifierSpec, s_FileFilter.GetName() );
}

const Finder::FinderSpec& WorldFileAttribute::s_FileFilter = FinderSpecs::Asset::WORLD_DECORATION;

Attribute::AttributeUsage WorldFileAttribute::GetAttributeUsage() const
{
  return Attribute::AttributeUsages::Class;
}

Attribute::AttributeCategoryType WorldFileAttribute::GetCategoryType() const
{
  return Attribute::AttributeCategoryTypes::File;
}

//tuid WorldFileAttribute::GetFileID() const
//{
//  return m_FileID;
//}
//
//void WorldFileAttribute::SetFileID( const tuid& fileID )
//{
//  if ( m_FileID != fileID )
//  {
//    m_FileID = fileID;
//    RaiseChanged( GetClass()->FindField( &WorldFileAttribute::m_FileID ) );
//  }
//}

const Finder::FinderSpec* WorldFileAttribute::GetFileFilter() const
{
  return &s_FileFilter;
}