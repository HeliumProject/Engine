#include "Precompile.h"
#include "InstanceSet.h"
#include "Instance.h"
#include "InstanceType.h"

#include "Common/Container/Insert.h" 

using Nocturnal::Insert; 
using namespace Luna;

LUNA_DEFINE_TYPE(Luna::InstanceSet);

void InstanceSet::InitializeType()
{
  Reflect::RegisterClass< Luna::InstanceSet >( "Luna::InstanceSet" );
}

void Luna::InstanceSet::CleanupType()
{
  Reflect::UnregisterClass< Luna::InstanceSet >();
}

InstanceSet::InstanceSet(Luna::InstanceType* type)
: m_Type (type)
{

}

InstanceSet::~InstanceSet()
{

}

Luna::InstanceType* InstanceSet::GetInstanceType()
{
  return m_Type;
}

void InstanceSet::AddInstance(Luna::Instance* i)
{
  if (m_Instances.empty())
  {
    m_Type->AddSet(this);
  }

  Insert<S_InstanceDumbPtr>::Result inserted = m_Instances.insert( i );
  NOC_ASSERT( *inserted.first == i );

  if ( !i->IsTransient() )
  {
    m_InstanceAdded.Raise( InstanceSetChangeArgs( this, i ) );
  }

  if ( i->UseAutoName() )
  {
    i->Rename(i->GenerateName());
  }
}

void InstanceSet::RemoveInstance(Luna::Instance* i)
{
  m_Instances.erase(i);

  m_InstanceRemoved.Raise( InstanceSetChangeArgs( this, i ) );

  // Order is important here... don't remove the set until notifying listeners that the instance has been removed.
  if (m_Instances.empty())
  {
    m_Type->RemoveSet(this);
  }
}

bool InstanceSet::ContainsInstance(Luna::Instance* i)
{
  return m_Instances.find(i) != m_Instances.end();
}

const S_InstanceDumbPtr& InstanceSet::GetInstances() const
{
  return m_Instances;
}
