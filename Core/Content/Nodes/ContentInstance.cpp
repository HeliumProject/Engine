#include "ContentInstance.h"

#include "Foundation/Component/ComponentHandle.h"
#include "Foundation/Log.h"

using namespace Helium;
using namespace Helium::Content;
using namespace Helium::Component;

REFLECT_DEFINE_ABSTRACT(Instance);

void Instance::EnumerateClass( Reflect::Compositor<Instance>& comp )
{
  Reflect::Field* fieldSolid                = comp.AddField( &Instance::m_Solid, "m_Solid" );
  Reflect::Field* fieldSolidOverride        = comp.AddField( &Instance::m_SolidOverride, "m_SolidOverride" );
  Reflect::Field* fieldTransparent          = comp.AddField( &Instance::m_Transparent, "m_Transparent" );
  Reflect::Field* fieldTransparentOverride  = comp.AddField( &Instance::m_TransparentOverride, "m_TransparentOverride" );
}

Instance::Instance()
: m_Solid (false)
, m_SolidOverride (false)
, m_Transparent (false)
, m_TransparentOverride (false)
{

}

Instance::Instance(const Helium::TUID& id)
: PivotTransform (id)
, m_Solid (false)
, m_SolidOverride (false)
, m_Transparent (false)
, m_TransparentOverride (false)
{

}
