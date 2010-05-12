#include "Precompile.h"
#include "Controller.h"

#include "Scene.h"
#include "SceneManager.h"

#include "ControllerType.h"

#include "Core/Enumerator.h"
#include "UIToolKit/ImageManager.h"

#include "PrimitivePointer.h"
#include "PrimitiveCube.h"

#include "Content/Controller.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::Controller);

void Controller::InitializeType()
{
  Reflect::RegisterClass< Luna::Controller >( "Luna::Controller" );
}

void Controller::CleanupType()
{
  Reflect::UnregisterClass< Luna::Controller >();
}

Controller::Controller(Luna::Scene* scene)
: Luna::Instance (scene, new Content::Controller ())
{

}

Controller::Controller(Luna::Scene* scene, Content::Controller* controller)
: Luna::Instance ( scene, controller )
{

}

Controller::~Controller()
{

}

i32 Controller::GetImageIndex() const
{
  return UIToolKit::GlobalImageManager().GetImageIndex( "controller_16.png" );
}

std::string Controller::GetApplicationTypeName() const
{
  return "Controller";
}

SceneNodeTypePtr Controller::CreateNodeType( Luna::Scene* scene ) const
{
  // Overridden to create an controller-specific type
  Luna::ControllerType* nodeType = new Luna::ControllerType( scene, GetType() );

  // Set the image index (usually this is handled by the base class, but we aren't calling the base)
  nodeType->SetImageIndex( GetImageIndex() );

  return nodeType;
}

void Controller::Evaluate(GraphDirection direction)
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
        Luna::ControllerType* type = Reflect::AssertCast<Luna::ControllerType>(m_NodeType);

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

void Controller::Render( RenderVisitor* render )
{
  const Content::Controller* package = GetPackage< Content::Controller >();

  // pointer is drawn normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = render->State().m_Matrix.Normalized();
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &Controller::DrawPointer;
  }

  // shape is drawn non-normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = render->State().m_Matrix;
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &Controller::DrawBounds;

    if ( package->m_TransparentOverride ? package->m_Transparent : Reflect::AssertCast<Luna::ControllerType>( m_NodeType )->IsTransparent() )
    {
      entry->m_Flags |= RenderFlags::DistanceSort;
    }
  }

  // don't call __super here, it will draw big ass axes
  Luna::HierarchyNode::Render( render );
}

void Controller::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::HierarchyNode* node = Reflect::ConstAssertCast<Luna::HierarchyNode>( object );

  const Luna::Controller* controller = Reflect::ConstAssertCast<Luna::Controller>( node );

  const Luna::ControllerType* type = Reflect::ConstAssertCast<Luna::ControllerType>( controller->GetNodeType() );

  node->SetMaterial( type->GetMaterial() );

  // draw type pointer
  type->GetPointer()->Draw( args );
}

void Controller::DrawBounds( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::Controller* controller = Reflect::ConstAssertCast<Luna::Controller>( object );

  const Luna::ControllerType* type = Reflect::ConstAssertCast<Luna::ControllerType>( controller->GetNodeType() );

  const Content::Controller* package = controller->GetPackage< Content::Controller >();

  controller->SetMaterial( type->GetMaterial() );

  // draw shape primitive
  type->GetBounds()->Draw( args, package->m_SolidOverride ? &package->m_Solid : NULL, package->m_TransparentOverride ? &package->m_Transparent : NULL );
}

bool Controller::Pick( PickVisitor* pick )
{
  bool result = false;

  const Luna::ControllerType* type = Reflect::AssertCast<Luna::ControllerType>(m_NodeType);

  const Content::Controller* package = GetPackage< Content::Controller >();

  pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());
  result |= type->GetPointer()->Pick (pick);
  
  pick->SetCurrentObject (this, pick->State().m_Matrix);
  result |= type->GetBounds()->Pick(pick, package->m_SolidOverride ? &package->m_Solid : NULL);

  return result;
}