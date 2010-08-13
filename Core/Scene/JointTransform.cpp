/*#include "Precompile.h"*/
#include "JointTransform.h"

#include "Core/Content/Nodes/ContentJointTransform.h"
#include "Foundation/Math/EulerAngles.h"

#include "Application/UI/ArtProvider.h"
#include "Color.h"
#include "PrimitiveAxes.h"
#include "PrimitiveRings.h"

#include "Application/Undo/PropertyCommand.h"
#include "HierarchyNodeType.h"
#include "Core/Scene/Scene.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Core;

// Initialize static members
static D3DMATERIAL9 g_JointTransformMaterial;

SCENE_DEFINE_TYPE( Core::JointTransform );

void JointTransform::InitializeType()
{
  Reflect::RegisterClassType< Core::JointTransform >( TXT( "Core::JointTransform" ) );

  ZeroMemory(&g_JointTransformMaterial, sizeof(g_JointTransformMaterial));
  g_JointTransformMaterial.Ambient = Core::Color::DARKGREEN;
  g_JointTransformMaterial.Diffuse = Core::Color::BLACK;
  g_JointTransformMaterial.Specular = Core::Color::BLACK;
}

void JointTransform::CleanupType()
{
  Reflect::UnregisterClassType< Core::JointTransform >();
}

JointTransform::JointTransform( Core::Scene* scene, Content::JointTransform* joint ) : Core::Transform( scene, joint )
{
  Core::PrimitiveRings* rings = static_cast< Core::PrimitiveRings* >( m_Owner->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointRings ) );
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

  if (IsSelected() && m_Owner->IsFocused() )
  {
    entry->m_Draw = &JointTransform::DrawSelected;
  }
  else
  {
    entry->m_Draw = &JointTransform::DrawNormal;
  }

  // don't call __super here, it will draw big ass axes
  Core::HierarchyNode::Render( render );
}

void JointTransform::DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Core::HierarchyNode* node = Reflect::ConstAssertCast<Core::HierarchyNode>( object );

  const Core::JointTransform* joint = Reflect::ConstAssertCast<Core::JointTransform>( node );

  joint->SetMaterial( g_JointTransformMaterial );

  node->GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointAxes )->Draw( args );
  node->GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointRings )->Draw( args );
}

void JointTransform::DrawSelected( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Core::HierarchyNode* node = Reflect::ConstAssertCast<Core::HierarchyNode>( object );

  const Core::JointTransform* joint = Reflect::ConstAssertCast<Core::JointTransform>( node );

  joint->SetMaterial( g_JointTransformMaterial );

  node->GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointAxes )->Draw( args );
  node->GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointRings )->Draw( args );
}

bool JointTransform::Pick( PickVisitor* pick )
{
  pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());

  return pick->PickPoint(Vector3::Zero, static_cast< Core::PrimitiveAxes* >( m_Owner->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointAxes ) )->m_Length);
}