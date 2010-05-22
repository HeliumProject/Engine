#include "ArtFileAttribute.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( ArtFileAttribute );

void ArtFileAttribute::EnumerateClass( Reflect::Compositor<ArtFileAttribute>& comp )
{
  comp.GetComposite().m_UIName = "Art File";
  comp.GetComposite().SetProperty( AssetProperties::ShortDescription, "Contains a reference to a Maya file (.mb) which contains the art (geometry, animation, etc.) for an entity in the game." );
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "All instances of an entity will utilize the same Maya file to represent their geometry in-game.  In addition to entities, Animation Clips and Animation Sets can also be associated with Maya files." );
  comp.GetComposite().SetProperty( AssetProperties::SmallIcon, "attribute_artfile_16.png" );

  Reflect::Field* fieldFragmentNode = comp.AddField( &ArtFileAttribute::m_FragmentNode, "m_FragmentNode" );

  Reflect::Field* fieldExtents = comp.AddField( &ArtFileAttribute::m_Extents, "m_Extents" );
  Reflect::Field* fieldOffset = comp.AddField( &ArtFileAttribute::m_Offset, "m_Offset" );
  Reflect::Field* fieldMinima = comp.AddField( &ArtFileAttribute::m_Minima, "m_Minima" );
  Reflect::Field* fieldMaxima = comp.AddField( &ArtFileAttribute::m_Maxima, "m_Maxima" );
}

const Finder::FinderSpec& ArtFileAttribute::s_FileFilter = FinderSpecs::Extension::MAYA_BINARY;

Attribute::AttributeUsage ArtFileAttribute::GetAttributeUsage() const
{
  return Attribute::AttributeUsages::Class;
}

const Finder::FinderSpec* ArtFileAttribute::GetFileFilter() const
{
  return &s_FileFilter;
}