#include "Precompile.h"
#include "Locator.h"

#include "Editor/Scene/Scene.h"
#include "Editor/Scene/SceneManager.h"

#include "LocatorType.h"

#include "Editor/PropertiesGenerator.h"
#include "Application/UI/ArtProvider.h"

#include "PrimitiveLocator.h"
#include "PrimitiveCube.h"

#include "Core/Content/Nodes/Locator.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Editor;

// RTTI
EDITOR_DEFINE_TYPE(Editor::Locator);

void Locator::InitializeType()
{
  Reflect::RegisterClass< Editor::Locator >( TXT( "Editor::Locator" ) );

  PropertiesGenerator::InitializePanel( TXT( "Locator" ), CreatePanelSignature::Delegate( &Locator::CreatePanel ) );
}

void Locator::CleanupType()
{
  Reflect::UnregisterClass< Editor::Locator >();
}

Locator::Locator(Editor::Scene* scene)
: Editor::Instance (scene, new Content::Locator ())
{

}

Locator::Locator(Editor::Scene* scene, Content::Locator* locator)
: Editor::Instance ( scene, locator )
{

}

Locator::~Locator()
{

}

i32 Locator::GetImageIndex() const
{
  return Helium::GlobalFileIconsTable().GetIconID( TXT( "locator" ) );
}

tstring Locator::GetApplicationTypeName() const
{
  return TXT( "Locator" );
}

SceneNodeTypePtr Locator::CreateNodeType( Editor::Scene* scene ) const
{
  // Overridden to create an locator-specific type
  Editor::LocatorType* nodeType = new Editor::LocatorType( scene, GetType() );

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
        Editor::LocatorType* type = Reflect::AssertCast<Editor::LocatorType>(m_NodeType);

        const Editor::Primitive* prim = type->GetShape( GetPackage< Content::Locator >()->m_Shape );
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

    if ( package->m_TransparentOverride ? package->m_Transparent : Reflect::AssertCast<Editor::InstanceType>( m_NodeType )->IsTransparent() )
    {
      entry->m_Flags |= RenderFlags::DistanceSort;
    }
  }

  // don't call __super here, it will draw big ass axes
  Editor::HierarchyNode::Render( render );
}

void Locator::DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Editor::Locator* locator = Reflect::ConstAssertCast<Editor::Locator>( object );

  const Editor::LocatorType* type = Reflect::ConstAssertCast<Editor::LocatorType>( locator->GetNodeType() );

  const Content::Locator* package = locator->GetPackage< Content::Locator >();

  locator->SetMaterial( type->GetMaterial() );

  const Editor::Primitive* prim = type->GetShape( package->m_Shape );
  if (prim)
  {
    prim->Draw( args, package->m_SolidOverride ? &package->m_Solid : NULL, package->m_TransparentOverride ? &package->m_Transparent : NULL );
  }
}

bool Locator::Pick( PickVisitor* pick )
{
  bool result = false;

  const Editor::LocatorType* type = Reflect::AssertCast<Editor::LocatorType>(m_NodeType);

  const Content::Locator* package = GetPackage< Content::Locator >();

  pick->SetCurrentObject (this, pick->State().m_Matrix);

  const Editor::Primitive* prim = type->GetShape( package->m_Shape );
  if (prim)
  {
    result |= prim->Pick(pick, package->m_SolidOverride ? &package->m_Solid : NULL);
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Editor::Locator.
// 
bool Locator::ValidatePanel(const tstring& name)
{
  if (name == TXT( "Locator" ) )
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

void Locator::CreatePanel( CreatePanelArgs& args )
{
  args.m_Generator->PushPanel( TXT( "Locator" ), true);
  {
    args.m_Generator->PushContainer();
    {
      args.m_Generator->AddLabel( TXT( "Shape" ) );

      Inspect::Choice* choice = args.m_Generator->AddChoice<Editor::Locator, int>(args.m_Selection, &Locator::GetShape, &Locator::SetShape);
      choice->SetDropDown( true );
      Inspect::V_Item items;

      {
        tostringstream str;
        str << Content::LocatorShapes::Cross;
        items.push_back( Inspect::Item( TXT( "Cross" ), str.str() ) );
      }

      {
        tostringstream str;
        str << Content::LocatorShapes::Cube;
        items.push_back( Inspect::Item( TXT( "Cube" ), str.str() ) );
      }

      choice->SetItems( items );
    }
    args.m_Generator->Pop();
  }
  args.m_Generator->Pop();
}