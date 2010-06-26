#include "Precompile.h"
#include "AmbientVolumeLight.h"

#include "Scene.h"
#include "SceneManager.h"
#include "InstanceType.h"

#include "Core/Enumerator.h"
#include "Application/UI/ImageManager.h"

#include "PrimitiveSphere.h"
#include "PrimitiveCube.h"
#include "PrimitivePointer.h"

// Using
using namespace Reflect;
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(AmbientVolumeLight);

void AmbientVolumeLight::InitializeType()
{
  Reflect::RegisterClass< AmbientVolumeLight >( "Luna::AmbientVolumeLight" );
  Enumerator::InitializePanel( "AmbientVolumeLight", CreatePanelSignature::Delegate( &AmbientVolumeLight::CreatePanel ) );
}

void AmbientVolumeLight::CleanupType()
{

}

AmbientVolumeLight::AmbientVolumeLight(Scene* scene)
: Light (scene, new Content::AmbientVolumeLight() )
, m_VolumeLightInnerPrim (NULL)
, m_VolumeLightOuterPrim (NULL)
, m_FirstEntry( true )
{

}

AmbientVolumeLight::AmbientVolumeLight(Scene* scene, Content::AmbientVolumeLight* light)
: Light ( scene, light )
, m_VolumeLightInnerPrim (NULL)
, m_VolumeLightOuterPrim (NULL)
, m_FirstEntry( true )
{

}

AmbientVolumeLight::~AmbientVolumeLight()
{
  delete m_VolumeLightInnerPrim;
  delete m_VolumeLightOuterPrim;
}

i32 AmbientVolumeLight::GetImageIndex() const
{
  return Nocturnal::GlobalImageManager().GetImageIndex( "light.png" );
}

std::string AmbientVolumeLight::GetApplicationTypeName() const
{
  return "AmbientVolumeLight";
}

void AmbientVolumeLight::Initialize()
{
  __super::Initialize();

  Content::AmbientVolumeLight* light = GetPackage<Content::AmbientVolumeLight>();

  SwitchVolumeLightType( GetVolumeLightType() );

  
}

void AmbientVolumeLight::Create()
{
  __super::Create();

  if (m_VolumeLightInnerPrim)
  {
    m_VolumeLightInnerPrim->Create();
  }

  if (m_VolumeLightOuterPrim)
  {
    m_VolumeLightOuterPrim->Create();
  }
}

void AmbientVolumeLight::Delete()
{
  __super::Delete();

  if (m_VolumeLightInnerPrim)
  {
    m_VolumeLightInnerPrim->Delete();
  }

  if (m_VolumeLightOuterPrim)
  {
    m_VolumeLightOuterPrim->Delete();
  }
}

void AmbientVolumeLight::SetScale( const Math::Scale& value )
{
  // protect against infinite calls between SetOuterRadius and SetScale
  if( m_FirstEntry )
  {
    m_FirstEntry = false;

    // don't allow negative scaling - take abs of values
    Math::Scale newScale( abs(value.x), abs(value.y) , abs(value.z) );
    float maximum = std::max( std::max( newScale.x, newScale.y), newScale.z );
    
    __super::SetScale( newScale );

    SetOuterRadius( maximum );
  }
  m_FirstEntry = true;
}

void AmbientVolumeLight::Render( RenderVisitor* render )
{
  Math::Matrix4 normalizedRenderMatrix = render->State().m_Matrix.Normalized();
  // pointer is drawn normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = normalizedRenderMatrix;
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &AmbientVolumeLight::DrawPointer;
  }

  // shape is drawn non-normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = render->State().m_Matrix;//normalizedRenderMatrix;
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &AmbientVolumeLight::DrawShape;

    if (Reflect::AssertCast<InstanceType>( m_NodeType )->IsTransparent())
    {
      entry->m_Flags |= RenderFlags::DistanceSort;
    }
  }

  // don't call __super here, it will draw big ass axes
  HierarchyNode::Render( render );
}

void AmbientVolumeLight::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const AmbientVolumeLight* light = ConstAssertCast<AmbientVolumeLight>( object );

  const InstanceType* type = ConstAssertCast<InstanceType>( light->GetNodeType() );

  light->SetMaterial( type->GetMaterial() );

  // draw type pointer
  type->GetPointer()->Draw( args );
}

void AmbientVolumeLight::DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const AmbientVolumeLight* light = ConstAssertCast<AmbientVolumeLight>( object );

  const InstanceType* type = ConstAssertCast<InstanceType>( light->GetNodeType() );

  const Content::Light* contentLight = light->GetPackage< Content::Light >();

  if( contentLight )
  {
    Math::HDRColor3 color = contentLight->m_Color;
    color.s = 1.0f;
    color.ToFloat( s_Material.Ambient.r, s_Material.Ambient.g, s_Material.Ambient.b );
  }

  light->SetMaterial( s_Material );

  if (light->m_VolumeLightInnerPrim)
  {
    light->m_VolumeLightInnerPrim->Draw( args );
  }

  if (light->m_VolumeLightOuterPrim)
  {
    light->m_VolumeLightOuterPrim->Draw( args );
  }


}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by AmbientVolumeLight.
//
bool AmbientVolumeLight::ValidatePanel(const std::string& name)
{
  if (name == "AmbientVolumeLight")
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

void AmbientVolumeLight::CreatePanel( CreatePanelArgs& args )
{
  args.m_Enumerator->PushPanel("Ambient Volume Light", true);
  {
    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Volume Light");

      Inspect::Choice* choice = args.m_Enumerator->AddChoice<AmbientVolumeLight, int>(args.m_Selection, &AmbientVolumeLight::GetVolumeLightType, &AmbientVolumeLight::SetVolumeLightType);
      choice->SetDropDown( true );

      Inspect::V_Item items;
      {
        {
          std::ostringstream str;
          str << Content::VolumeLightTypes::Sphere;
          items.push_back( Inspect::Item( "Sphere", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Content::VolumeLightTypes::Cuboid;
          items.push_back( Inspect::Item( "Cuboid", str.str() ) );
        }
      }
      choice->SetItems( items );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Inner Radius");
      args.m_Enumerator->AddValue<AmbientVolumeLight, float>( args.m_Selection, &AmbientVolumeLight::GetInnerRadius, &AmbientVolumeLight::SetInnerRadius );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Outer Radius");
      args.m_Enumerator->AddValue<AmbientVolumeLight, float>( args.m_Selection, &AmbientVolumeLight::GetOuterRadius, &AmbientVolumeLight::SetOuterRadius );
    }
    args.m_Enumerator->Pop();
  }

  args.m_Enumerator->Pop();
}

float AmbientVolumeLight::GetInnerRadius() const
{
  return GetPackage< Content::AmbientVolumeLight >()->m_InnerRadius;
}
void AmbientVolumeLight::SetInnerRadius( float radius )
{
  GetPackage< Content::AmbientVolumeLight >()->m_InnerRadius = radius;

  if (GetVolumeLightType() == Content::VolumeLightTypes::Sphere )
  {
    if( m_VolumeLightInnerPrim )
    {
      float outerRadius = GetPackage< Content::AmbientVolumeLight >()->m_OuterRadius;
      if( outerRadius <= 0 )
        Reflect::DangerousCast< PrimitiveSphere >(m_VolumeLightInnerPrim)->m_Radius = 0;
      else
        Reflect::DangerousCast< PrimitiveSphere >(m_VolumeLightInnerPrim)->m_Radius = radius / outerRadius;
      m_VolumeLightInnerPrim->Update();
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
}

float AmbientVolumeLight::GetOuterRadius() const
{
  return GetPackage< Content::AmbientVolumeLight >()->m_OuterRadius;
}
void AmbientVolumeLight::SetOuterRadius( float radius )
{
  GetPackage< Content::AmbientVolumeLight >()->m_OuterRadius = radius;

  if (GetVolumeLightType() == Content::VolumeLightTypes::Sphere )
  {
    if (m_VolumeLightInnerPrim)
    {
      if( radius <= 0 )
      {
        Reflect::DangerousCast< PrimitiveSphere >(m_VolumeLightInnerPrim)->m_Radius = 0;
      }
      else
      {
         Reflect::DangerousCast< PrimitiveSphere >(m_VolumeLightInnerPrim)->m_Radius =  GetInnerRadius() / radius;
      }
      m_VolumeLightInnerPrim->Update();
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
}

int AmbientVolumeLight::GetVolumeLightType() const
{
  return GetPackage< Content::AmbientVolumeLight >()->m_VolumeLightType;
}

void AmbientVolumeLight::SwitchVolumeLightType( int type )
{
  GetPackage< Content::AmbientVolumeLight >()->m_VolumeLightType = static_cast< Content::VolumeLightType >(type);

  switch( type )
  {

  case Content::VolumeLightTypes::Sphere:
    {

      if( m_VolumeLightInnerPrim )
      {
        m_VolumeLightInnerPrim->Delete();
        delete m_VolumeLightInnerPrim;
      }

      m_VolumeLightInnerPrim =  new PrimitiveSphere ( m_Scene->GetView()->GetResources() );
      m_VolumeLightInnerPrim->Create();

      f32 outerRad = GetOuterRadius();
      if( outerRad <= 0 )
        DangerousCast<PrimitiveSphere>( m_VolumeLightInnerPrim )->m_Radius = 0;
      else
        DangerousCast<PrimitiveSphere>( m_VolumeLightInnerPrim )->m_Radius = GetInnerRadius() / GetOuterRadius();
      m_VolumeLightInnerPrim->Update(); 

      if( m_VolumeLightOuterPrim )
      {
        m_VolumeLightOuterPrim->Delete();
        delete m_VolumeLightOuterPrim;
      }

      m_VolumeLightOuterPrim =  new PrimitiveSphere ( m_Scene->GetView()->GetResources() );
      m_VolumeLightOuterPrim->Update();
      m_VolumeLightOuterPrim->Create();

      break;
    }
  case Content::VolumeLightTypes::Cuboid:
    {
      if( m_VolumeLightInnerPrim )
      {
        m_VolumeLightInnerPrim->Delete();
        delete m_VolumeLightInnerPrim;
        m_VolumeLightInnerPrim = NULL;
      }

      if( m_VolumeLightOuterPrim )
      {
        m_VolumeLightOuterPrim->Delete();
        delete m_VolumeLightOuterPrim;
      }

      m_VolumeLightOuterPrim =  new PrimitiveCube ( m_Scene->GetView()->GetResources() );
      m_VolumeLightOuterPrim->Update(); 
      m_VolumeLightOuterPrim->Create();
    }

    break;
  }

}
void AmbientVolumeLight::SetVolumeLightType( int type )
{
  
  if( GetPackage< Content::AmbientVolumeLight >()->m_VolumeLightType == type )
  {
    return;
  }

  SwitchVolumeLightType( type );
}
