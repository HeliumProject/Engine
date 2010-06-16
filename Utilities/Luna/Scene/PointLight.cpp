#include "Precompile.h"
#include "PointLight.h"

#include "Scene.h"
#include "SceneManager.h"
#include "InstanceType.h"

#include "Core/Enumerator.h"
#include "Application/UI/ImageManager.h"

#include "PrimitiveSphere.h"
#include "PrimitivePointer.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::PointLight);

void PointLight::InitializeType()
{
  Reflect::RegisterClass< Luna::PointLight >( "Luna::PointLight" );

  Enumerator::InitializePanel( "PointLight", CreatePanelSignature::Delegate( &PointLight::CreatePanel ) );
}

void PointLight::CleanupType()
{
  Reflect::UnregisterClass< Luna::PointLight >();
}

PointLight::PointLight(Luna::Scene* scene)
: Luna::Light (scene, new Content::PointLight() )
, m_InnerSphere (NULL)
, m_OuterSphere (NULL)
, m_FirstEntry( true )
{

}

PointLight::PointLight(Luna::Scene* scene, Content::PointLight* light)
: Luna::Light ( scene, light )
, m_InnerSphere (NULL)
, m_OuterSphere (NULL)
, m_FirstEntry( true )
{

}

PointLight::~PointLight()
{
  delete m_InnerSphere;
  delete m_OuterSphere;
}

i32 PointLight::GetImageIndex() const
{
  return Nocturnal::GlobalImageManager().GetImageIndex( "light_16.png" );
}

std::string PointLight::GetApplicationTypeName() const
{
  return "PointLight";
}

void PointLight::Initialize()
{
  __super::Initialize();

  Content::PointLight* light = GetPackage<Content::PointLight>();

  if (!m_InnerSphere)
  {
    m_InnerSphere = new Luna::PrimitiveSphere ( m_Scene->GetView()->GetResources() );
    m_InnerSphere->m_Radius = light->GetInnerRadius();
    m_InnerSphere->Update();
  }

  if (!m_OuterSphere)
  {
    m_OuterSphere = new Luna::PrimitiveSphere ( m_Scene->GetView()->GetResources() );
    m_OuterSphere->m_Radius = light->GetOuterRadius();
    m_OuterSphere->Update();
  }
}

void PointLight::Create()
{
  __super::Create();

  if (m_InnerSphere)
  {
    m_InnerSphere->Create();
  }

  if (m_OuterSphere)
  {
    m_OuterSphere->Create();
  }
}

void PointLight::Delete()
{
  __super::Delete();

  if (m_InnerSphere)
  {
    m_InnerSphere->Delete();
  }

  if (m_OuterSphere)
  {
    m_OuterSphere->Delete();
  }
}

void PointLight::SetScale( const Math::Scale& value )
{
  // protect against infinite calls between SetOuterRadius and SetScale
  if( m_FirstEntry )
  {
    m_FirstEntry = false;

     // don't allow negative scaling - take abs of values
    float maximum = std::max( std::max( abs(value.x), abs(value.y) ), abs(value.z) );
    Math::Scale newScale( maximum, maximum, maximum );
    __super::SetScale( newScale );

    SetOuterRadius( maximum );
  }
  m_FirstEntry = true;
}

void PointLight::Render( RenderVisitor* render )
{
  Math::Matrix4 normalizedRenderMatrix = render->State().m_Matrix.Normalized();
  // pointer is drawn normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = normalizedRenderMatrix;
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &PointLight::DrawPointer;
  }

  // shape is drawn normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = normalizedRenderMatrix;
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &PointLight::DrawShape;

    if ( Reflect::AssertCast< Luna::InstanceType >( m_NodeType )->IsTransparent() )
    {
      entry->m_Flags |= RenderFlags::DistanceSort;
    }
  }

  // don't call __super here, it will draw big ass axes
  Luna::HierarchyNode::Render( render );
}

void PointLight::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::PointLight* light = Reflect::ConstAssertCast<Luna::PointLight>( object );

  const Luna::InstanceType* type = Reflect::ConstAssertCast<Luna::InstanceType>( light->GetNodeType() );

  light->SetMaterial( type->GetMaterial() );

  // draw type pointer
  type->GetPointer()->Draw( args );
}

void PointLight::DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::PointLight* light = Reflect::ConstAssertCast<Luna::PointLight>( object );

  const Luna::InstanceType* type = Reflect::ConstAssertCast<Luna::InstanceType>( light->GetNodeType() );

  const Content::Light* contentLight = light->GetPackage< Content::Light >();
  
  if( contentLight )
  {
    Math::HDRColor3 color = contentLight->m_Color;
    color.s = 1.0f;
    color.ToFloat( s_Material.Ambient.r, s_Material.Ambient.g, s_Material.Ambient.b );
    s_Material.Ambient.a = 0.25f;
  }
  
  light->SetMaterial( s_Material );

  if( light->m_AreaLightPrim )
  {
    light->m_AreaLightPrim->Draw( args );
  }

  if (light->GetInnerSphere())
  {
    light->GetInnerSphere()->Draw( args );
  }

  if (light->GetOuterSphere())
  {
    light->GetOuterSphere()->Draw( args );
  }

  
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Luna::PointLight.
//
bool PointLight::ValidatePanel(const std::string& name)
{
  if (name == "PointLight")
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

void PointLight::CreatePanel( CreatePanelArgs& args )
{
  args.m_Enumerator->PushPanel("Point Light", true);
  {
    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Inner Radius");
      args.m_Enumerator->AddValue<Luna::PointLight, float>( args.m_Selection, &PointLight::GetInnerRadius, &PointLight::SetInnerRadius );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Outer Radius");
      args.m_Enumerator->AddValue<Luna::PointLight, float>( args.m_Selection, &PointLight::GetOuterRadius, &PointLight::SetOuterRadius );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Area Light Radius");
      args.m_Enumerator->AddValue<Light, f32>( args.m_Selection, &Light::GetAreaLightRadius, &Light::SetAreaLightRadius );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Area Light Dimensions");
      args.m_Enumerator->AddValue<Light, Math::Vector2>( args.m_Selection, &Light::GetAreaLightDimensions, &Light::SetAreaLightDimensions );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Samples Per Meter");
      args.m_Enumerator->AddValue<Light, f32>( args.m_Selection, &Light::GetAreaLightSamplesPerMeter, &Light::SetAreaLightSamplesPerMeter );
    }
    args.m_Enumerator->Pop();

  }

  args.m_Enumerator->Pop();
}

float PointLight::GetInnerRadius() const
{
  return GetPackage< Content::PointLight >()->GetInnerRadius();
}
void PointLight::SetInnerRadius( float radius )
{
  Content::PointLight* light = GetPackage< Content::PointLight >();
  light->SetInnerRadius( radius );
  radius = light->GetInnerRadius();

  if (m_InnerSphere)
  {
    m_InnerSphere->m_Radius = radius;
    m_InnerSphere->Update();
  }

  if( GetOuterRadius() < radius )
  {
    SetOuterRadius( radius );
  }
  else
  {
    m_Changed.Raise( LightChangeArgs( this ) );
  }
}

float PointLight::GetOuterRadius() const
{
  return GetPackage< Content::PointLight >()->GetOuterRadius();
}
void PointLight::SetOuterRadius( float radius )
{
  Content::PointLight* light = GetPackage< Content::PointLight >();
  light->SetOuterRadius( radius );
  radius = light->GetOuterRadius();

  if (m_OuterSphere)
  {
    m_OuterSphere->m_Radius = radius;
    m_OuterSphere->Update();
  }

  if( GetInnerRadius() > radius )
  {
    SetInnerRadius( radius );
  }
  else
  {
    m_Changed.Raise( LightChangeArgs( this ) );
  }

  SetScale( Math::Scale( radius, radius, radius ) );
}

void PointLight::Evaluate( GraphDirection direction )
{
  Luna::Transform::Evaluate(direction);

  switch (direction)
  {
    case GraphDirections::Downstream:
    {
      // start the box from scratch
      m_ObjectBounds.Reset();

      if ( m_NodeType )
      {
        m_ObjectBounds.minimum.x  = -1.0f;
        m_ObjectBounds.maximum.x  =  1.0f;
        m_ObjectBounds.minimum.y  = -1.0f;
        m_ObjectBounds.maximum.y  =  1.0f;
        m_ObjectBounds.minimum.z  = -1.0f;
        m_ObjectBounds.maximum.z  =  1.0f;
      }
      break;
    }
  }
}