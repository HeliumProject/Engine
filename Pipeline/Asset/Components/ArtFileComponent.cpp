#include "ArtFileComponent.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( ArtFileComponent );

void ArtFileComponent::EnumerateClass( Reflect::Compositor<ArtFileComponent>& comp )
{
  comp.GetComposite().m_UIName = TXT( "Art File" );
  comp.GetComposite().SetProperty( AssetProperties::ShortDescription, "Contains a reference to a Maya file (.mb) which contains the art (geometry, animation, etc.) for an entity in the game." );
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "All instances of an entity will utilize the same Maya file to represent their geometry in-game.  In addition to entities, Animation Clips and Animation Sets can also be associated with Maya files." );
  comp.GetComposite().SetProperty( AssetProperties::SmallIcon, "attribute_artfile_16.png" );

  Reflect::Field* fieldFragmentNode = comp.AddField( &ArtFileComponent::m_FragmentNode, "m_FragmentNode" );

  Reflect::Field* fieldExtents = comp.AddField( &ArtFileComponent::m_Extents, "m_Extents" );
  Reflect::Field* fieldOffset = comp.AddField( &ArtFileComponent::m_Offset, "m_Offset" );
  Reflect::Field* fieldMinima = comp.AddField( &ArtFileComponent::m_Minima, "m_Minima" );
  Reflect::Field* fieldMaxima = comp.AddField( &ArtFileComponent::m_Maxima, "m_Maxima" );
}

const tstring ArtFileComponent::s_FileFilter = TXT( "*.fbx" );

Component::ComponentUsage ArtFileComponent::GetComponentUsage() const
{
  return Component::ComponentUsages::Class;
}

const tstring& ArtFileComponent::GetFileFilter() const
{
  return s_FileFilter;
}