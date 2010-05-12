#include "BangleRemapAttribute.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( BangleRemapAttribute );

void BangleRemapAttribute::EnumerateClass( Reflect::Compositor<BangleRemapAttribute>& comp )
{
  comp.GetComposite().SetProperty( Reflect::PropertyNames::UIName, "Bangle Remap" );

  Reflect::Field* fieldBangleRemap = comp.AddField( &BangleRemapAttribute::m_BangleRemap, "m_BangleRemap" );
  Reflect::Field* fieldDiscardMainGeom = comp.AddField( &BangleRemapAttribute::m_DiscardMainGeom, "m_DiscardMainGeom" );
}


AttributeUsage BangleRemapAttribute::GetAttributeUsage() const
{
  return Asset::AttributeUsages::Class;
}

AttributeCategoryType BangleRemapAttribute::GetCategoryType() const
{
  return AttributeCategoryTypes::Misc;
}