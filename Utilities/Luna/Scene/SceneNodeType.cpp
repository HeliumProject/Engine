#include "Precompile.h"
#include "SceneNodeType.h"
#include "SceneNode.h"
#include "Common/Container/Insert.h" 
#include "Asset/SceneManifest.h"

using namespace Luna;

LUNA_DEFINE_TYPE( Luna::SceneNodeType );

void SceneNodeType::InitializeType()
{
  Reflect::RegisterClass< Luna::SceneNodeType >( "Luna::SceneNodeType" );
}

void SceneNodeType::CleanupType()
{
  Reflect::UnregisterClass< Luna::SceneNodeType >();
}

SceneNodeType::SceneNodeType(Luna::Scene* scene, i32 instanceType)
: m_Scene( scene )
, m_InstanceType ( instanceType )
, m_ImageIndex( -1 )
{

}

SceneNodeType::~SceneNodeType()
{

}

Luna::Scene* SceneNodeType::GetScene()
{
  return m_Scene;
}

const std::string& SceneNodeType::GetName() const
{
  return m_Name;
}

void SceneNodeType::SetName( const std::string& name )
{
  m_Name = name;
}

i32 SceneNodeType::GetImageIndex() const
{
  return m_ImageIndex;
}

void SceneNodeType::SetImageIndex( i32 index )
{
  m_ImageIndex = index;
}

void SceneNodeType::Reset()
{
  m_Instances.clear();
}

void SceneNodeType::AddInstance(SceneNodePtr n)
{
  n->SetNodeType( this );

  Nocturnal::Insert<HM_SceneNodeSmartPtr>::Result inserted = m_Instances.insert( HM_SceneNodeSmartPtr::value_type( n->GetID(), n ) );

  if (!inserted.second)
  {
    NOC_BREAK();
  }

  if ( !n->IsTransient() )
  {
    m_NodeAdded.Raise( n.Ptr() );
  }
}

void SceneNodeType::RemoveInstance(SceneNodePtr n)
{
  m_NodeRemoved.Raise( n.Ptr() );

  m_Instances.erase( n->GetID() );

  n->SetNodeType( NULL );
}

const HM_SceneNodeSmartPtr& SceneNodeType::GetInstances() const
{
  return m_Instances;
}

i32 SceneNodeType::GetInstanceType() const
{
  return m_InstanceType;
}

void SceneNodeType::PopulateManifest( Asset::SceneManifest* manifest ) const
{
  HM_SceneNodeSmartPtr::const_iterator instItr = m_Instances.begin();
  HM_SceneNodeSmartPtr::const_iterator instEnd = m_Instances.end();
  for ( ; instItr != instEnd; ++instItr )
  {
    instItr->second->PopulateManifest(manifest);
  }
}