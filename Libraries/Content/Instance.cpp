#include "Instance.h"
#include "Scene.h"

#include "Attribute/AttributeHandle.h"
#include "Console/Console.h"
#include "File/Manager.h"
#include "Finder/ContentSpecs.h"

using namespace Content;
using namespace Attribute; 

REFLECT_DEFINE_ABSTRACT(Instance);

void Instance::EnumerateClass( Reflect::Compositor<Instance>& comp )
{
  Reflect::Field* fieldConfiguredType = comp.AddField( &Instance::m_ConfiguredType, "m_ConfiguredType", Reflect::FieldFlags::Hide );

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

Instance::Instance(const UniqueID::TUID& id)
: PivotTransform (id)
, m_Solid (false)
, m_SolidOverride (false)
, m_Transparent (false)
, m_TransparentOverride (false)
{

}
