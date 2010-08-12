/*#include "Precompile.h"*/
#include "InstanceSet.h"
#include "Instance.h"
#include "InstanceType.h"

#include "Foundation/Container/Insert.h" 

using Helium::Insert; 

using namespace Helium;
using namespace Helium::Core;

SCENE_DEFINE_TYPE(Core::InstanceSet);

void InstanceSet::InitializeType()
{
  Reflect::RegisterClassType< Core::InstanceSet >( TXT( "Core::InstanceSet" ) );
}

void Core::InstanceSet::CleanupType()
{
  Reflect::UnregisterClassType< Core::InstanceSet >();
}

InstanceSet::InstanceSet(Core::InstanceType* type)
: m_Type (type)
{

}

InstanceSet::~InstanceSet()
{

}

Core::InstanceType* InstanceSet::GetInstanceType()
{
  return m_Type;
}

void InstanceSet::AddInstance(Core::Instance* i)
{
  if (m_Instances.empty())
  {
    m_Type->AddSet(this);
  }

  Insert<S_InstanceDumbPtr>::Result inserted = m_Instances.insert( i );
  HELIUM_ASSERT( *inserted.first == i );

  if ( !i->IsTransient() )
  {
    m_InstanceAdded.Raise( InstanceSetChangeArgs( this, i ) );
  }

  if ( i->UseAutoName() )
  {
    i->Rename(i->GenerateName());
  }
}

void InstanceSet::RemoveInstance(Core::Instance* i)
{
  m_Instances.erase(i);

  m_InstanceRemoved.Raise( InstanceSetChangeArgs( this, i ) );

  // Order is important here... don't remove the set until notifying listeners that the instance has been removed.
  if (m_Instances.empty())
  {
    m_Type->RemoveSet(this);
  }
}

bool InstanceSet::ContainsInstance(Core::Instance* i)
{
  return m_Instances.find(i) != m_Instances.end();
}

const S_InstanceDumbPtr& InstanceSet::GetInstances() const
{
  return m_Instances;
}
