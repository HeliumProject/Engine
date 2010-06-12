#include "Precompile.h"
#include "JointTransform.h"

#include "Content/JointTransform.h"
#include "math/EulerAngles.h"

#include "UIToolKit/ImageManager.h"
#include "Color.h"
#include "PrimitiveAxes.h"
#include "PrimitiveRings.h"

#include "Application/Undo/PropertyCommand.h"
#include "HierarchyNodeType.h"
#include "Scene.h"

using namespace Math;
using namespace Luna;

// Initialize static members
static D3DMATERIAL9 g_JointTransformMaterial;

LUNA_DEFINE_TYPE( Luna::JointTransform );

void JointTransform::InitializeType()
{
  Reflect::RegisterClass< Luna::JointTransform >( "Luna::JointTransform" );

  ZeroMemory(&g_JointTransformMaterial, sizeof(g_JointTransformMaterial));
  g_JointTransformMaterial.Ambient = Luna::Color::DARKGREEN;
  g_JointTransformMaterial.Diffuse = Luna::Color::BLACK;
  g_JointTransformMaterial.Specular = Luna::Color::BLACK;
}

void JointTransform::CleanupType()
{
  Reflect::UnregisterClass< Luna::JointTransform >();
}

JointTransform::JointTransform( Luna::Scene* scene, Content::JointTransform* joint ) : Luna::Transform( scene, joint )
{
  Luna::PrimitiveRings* rings = static_cast< Luna::PrimitiveRings* >( m_Scene->GetView()->GetGlobalPrimitive( GlobalPrimitives::JointRings ) );
  m_ObjectBounds.minimum = Math::Vector3(-rings->m_Radius, -rings->m_Radius, -rings->m_Radius);
  m_ObjectBounds.maximum = Math::Vector3(rings->m_Radius, rings->m_Radius, rings->m_Radius);
}

JointTransform::~JointTransform()
{

}

i32 JointTransform::GetImageIndex() const
{
  return UIToolKit::GlobalImageManager().GetImageIndex( "joint_16.png" );
}

std::string JointTransform::GetApplicationTypeName() const
{
  return "Joint";
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
  Luna::HierarchyNode::Render( render );
}

void JointTransform::DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::HierarchyNode* node = Reflect::ConstAssertCast<Luna::HierarchyNode>( object );

  const Luna::JointTransform* joint = Reflect::ConstAssertCast<Luna::JointTransform>( node );

  joint->SetMaterial( g_JointTransformMaterial );

  node->GetScene()->GetView()->GetGlobalPrimitive( GlobalPrimitives::JointAxes )->Draw( args );
  node->GetScene()->GetView()->GetGlobalPrimitive( GlobalPrimitives::JointRings )->Draw( args );
}

void JointTransform::DrawSelected( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::HierarchyNode* node = Reflect::ConstAssertCast<Luna::HierarchyNode>( object );

  const Luna::JointTransform* joint = Reflect::ConstAssertCast<Luna::JointTransform>( node );

  joint->SetMaterial( g_JointTransformMaterial );

  node->GetScene()->GetView()->GetGlobalPrimitive( GlobalPrimitives::JointAxes )->Draw( args );
  node->GetScene()->GetView()->GetGlobalPrimitive( GlobalPrimitives::JointRings )->Draw( args );
}

bool JointTransform::Pick( PickVisitor* pick )
{
  pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());

  return pick->PickPoint(Vector3::Zero, static_cast< Luna::PrimitiveAxes* >( m_Scene->GetView()->GetGlobalPrimitive( GlobalPrimitives::JointAxes ) )->m_Length);
}