/*#include "Precompile.h"*/
#include "Volume.h"

#include "Core/Scene/Scene.h"
#include "Core/Scene/SceneManager.h"
#include "Core/Scene/VolumeType.h"
#include "Core/Content/Nodes/ContentVolume.h"
#include "Core/Scene/PropertiesGenerator.h"
#include "Core/Scene/PrimitiveCube.h"
#include "Core/Scene/PrimitiveCylinder.h"
#include "Core/Scene/PrimitiveSphere.h"
#include "Core/Scene/PrimitiveCapsule.h"
#include "Core/Scene/PrimitivePointer.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Core;

// RTTI
SCENE_DEFINE_TYPE(Core::Volume);

void Volume::InitializeType()
{
  Reflect::RegisterClassType< Core::Volume >( TXT( "Core::Volume" ) );

  PropertiesGenerator::InitializePanel( TXT( "Volume" ), CreatePanelSignature::Delegate( &Volume::CreatePanel ) );
}

void Volume::CleanupType()
{
  Reflect::UnregisterClassType< Core::Volume >();
}

Volume::Volume(Core::Scene* scene)
: Core::Instance (scene, new Content::Volume ())
{

}

Volume::Volume(Core::Scene* scene, Content::Volume* volume)
: Core::Instance ( scene, volume )
{

}

Volume::~Volume()
{

}

i32 Volume::GetImageIndex() const
{
  return -1; // Helium::GlobalFileIconsTable().GetIconID( TXT( "volume" ) );
}

tstring Volume::GetApplicationTypeName() const
{
  return TXT( "Volume" );
}

SceneNodeTypePtr Volume::CreateNodeType( Core::Scene* scene ) const
{
  // Overridden to create an volume-specific type
  Core::VolumeType* nodeType = new Core::VolumeType( scene, GetType() );

  // Set the image index (usually this is handled by the base class, but we aren't calling the base)
  nodeType->SetImageIndex( GetImageIndex() );

  return nodeType;
}

int Volume::GetShape() const
{
  return GetPackage< Content::Volume >()->m_Shape;
}

void Volume::SetShape( int shape )
{
  GetPackage< Content::Volume >()->m_Shape = static_cast< Content::VolumeShape > (shape);
}

void Volume::Evaluate(GraphDirection direction)
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
        Core::VolumeType* type = Reflect::AssertCast<Core::VolumeType>(m_NodeType);

        if ( IsPointerVisible() )
        {
          Math::AlignedBox box (type->GetPointer()->GetBounds());

          Math::Scale scale;
          Math::Matrix3 rotate;
          Math::Vector3 translate;
          m_InverseGlobalTransform.Decompose (scale, rotate, translate);

          //  this will compensate for the normalized render of the pointer
          box.Transform (Math::Matrix4 (scale));

          m_ObjectBounds.Merge( box );
        }

        const Core::Primitive* prim = type->GetShape( GetPackage< Content::Volume >()->m_Shape );
        if (prim)
        {
          m_ObjectBounds.Merge(prim->GetBounds());
        }
      }

      break;
    }
  }
}

void Volume::Render( RenderVisitor* render )
{
  const Content::Volume* package = GetPackage< Content::Volume >();

  // pointer is drawn normalized
  if ( IsPointerVisible() )
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = render->State().m_Matrix.Normalized();
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &Volume::DrawPointer;
  }

  // shape is drawn non-normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = render->State().m_Matrix;
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &Volume::DrawShape;

    if ( package->m_TransparentOverride ? package->m_Transparent : Reflect::AssertCast<Core::InstanceType>( m_NodeType )->IsTransparent() )
    {
      entry->m_Flags |= RenderFlags::DistanceSort;
    }
  }

  // don't call __super here, it will draw big ass axes
  Core::HierarchyNode::Render( render );
}

void Volume::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Core::Volume* volume = Reflect::ConstAssertCast<Core::Volume>( object );

  const Core::VolumeType* type = Reflect::ConstAssertCast<Core::VolumeType>( volume->GetNodeType() );

  volume->SetMaterial( type->GetMaterial() );

  // draw type pointer
  type->GetPointer()->Draw( args );
}

void Volume::DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Core::Volume* volume = Reflect::ConstAssertCast<Core::Volume>( object );

  const Core::VolumeType* type = Reflect::ConstAssertCast<Core::VolumeType>( volume->GetNodeType() );

  const Content::Volume* package = volume->GetPackage< Content::Volume >();

  volume->SetMaterial( type->GetMaterial() );

  const Core::Primitive* prim = type->GetShape( volume->GetPackage< Content::Volume >()->m_Shape );
  if (prim)
  {
    prim->Draw( args, package->m_SolidOverride ? &package->m_Solid : NULL, package->m_TransparentOverride ? &package->m_Transparent : NULL );
  }
}

bool Volume::Pick( PickVisitor* pick )
{
  bool result = false;

  Core::VolumeType* type = Reflect::AssertCast<Core::VolumeType>(m_NodeType);

  const Content::Volume* package = GetPackage< Content::Volume >();

  pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());
  result |= type->GetPointer()->Pick (pick);

  pick->SetCurrentObject (this, pick->State().m_Matrix);

  const Core::Primitive* prim = type->GetShape( GetPackage< Content::Volume >()->m_Shape );
  if (prim)
  {
    result |= prim->Pick(pick, package->m_SolidOverride ? &package->m_Solid : NULL);
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Core::Volume.
// 
bool Volume::ValidatePanel(const tstring& name)
{
  if (name == TXT( "Volume" ) )
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

void Volume::CreatePanel( CreatePanelArgs& args )
{
  args.m_Generator->PushContainer( TXT( "Volume" ) );
  {
    args.m_Generator->PushContainer();
    {
        static const tstring helpText = TXT( "Select the shape of this volume." );
      args.m_Generator->AddLabel( TXT( "Shape" ) )->a_HelpText.Set( helpText );

      Inspect::Choice* choice = args.m_Generator->AddChoice<Core::Volume, int>(args.m_Selection, &Volume::GetShape, &Volume::SetShape);
      choice->a_IsDropDown.Set( true );
      choice->a_HelpText.Set( helpText );
      std::vector< Inspect::ChoiceItem > items;

      {
        tostringstream str;
        str << Content::VolumeShapes::Cube;
        items.push_back( Inspect::ChoiceItem( TXT( "Cube" ), str.str() ) );
      }

      {
        tostringstream str;
        str << Content::VolumeShapes::Cylinder;
        items.push_back( Inspect::ChoiceItem( TXT( "Cylinder" ), str.str() ) );
      }

      {
        tostringstream str;
        str << Content::VolumeShapes::Sphere;
        items.push_back( Inspect::ChoiceItem( TXT( "Sphere" ), str.str() ) );
      }

      {
        tostringstream str;
        str << Content::VolumeShapes::Capsule;
        items.push_back( Inspect::ChoiceItem( TXT( "Capsule" ), str.str() ) );
      }

      choice->a_Items.Set( items );
    }
    args.m_Generator->Pop();
  }
  args.m_Generator->Pop();
}

bool Volume::IsPointerVisible() const
{
  HELIUM_ASSERT(m_VisibilityData); 
  return m_VisibilityData->GetShowPointer(); 
}

void Volume::SetPointerVisible(bool visible)
{
  HELIUM_ASSERT(m_VisibilityData); 
  m_VisibilityData->SetShowPointer(visible); 

  // we need to dirty to cause our bounds needs to be re-computed
  Dirty();
}