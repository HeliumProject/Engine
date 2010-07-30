#include "Precompile.h"
#include "JointTransform.h"

#include "Pipeline/Content/Nodes/JointTransform.h"
#include "Foundation/Math/EulerAngles.h"

#include "Application/UI/ArtProvider.h"
#include "Color.h"
#include "PrimitiveAxes.h"
#include "PrimitiveRings.h"

#include "Application/Undo/PropertyCommand.h"
#include "HierarchyNodeType.h"
#include "Scene.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Editor;

// Initialize static members
static D3DMATERIAL9 g_JointTransformMaterial;

EDITOR_DEFINE_TYPE( Editor::JointTransform );

void JointTransform::InitializeType()
{
  Reflect::RegisterClass< Editor::JointTransform >( TXT( "Editor::JointTransform" ) );

  ZeroMemory(&g_JointTransformMaterial, sizeof(g_JointTransformMaterial));
  g_JointTransformMaterial.Ambient = Editor::Color::DARKGREEN;
  g_JointTransformMaterial.Diffuse = Editor::Color::BLACK;
  g_JointTransformMaterial.Specular = Editor::Color::BLACK;
}

void JointTransform::CleanupType()
{
  Reflect::UnregisterClass< Editor::JointTransform >();
}

JointTransform::JointTransform( Editor::Scene* scene, Content::JointTransform* joint ) : Editor::Transform( scene, joint )
{
  Editor::PrimitiveRings* rings = static_cast< Editor::PrimitiveRings* >( m_Scene->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointRings ) );
  m_ObjectBounds.minimum = Math::Vector3(-rings->m_Radius, -rings->m_Radius, -rings->m_Radius);
  m_ObjectBounds.maximum = Math::Vector3(rings->m_Radius, rings->m_Radius, rings->m_Radius);
}

JointTransform::~JointTransform()
{

}

i32 JointTransform::GetImageIndex() const
{
  return Helium::GlobalFileIconsTable().GetIconID( TXT( "joint" ) );
}

tstring JointTransform::GetApplicationTypeName() const
{
  return TXT( "Joint" );
}

void JointTransform::Render( RenderVisitor* render )
{
  RenderEntry* entry = render->Allocate(this);

  entry->m_Location = render->State().m_Matrix.Normalized();
  entry->m_Center = m_ObjectBounds.Center();

  if (IsSelected() && m_Scene->IsCurrent())
  {
    entry->m_Draw = &JointTransform::DrawSelected;
  }
  else
  {
    entry->m_Draw = &JointTransform::DrawNormal;
  }

  // don't call __super here, it will draw big ass axes
  Editor::HierarchyNode::Render( render );
}

void JointTransform::DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Editor::HierarchyNode* node = Reflect::ConstAssertCast<Editor::HierarchyNode>( object );

  const Editor::JointTransform* joint = Reflect::ConstAssertCast<Editor::JointTransform>( node );

  joint->SetMaterial( g_JointTransformMaterial );

  node->GetScene()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointAxes )->Draw( args );
  node->GetScene()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointRings )->Draw( args );
}

void JointTransform::DrawSelected( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Editor::HierarchyNode* node = Reflect::ConstAssertCast<Editor::HierarchyNode>( object );

  const Editor::JointTransform* joint = Reflect::ConstAssertCast<Editor::JointTransform>( node );

  joint->SetMaterial( g_JointTransformMaterial );

  node->GetScene()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointAxes )->Draw( args );
  node->GetScene()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointRings )->Draw( args );
}

bool JointTransform::Pick( PickVisitor* pick )
{
  pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());

  return pick->PickPoint(Vector3::Zero, static_cast< Editor::PrimitiveAxes* >( m_Scene->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointAxes ) )->m_Length);
}