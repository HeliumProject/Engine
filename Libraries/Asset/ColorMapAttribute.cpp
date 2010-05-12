#include "ColorMapAttribute.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( ColorMapAttribute );

void ColorMapAttribute::EnumerateClass( Reflect::Compositor<ColorMapAttribute>& comp )
{
  comp.GetComposite().m_UIName = "Color Map (Base)";

  Reflect::EnumerationField* enumAlphaMipGenFilter = comp.AddEnumerationField( &ColorMapAttribute::m_AlphaMipGenFilter, "m_AlphaMipGenFilter", AssetFlags::RealTimeUpdateable );
  Reflect::EnumerationField* enumAlphaPostMipFilter = comp.AddEnumerationField( &ColorMapAttribute::m_AlphaPostMipFilter, "m_AlphaPostMipFilter", AssetFlags::RealTimeUpdateable );
  Reflect::Field* fieldAlphaMipFilterPasses = comp.AddField( &ColorMapAttribute::m_AlphaMipFilterPasses, "m_AlphaMipFilterPasses", Reflect::FieldFlags::Hide );
}



///////////////////////////////////////////////////////////////////////////////
// All classes that derive from ColorMapAttribute will occupy the same slot
// within an attribute collection.
// 
i32 ColorMapAttribute::GetSlot() const
{
  return Reflect::GetType< ColorMapAttribute >();
}

///////////////////////////////////////////////////////////////////////////////
// If any of the texture settings on oldAttrib differ from this class's value,
// this function returns true.
// 
bool ColorMapAttribute::ShouldRebuildTexture( const TextureMapAttribute* oldAttrib ) const
{
  if ( __super::ShouldRebuildTexture( oldAttrib ) )
  {
    return true;
  }

  const ColorMapAttribute* oldColorMap = Reflect::ConstObjectCast< ColorMapAttribute >( oldAttrib );
  if ( oldColorMap == NULL )
  {
    return true;
  }

  return
    m_AlphaMipGenFilter != oldColorMap->m_AlphaMipGenFilter || 
    m_AlphaPostMipFilter != oldColorMap->m_AlphaPostMipFilter;
}