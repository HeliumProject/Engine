#include "ExpensiveMapComponent.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( ExpensiveMapComponent );

void ExpensiveMapComponent::EnumerateClass( Reflect::Compositor<ExpensiveMapComponent>& comp )
{
  comp.GetComposite().m_UIName = TXT( "Expensive Map (Base)" );

  Reflect::EnumerationField* enumIncanMipGenFilter = comp.AddEnumerationField( &ExpensiveMapComponent::m_IncanMipGenFilter, "m_IncanMipGenFilter", AssetFlags::RealTimeUpdateable );
  Reflect::EnumerationField* enumIncanPostMipFilter = comp.AddEnumerationField( &ExpensiveMapComponent::m_IncanPostMipFilter, "m_IncanPostMipFilter", AssetFlags::RealTimeUpdateable );

  Reflect::EnumerationField* enumParaMipGenFilter = comp.AddEnumerationField( &ExpensiveMapComponent::m_ParaMipGenFilter, "m_ParaMipGenFilter", AssetFlags::RealTimeUpdateable );
  Reflect::EnumerationField* enumParaPostMipFilter = comp.AddEnumerationField( &ExpensiveMapComponent::m_ParaPostMipFilter, "m_ParaPostMipFilter", AssetFlags::RealTimeUpdateable );

  Reflect::EnumerationField* enumDetailMaskMipGenFilter = comp.AddEnumerationField( &ExpensiveMapComponent::m_DetailMaskMipGenFilter, "m_DetailMaskMipGenFilter", AssetFlags::RealTimeUpdateable );
  Reflect::EnumerationField* enumDetailMaskPostMipFilter = comp.AddEnumerationField( &ExpensiveMapComponent::m_DetailMaskPostMipFilter, "m_DetailMaskPostMipFilter", AssetFlags::RealTimeUpdateable );

  Reflect::Field* fieldIncanMipFilterPasses = comp.AddField( &ExpensiveMapComponent::m_IncanMipFilterPasses, "m_IncanMipFilterPasses", Reflect::FieldFlags::Hide );

  Reflect::Field* fieldParaMipFilterPasses = comp.AddField( &ExpensiveMapComponent::m_ParaMipFilterPasses, "m_ParaMipFilterPasses", Reflect::FieldFlags::Hide );

  Reflect::Field* fieldDetailMaskMipFilterPasses = comp.AddField( &ExpensiveMapComponent::m_DetailMaskMipFilterPasses, "m_DetailMaskMipFilterPasses", Reflect::FieldFlags::Hide );
}



///////////////////////////////////////////////////////////////////////////////
// All classes that derive from ExpensiveMapComponent will occupy the same slot
// within an attribute collection.
// 
i32 ExpensiveMapComponent::GetSlot() const
{
  return Reflect::GetType< ExpensiveMapComponent >();
}

///////////////////////////////////////////////////////////////////////////////
// If any of the texture settings on oldAttrib differ from this class's value,
// this function returns true.
// 
bool ExpensiveMapComponent::ShouldRebuildTexture( const TextureMapComponent* oldAttrib ) const
{
  if ( __super::ShouldRebuildTexture( oldAttrib ) )
  {
    return true;
  }

  const ExpensiveMapComponent* oldExpensiveMap = Reflect::ConstObjectCast< ExpensiveMapComponent >( oldAttrib );
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