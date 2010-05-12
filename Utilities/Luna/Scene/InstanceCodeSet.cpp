#include "Precompile.h"
#include "InstanceCodeSet.h"

#include "Instance.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::InstanceCodeSet);

void InstanceCodeSet::InitializeType()
{
  Reflect::RegisterClass< Luna::InstanceCodeSet >( "Luna::InstanceCodeSet" );
}

void InstanceCodeSet::CleanupType()
{
  Reflect::UnregisterClass< Luna::InstanceCodeSet >();
}

InstanceCodeSet::InstanceCodeSet(Luna::InstanceType* type, const std::string& classStr)
: Luna::InstanceSet(type)
, m_Name (classStr)
{

}

const std::string& InstanceCodeSet::GetName() const
{
  return m_Name;
}

void InstanceCodeSet::AddInstance(Luna::Instance* i)
{
  // set class link (must be done before calling base class)
  i->SetCodeSet( this );

  __super::AddInstance(i);
}

void InstanceCodeSet::RemoveInstance(Luna::Instance* i)
{
  __super::RemoveInstance(i);

  // release class list (must be done after calling base class)
  i->SetCodeSet( NULL );
}
