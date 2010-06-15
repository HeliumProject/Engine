#include "Precompile.h"
#include "PortalLight.h"

#include "Scene.h"
#include "SceneManager.h"
#include "InstanceType.h"

#include "Core/Enumerator.h"
#include "Luna/UI/ImageManager.h"

#include "PrimitiveCube.h"
#include "PrimitivePointer.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::PortalLight);

void PortalLight::InitializeType()
{
  Reflect::RegisterClass< Luna::PortalLight >( "Luna::PortalLight" );

  Enumerator::InitializePanel( "PortalLight", CreatePanelSignature::Delegate( &PortalLight::CreatePanel ) );
}

void PortalLight::CleanupType()
{
  Reflect::UnregisterClass< Luna::PortalLight >();
}

PortalLight::PortalLight(Luna::Scene* scene)
: Luna::Light (scene, new Content::PortalLight() )
, m_Cube( NULL )
{

}

PortalLight::PortalLight(Luna::Scene* scene, Content::PortalLight* light)
: Luna::Light ( scene, light )
, m_Cube( NULL )
{

}

PortalLight::~PortalLight()
{
  delete m_Cube;
}

i32 PortalLight::GetImageIndex() const
{
  return Luna::GlobalImageManager().GetImageIndex( "light_16.png" );
}

std::string PortalLight::GetApplicationTypeName() const
{
  return "PortalLight";
}

void PortalLight::Initialize()
{
  if (!m_Cube)
  {
    m_Cube = new Luna::PrimitiveCube ( m_Scene->GetView()->GetResources() );
    m_Cube->Update();
  }

  __super::Initialize();
}

void PortalLight::Create()
{
  if( m_Cube )
  {
    m_Cube->Create();
  }

  __super::Create();
}

void PortalLight::Delete()
{
  if( m_Cube )
  {
    m_Cube->Delete();
  }

  __super::Delete();
}

void PortalLight::SetScale( const Math::Scale& value )
{
   // don't allow negative scaling - take abs of values
    Math::Scale newScale( abs(value.x), abs(value.y), abs(value.z) );
    newScale.z = 0.25f;

  // protect against infinite calls between SetOuterRadius and SetScale
  __super::SetScale( newScale );


}


void PortalLight::Render( RenderVisitor* render )
{
  // pointer is drawn normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = render->State().m_Matrix.Normalized();
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &PortalLight::DrawPointer;
  }

  // shape is drawn non-normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = render->State().m_Matrix;
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &PortalLight::DrawShape;
  }

  // don't call __super here, it will draw big ass axes
  Luna::HierarchyNode::Render( render );
}

void PortalLight::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::PortalLight* light = Reflect::ConstAssertCast<Luna::PortalLight>( object );

  const Luna::InstanceType* type = Reflect::ConstAssertCast<Luna::InstanceType>( light->GetNodeType() );

  light->SetMaterial( type->GetMaterial() );

  // draw type pointer
  type->GetPointer()->Draw( args );
}

void PortalLight::DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::PortalLight* light = Reflect::ConstAssertCast<Luna::PortalLight>( object );

  const Luna::InstanceType* type = Reflect::ConstAssertCast<Luna::InstanceType>( light->GetNodeType() );
  
  light->SetMaterial( type->GetMaterial() );

  if (light->GetCube())
  {
    light->GetCube()->Draw( args );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Luna::PortalLight.
//
bool PortalLight::ValidatePanel(const std::string& name)
{
  if (name == "PortalLight")
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

void PortalLight::CreatePanel( CreatePanelArgs& args )
{
 
}