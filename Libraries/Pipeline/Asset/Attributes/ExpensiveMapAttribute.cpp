#include "ExpensiveMapAttribute.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( ExpensiveMapAttribute );

void ExpensiveMapAttribute::EnumerateClass( Reflect::Compositor<ExpensiveMapAttribute>& comp )
{
  comp.GetComposite().m_UIName = "Expensive Map (Base)";

  Reflect::EnumerationField* enumIncanMipGenFilter = comp.AddEnumerationField( &ExpensiveMapAttribute::m_IncanMipGenFilter, "m_IncanMipGenFilter", AssetFlags::RealTimeUpdateable );
  Reflect::EnumerationField* enumIncanPostMipFilter = comp.AddEnumerationField( &ExpensiveMapAttribute::m_IncanPostMipFilter, "m_IncanPostMipFilter", AssetFlags::RealTimeUpdateable );

  Reflect::EnumerationField* enumParaMipGenFilter = comp.AddEnumerationField( &ExpensiveMapAttribute::m_ParaMipGenFilter, "m_ParaMipGenFilter", AssetFlags::RealTimeUpdateable );
  Reflect::EnumerationField* enumParaPostMipFilter = comp.AddEnumerationField( &ExpensiveMapAttribute::m_ParaPostMipFilter, "m_ParaPostMipFilter", AssetFlags::RealTimeUpdateable );

  Reflect::EnumerationField* enumDetailMaskMipGenFilter = comp.AddEnumerationField( &ExpensiveMapAttribute::m_DetailMaskMipGenFilter, "m_DetailMaskMipGenFilter", AssetFlags::RealTimeUpdateable );
  Reflect::EnumerationField* enumDetailMaskPostMipFilter = comp.AddEnumerationField( &ExpensiveMapAttribute::m_DetailMaskPostMipFilter, "m_DetailMaskPostMipFilter", AssetFlags::RealTimeUpdateable );

  Reflect::Field* fieldIncanMipFilterPasses = comp.AddField( &ExpensiveMapAttribute::m_IncanMipFilterPasses, "m_IncanMipFilterPasses", Reflect::FieldFlags::Hide );

  Reflect::Field* fieldParaMipFilterPasses = comp.AddField( &ExpensiveMapAttribute::m_ParaMipFilterPasses, "m_ParaMipFilterPasses", Reflect::FieldFlags::Hide );

  Reflect::Field* fieldDetailMaskMipFilterPasses = comp.AddField( &ExpensiveMapAttribute::m_DetailMaskMipFilterPasses, "m_DetailMaskMipFilterPasses", Reflect::FieldFlags::Hide );
}



///////////////////////////////////////////////////////////////////////////////
// All classes that derive from ExpensiveMapAttribute will occupy the same slot
// within an attribute collection.
// 
i32 ExpensiveMapAttribute::GetSlot() const
{
  return Reflect::GetType< ExpensiveMapAttribute >();
}

///////////////////////////////////////////////////////////////////////////////
// If any of the texture settings on oldAttrib differ from this class's value,
// this function returns true.
// 
bool ExpensiveMapAttribute::ShouldRebuildTexture( const TextureMapAttribute* oldAttrib ) const
{
  if ( __super::ShouldRebuildTexture( oldAttrib ) )
  {
    return true;
  }

  const ExpensiveMapAttribute* oldExpensiveMap = Reflect::ConstObjectCast< ExpensiveMapAttribute >( oldAttrib );
  if ( oldExpensiveMap == NULL )
  {
    return true;
  }

  return
    m_IncanMipGenFilter != oldExpensiveMap->m_IncanMipGenFilter ||
    m_IncanPostMipFilter != oldExpensiveMap->m_IncanPostMipFilter ||
    m_ParaMipGenFilter != oldExpensiveMap->m_ParaMipGenFilter || 
    m_ParaPostMipFilter != oldExpensiveMap->m_ParaPostMipFilter;
}