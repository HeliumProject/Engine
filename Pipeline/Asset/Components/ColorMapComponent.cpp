#include "ColorMapComponent.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( ColorMapComponent );

void ColorMapComponent::EnumerateClass( Reflect::Compositor<ColorMapComponent>& comp )
{
  comp.GetComposite().m_UIName = TXT( "Color Map (Base)" );

  Reflect::EnumerationField* enumAlphaMipGenFilter = comp.AddEnumerationField( &ColorMapComponent::m_AlphaMipGenFilter, "m_AlphaMipGenFilter", AssetFlags::RealTimeUpdateable );
  Reflect::EnumerationField* enumAlphaPostMipFilter = comp.AddEnumerationField( &ColorMapComponent::m_AlphaPostMipFilter, "m_AlphaPostMipFilter", AssetFlags::RealTimeUpdateable );
  Reflect::Field* fieldAlphaMipFilterPasses = comp.AddField( &ColorMapComponent::m_AlphaMipFilterPasses, "m_AlphaMipFilterPasses", Reflect::FieldFlags::Hide );
}



///////////////////////////////////////////////////////////////////////////////
// All classes that derive from ColorMapComponent will occupy the same slot
// within an attribute collection.
// 
i32 ColorMapComponent::GetSlot() const
{
  return Reflect::GetType< ColorMapComponent >();
}

///////////////////////////////////////////////////////////////////////////////
// If any of the texture settings on oldAttrib differ from this class's value,
// this function returns true.
// 
bool ColorMapComponent::ShouldRebuildTexture( const TextureMapComponent* oldAttrib ) const
{
  if ( __super::ShouldRebuildTexture( oldAttrib ) )
  {
    return true;
  }

  const ColorMapComponent* oldColorMap = Reflect::ConstObjectCast< ColorMapComponent >( oldAttrib );
  if ( oldColorMap == NULL )
  {
    return true;
  }

  return
    m_AlphaMipGenFilter != oldColorMap->m_AlphaMipGenFilter || 
    m_AlphaPostMipFilter != oldColorMap->m_AlphaPostMipFilter;
}