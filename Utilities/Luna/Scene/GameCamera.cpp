#include "Precompile.h"
#include "GameCamera.h"

#include "Scene.h"
#include "SceneManager.h"

#include "GameCameraType.h"

#include "Core/Enumerator.h"
#include "UIToolKit/ImageManager.h"

#include "PrimitivePointer.h"
#include "PrimitiveCube.h"

#include "Content/GameCamera.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::GameCamera);

void GameCamera::InitializeType()
{
  Reflect::RegisterClass< Luna::GameCamera >( "Luna::GameCamera" );
}

void GameCamera::CleanupType()
{
  Reflect::UnregisterClass< Luna::GameCamera >();
}

GameCamera::GameCamera(Luna::Scene* scene)
: Luna::Instance (scene, new Content::GameCamera ())
{

}

GameCamera::GameCamera(Luna::Scene* scene, Content::GameCamera* controller)
: Luna::Instance ( scene, controller )
{

}

GameCamera::~GameCamera()
{

}

i32 GameCamera::GetImageIndex() const
{
  return UIToolKit::GlobalImageManager().GetImageIndex( "game_camera_16.png" );
}

std::string GameCamera::GetApplicationTypeName() const
{
  return "GameCamera";
}

SceneNodeTypePtr GameCamera::CreateNodeType( Luna::Scene* scene ) const
{
  // Overridden to create an controller-specific type
  Luna::GameCameraType* nodeType = new Luna::GameCameraType( scene, GetType() );

  // Set the image index (usually this is handled by the base class, but we aren't calling the base)
  nodeType->SetImageIndex( GetImageIndex() );

  return nodeType;
}

void GameCamera::Evaluate(GraphDirection direction)
{
  __super::Evaluate(direction);

  switch (direction)
  {
  case GraphDirections::Downstream:
    {
      // start the box from scratch
      m_ObjectBounds.Reset();

      // merge type pointer into our bounding box
      if (m_NodeType)
      {
        Luna::GameCameraType* type = Reflect::AssertCast<Luna::GameCameraType>(m_NodeType);

        Math::AlignedBox box (type->GetPointer()->GetBounds());

        Math::Scale scale;
        Math::Matrix3 rotate;
        Math::Vector3 translate;
        m_InverseGlobalTransform.Decompose (scale, rotate, translate);

        //  this will compensate for the normalized render of the pointer
        box.Transform (Math::Matrix4 (scale));

        m_ObjectBounds.Merge( box );

        m_ObjectBounds.Merge(type->GetBounds()->GetBounds());
      }

      break;
    }
  }

}

void GameCamera::Render( RenderVisitor* render )
{
  const Content::GameCamera* package = GetPackage< Content::GameCamera >();

  // pointer is drawn normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = render->State().m_Matrix.Normalized();
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &GameCamera::DrawPointer;
  }

  // shape is drawn non-normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = render->State().m_Matrix;
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &GameCamera::DrawBounds;

    if ( package->m_TransparentOverride ? package->m_Transparent : Reflect::AssertCast<Luna::GameCameraType>( m_NodeType )->IsTransparent() )
    {
      entry->m_Flags |= RenderFlags::DistanceSort;
    }
  }

  // don't call __super here, it will draw big ass axes
  Luna::HierarchyNode::Render( render );
}

void GameCamera::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::HierarchyNode* node = Reflect::ConstAssertCast<Luna::HierarchyNode>( object );

  const Luna::GameCamera* controller = Reflect::ConstAssertCast<Luna::GameCamera>( node );

  const Luna::GameCameraType* type = Reflect::ConstAssertCast<Luna::GameCameraType>( controller->GetNodeType() );

  node->SetMaterial( type->GetMaterial() );

  // draw type pointer
  type->GetPointer()->Draw( args );
}

void GameCamera::DrawBounds( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::GameCamera* controller = Reflect::ConstAssertCast<Luna::GameCamera>( object );

  const Luna::GameCameraType* type = Reflect::ConstAssertCast<Luna::GameCameraType>( controller->GetNodeType() );

  const Content::GameCamera* package = controller->GetPackage< Content::GameCamera >();

  controller->SetMaterial( type->GetMaterial() );

  // draw shape primitive
  type->GetBounds()->Draw( args, package->m_SolidOverride ? &package->m_Solid : NULL, package->m_TransparentOverride ? &package->m_Transparent : NULL );
}

bool GameCamera::Pick( PickVisitor* pick )
{
  bool result = false;

  const Luna::GameCameraType* type = Reflect::AssertCast<Luna::GameCameraType>(m_NodeType);

  const Content::GameCamera* package = GetPackage< Content::GameCamera >();

  pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());
  result |= type->GetPointer()->Pick (pick);
  
  pick->SetCurrentObject (this, pick->State().m_Matrix);
  result |= type->GetBounds()->Pick(pick, package->m_SolidOverride ? &package->m_Solid : NULL);

  return result;
}
