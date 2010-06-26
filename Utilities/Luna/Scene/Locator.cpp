#include "Precompile.h"
#include "Locator.h"

#include "Scene.h"
#include "SceneManager.h"

#include "LocatorType.h"

#include "Core/Enumerator.h"
#include "Application/UI/ImageManager.h"

#include "PrimitiveLocator.h"
#include "PrimitiveCube.h"

#include "Pipeline/Content/Nodes/Instance/Locator.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::Locator);

void Locator::InitializeType()
{
  Reflect::RegisterClass< Luna::Locator >( "Luna::Locator" );

  Enumerator::InitializePanel( "Locator", CreatePanelSignature::Delegate( &Locator::CreatePanel ) );
}

void Locator::CleanupType()
{
  Reflect::UnregisterClass< Luna::Locator >();
}

Locator::Locator(Luna::Scene* scene)
: Luna::Instance (scene, new Content::Locator ())
{

}

Locator::Locator(Luna::Scene* scene, Content::Locator* locator)
: Luna::Instance ( scene, locator )
{

}

Locator::~Locator()
{

}

i32 Locator::GetImageIndex() const
{
  return Nocturnal::GlobalImageManager().GetImageIndex( "locator.png" );
}

std::string Locator::GetApplicationTypeName() const
{
  return "Locator";
}

SceneNodeTypePtr Locator::CreateNodeType( Luna::Scene* scene ) const
{
  // Overridden to create an locator-specific type
  Luna::LocatorType* nodeType = new Luna::LocatorType( scene, GetType() );

  // Set the image index (usually this is handled by the base class, but we aren't calling the base)
  nodeType->SetImageIndex( GetImageIndex() );

  return nodeType;
}

int Locator::GetShape() const
{
  return GetPackage< Content::Locator >()->m_Shape;
}

void Locator::SetShape( int shape )
{
  GetPackage< Content::Locator >()->m_Shape = static_cast< Content::LocatorShape > (shape);
}

void Locator::Evaluate(GraphDirection direction)
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
        Luna::LocatorType* type = Reflect::AssertCast<Luna::LocatorType>(m_NodeType);

        const Luna::Primitive* prim = type->GetShape( GetPackage< Content::Locator >()->m_Shape );
        if (prim)
        {
          m_ObjectBounds.Merge(prim->GetBounds());
        }
      }

      break;
    }
  }
}

void Locator::Render( RenderVisitor* render )
{
  const Content::Locator* package = GetPackage< Content::Locator >();

  // shape is drawn non-normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = render->State().m_Matrix;
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &Locator::DrawShape;

    if ( package->m_TransparentOverride ? package->m_Transparent : Reflect::AssertCast<Luna::InstanceType>( m_NodeType )->IsTransparent() )
    {
      entry->m_Flags |= RenderFlags::DistanceSort;
    }
  }

  // don't call __super here, it will draw big ass axes
  Luna::HierarchyNode::Render( render );
}

void Locator::DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::Locator* locator = Reflect::ConstAssertCast<Luna::Locator>( object );

  const Luna::LocatorType* type = Reflect::ConstAssertCast<Luna::LocatorType>( locator->GetNodeType() );

  const Content::Locator* package = locator->GetPackage< Content::Locator >();

  locator->SetMaterial( type->GetMaterial() );

  const Luna::Primitive* prim = type->GetShape( package->m_Shape );
  if (prim)
  {
    prim->Draw( args, package->m_SolidOverride ? &package->m_Solid : NULL, package->m_TransparentOverride ? &package->m_Transparent : NULL );
  }
}

bool Locator::Pick( PickVisitor* pick )
{
  bool result = false;

  const Luna::LocatorType* type = Reflect::AssertCast<Luna::LocatorType>(m_NodeType);

  const Content::Locator* package = GetPackage< Content::Locator >();

  pick->SetCurrentObject (this, pick->State().m_Matrix);

  const Luna::Primitive* prim = type->GetShape( package->m_Shape );
  if (prim)
  {
    result |= prim->Pick(pick, package->m_SolidOverride ? &package->m_Solid : NULL);
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Luna::Locator.
// 
bool Locator::ValidatePanel(const std::string& name)
{
  if (name == "Locator")
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

void Locator::CreatePanel( CreatePanelArgs& args )
{
  args.m_Enumerator->PushPanel("Locator", true);
  {
    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Shape");

      Inspect::Choice* choice = args.m_Enumerator->AddChoice<Luna::Locator, int>(args.m_Selection, &Locator::GetShape, &Locator::SetShape);
      choice->SetDropDown( true );
      Inspect::V_Item items;

      {
        std::ostringstream str;
        str << Content::LocatorShapes::Cross;
        items.push_back( Inspect::Item( "Cross", str.str() ) );
      }

      {
        std::ostringstream str;
        str << Content::LocatorShapes::Cube;
        items.push_back( Inspect::Item( "Cube", str.str() ) );
      }

      choice->SetItems( items );
    }
    args.m_Enumerator->Pop();
  }
  args.m_Enumerator->Pop();
}