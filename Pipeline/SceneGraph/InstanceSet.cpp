#include "PipelinePch.h"
#include "InstanceSet.h"
#include "Instance.h"
#include "InstanceType.h"

#include "Foundation/Container/Insert.h" 

using Helium::Insert; 

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(SceneGraph::InstanceSet);

void InstanceSet::InitializeType()
{
  Reflect::RegisterClassType< SceneGraph::InstanceSet >( TXT( "SceneGraph::InstanceSet" ) );
}

void SceneGraph::InstanceSet::CleanupType()
{
  Reflect::UnregisterClassType< SceneGraph::InstanceSet >();
}

InstanceSet::InstanceSet(SceneGraph::InstanceType* type)
: m_Type (type)
{

}

InstanceSet::~InstanceSet()
{

}

SceneGraph::InstanceType* InstanceSet::GetInstanceClass()
{
  return m_Type;
}

void InstanceSet::AddInstance(SceneGraph::Instance* i)
{
  if (m_Instances.empty())
  {
    m_Type->AddSet(this);
  }

  StdInsert<S_InstanceDumbPtr>::Result inserted = m_Instances.insert( i );
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

void InstanceSet::RemoveInstance(SceneGraph::Instance* i)
{
  m_Instances.erase(i);

  m_InstanceRemoved.Raise( InstanceSetChangeArgs( this, i ) );

  // Order is important here... don't remove the set until notifying listeners that the instance has been removed.
  if (m_Instances.empty())
  {
    m_Type->RemoveSet(this);
  }
}

bool InstanceSet::ContainsInstance(SceneGraph::Instance* i)
{
  return m_Instances.find(i) != m_Instances.end();
}

const S_InstanceDumbPtr& InstanceSet::GetInstances() const
{
  return m_Instances;
}
