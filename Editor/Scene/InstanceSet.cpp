#include "Precompile.h"
#include "InstanceSet.h"
#include "Instance.h"
#include "InstanceType.h"

#include "Foundation/Container/Insert.h" 

using Helium::Insert; 
using namespace Editor;

LUNA_DEFINE_TYPE(Editor::InstanceSet);

void InstanceSet::InitializeType()
{
  Reflect::RegisterClass< Editor::InstanceSet >( TXT( "Editor::InstanceSet" ) );
}

void Editor::InstanceSet::CleanupType()
{
  Reflect::UnregisterClass< Editor::InstanceSet >();
}

InstanceSet::InstanceSet(Editor::InstanceType* type)
: m_Type (type)
{

}

InstanceSet::~InstanceSet()
{

}

Editor::InstanceType* InstanceSet::GetInstanceType()
{
  return m_Type;
}

void InstanceSet::AddInstance(Editor::Instance* i)
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

void InstanceSet::RemoveInstance(Editor::Instance* i)
{
  m_Instances.erase(i);

  m_InstanceRemoved.Raise( InstanceSetChangeArgs( this, i ) );

  // Order is important here... don't remove the set until notifying listeners that the instance has been removed.
  if (m_Instances.empty())
  {
    m_Type->RemoveSet(this);
  }
}

bool InstanceSet::ContainsInstance(Editor::Instance* i)
{
  return m_Instances.find(i) != m_Instances.end();
}

const S_InstanceDumbPtr& InstanceSet::GetInstances() const
{
  return m_Instances;
}
