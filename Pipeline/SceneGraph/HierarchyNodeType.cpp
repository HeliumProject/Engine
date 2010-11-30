/*#include "Precompile.h"*/
#include "HierarchyNodeType.h"

#include "Foundation/Reflect/Object.h" 

#include "Pipeline/SceneGraph/Scene.h"
#include "HierarchyNode.h"
#include "Color.h"
#include "Primitive.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT( SceneGraph::HierarchyNodeType );

void HierarchyNodeType::InitializeType()
{
  Reflect::RegisterClassType< SceneGraph::HierarchyNodeType >( TXT( "SceneGraph::HierarchyNodeType" ) );
}

void SceneGraph::HierarchyNodeType::CleanupType()
{
  Reflect::UnregisterClassType< SceneGraph::HierarchyNodeType >();
}

HierarchyNodeType::HierarchyNodeType( SceneGraph::Scene* scene, const Reflect::Class* instanceClass )
: SceneGraph::SceneNodeType( scene, instanceClass )
{
  int index = -1;

  m_Visible = true;
  m_Selectable = true;

  ZeroMemory(&m_WireMaterial, sizeof(m_WireMaterial));
  m_WireMaterial.Ambient = SceneGraph::Color::BLACK;
  m_WireMaterial.Diffuse = SceneGraph::Color::BLACK;
  m_WireMaterial.Specular = SceneGraph::Color::BLACK;

  ZeroMemory(&m_SolidMaterial, sizeof(m_SolidMaterial));
  m_SolidMaterial.Ambient = SceneGraph::Color::DARKGRAY;
  m_SolidMaterial.Diffuse = SceneGraph::Color::DARKGRAY;
  m_SolidMaterial.Specular = SceneGraph::Color::DARKGRAY;
}

void HierarchyNodeType::Create()
{

}

void HierarchyNodeType::Delete()
{

}

bool HierarchyNodeType::IsVisible() const
{
  return m_Visible;
}

void HierarchyNodeType::SetVisible(bool value)
{
  m_Visible = value;

  HM_SceneNodeSmartPtr::const_iterator itr = m_Instances.begin();
  HM_SceneNodeSmartPtr::const_iterator end = m_Instances.end();
  for ( ; itr != end; ++itr )
  {
    itr->second->Dirty();
  }
}

bool HierarchyNodeType::IsSelectable() const
{
  return m_Selectable;
}

void HierarchyNodeType::SetSelectable(bool value)
{
  m_Selectable = value;

  HM_SceneNodeSmartPtr::const_iterator itr = m_Instances.begin();
  HM_SceneNodeSmartPtr::const_iterator end = m_Instances.end();
  for ( ; itr != end; ++itr )
  {
    itr->second->Dirty();
  }
}

bool HierarchyNodeType::IsTransparent()
{
  return false;
}
