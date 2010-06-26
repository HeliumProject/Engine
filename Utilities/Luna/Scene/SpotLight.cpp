#include "Precompile.h"
#include "SpotLight.h"
#include "Pipeline/Content/Nodes/Lights/SpotLight.h"

#include "Scene.h"
#include "SceneManager.h"

#include "InstanceType.h"

#include "Core/Enumerator.h"
#include "Application/UI/ImageManager.h"

#include "PrimitiveCone.h"
#include "PrimitivePointer.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(SpotLight);

void SpotLight::InitializeType()
{
  Reflect::RegisterClass< SpotLight >( "Luna::SpotLight" );

  Enumerator::InitializePanel( "SpotLight", CreatePanelSignature::Delegate( &SpotLight::CreatePanel ) );
}

void SpotLight::CleanupType()
{
  Reflect::UnregisterClass< SpotLight >();
}

SpotLight::SpotLight(Luna::Scene* scene)
: Luna::Light (scene, new Content::SpotLight() )
, m_InnerCone (NULL)
, m_OuterCone (NULL)
, m_FirstEntry( true )
{

}

SpotLight::SpotLight(Luna::Scene* scene, Content::SpotLight* light)
: Luna::Light ( scene, light )
, m_InnerCone (NULL)
, m_OuterCone (NULL)
, m_FirstEntry( true )
{

}

SpotLight::~SpotLight()
{
  delete m_InnerCone;
  delete m_OuterCone;
}

i32 SpotLight::GetImageIndex() const
{
  return Nocturnal::GlobalImageManager().GetImageIndex( "light.png" );
}

std::string SpotLight::GetApplicationTypeName() const
{
  return "SpotLight";
}

void SpotLight::Initialize()
{
  __super::Initialize();

  Content::SpotLight* light = GetPackage<Content::SpotLight>();

  if (!m_InnerCone)
  {
    m_InnerCone = new Luna::PrimitiveCone ( m_Scene->GetView()->GetResources() );
    m_InnerCone->m_Length = light->GetInnerRadius();
    m_InnerCone->m_Radius = tan(light->m_InnerConeAngle ) * m_InnerCone->m_Length;
    m_InnerCone->Update();
  }

  if (!m_OuterCone)
  {
    m_OuterCone = new Luna::PrimitiveCone ( m_Scene->GetView()->GetResources() );
    m_OuterCone->m_Length = light->GetOuterRadius();
    m_OuterCone->m_Radius = tan(light->m_OuterConeAngle ) * m_OuterCone->m_Length;
    m_OuterCone->Update();
  }
}

void SpotLight::Create()
{
  __super::Create();

  if (m_InnerCone)
  {
    m_InnerCone->Create();
  }

  if (m_OuterCone)
  {
    m_OuterCone->Create();
  }
}

void SpotLight::Delete()
{
  __super::Delete();

  if (m_InnerCone)
  {
    m_InnerCone->Delete();
  }

  if (m_OuterCone)
  {
    m_OuterCone->Delete();
  }
}

void SpotLight::SetScale( const Math::Scale& value )
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

void SpotLight::Evaluate( GraphDirection direction )
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
        float a = tanf(GetOuterConeAngle());
        m_ObjectBounds.minimum.x  = -a;
        m_ObjectBounds.maximum.x  =  a;
        m_ObjectBounds.minimum.y  = -a;
        m_ObjectBounds.maximum.y  =  a;
        m_ObjectBounds.minimum.z  = 0.0f;
        m_ObjectBounds.maximum.z  = 1.0f;
      }
      break;
    }
  }
  if(GetGodRayEnabled())
  {
    Content::SpotLight* spotLight = GetPackage< Content::SpotLight >();

    //Allow real-time update of the clip plane in luna drawing canvas
    spotLight->m_GlobalTransform = m_GlobalTransform;

    //Stupid hack alert; mess slightly with the position to get live link to detect this change...daym!
    spotLight->m_GlobalTransform.t.x += 0.00001f;
  }
}

void SpotLight::Render( RenderVisitor* render )
{
  Math::Matrix4 normalizedRenderMatrix = render->State().m_Matrix.Normalized();
  // pointer is drawn normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = normalizedRenderMatrix;
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &SpotLight::DrawPointer;
  }

  if( m_AreaLightPrim )
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = normalizedRenderMatrix;
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &SpotLight::DrawAreaLight;
  }

  // inner is drawn normalized
  if (m_InnerCone)
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = Math::Matrix4 (Math::Vector3(0.f, 0.f, -m_InnerCone->m_Length / 2.f)) * Math::Matrix4::RotateY(Math::Pi) * normalizedRenderMatrix;
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &SpotLight::DrawInner;

    if (Reflect::AssertCast<Luna::InstanceType>( m_NodeType )->IsTransparent())
    {
      entry->m_Flags |= RenderFlags::DistanceSort;
    }
  }

 

  // outer is normalized
  if (m_OuterCone)
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = Math::Matrix4 (Math::Vector3(0.f, 0.f, -m_OuterCone->m_Length / 2.f)) * Math::Matrix4::RotateY(Math::Pi) * normalizedRenderMatrix;
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &SpotLight::DrawOuter;

    if (Reflect::AssertCast<Luna::InstanceType>( m_NodeType )->IsTransparent())
    {
      entry->m_Flags |= RenderFlags::DistanceSort;
    }
  }

  if (GetGodRayEnabled() )
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = Math::Matrix4 ();
    entry->m_Center   = m_ObjectBounds.Center();
    entry->m_Draw     = &SpotLight::DrawGodRayClipPlane;

    if (Reflect::AssertCast<Luna::InstanceType>( m_NodeType )->IsTransparent())
    {
      entry->m_Flags |= RenderFlags::DistanceSort;
    }
  }

  // don't call __super here, it will draw big ass axes
  Luna::HierarchyNode::Render( render );
}

///////////////////////////////////////////////////////////////////////////////
inline Vector2 GetLineIntersection(const Vector2& p1, const Vector2& p2, 
                                   const Vector2& p3, const Vector2& p4)
{
  float a = (p4.x - p3.x)*(p1.y - p3.y) - (p4.y - p3.y)*(p1.x - p3.x);
  float b = (p4.y - p3.y)*(p2.x - p1.x) - (p4.x - p3.x)*(p2.y - p1.y);
  float u = a / b;

  return p1 + (p2 - p1)*u;
}

///////////////////////////////////////////////////////////////////////////////
// Display the GodRay clip-plane 
void SpotLight::DrawGodRayClipPlane( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::SpotLight*    lSpotLight  = Reflect::ConstAssertCast<Luna::SpotLight>( object );
  const Content::SpotLight*  spotLight   = lSpotLight->GetPackage< Content::SpotLight >();
  const float                b           = spotLight->m_GodRayClipPlaneOffset;

  if(b > 0.0f)
  {
    const int     cMaxPoints   = 5;

    float         outterAngle  = spotLight->m_OuterConeAngle;
    float         bottomA;
    float         topA;
    float         a            = b*tanf(outterAngle);

    //Uber compricated math...I hate myself for actually writing such a monstrosity. Abdul
    {
      const float   eulerAngleLimit     = Content::SpotLight::GetEulerAngleLimit();
      float         eulerAngle          = spotLight->m_GodRayClipPlanePhiOffset*eulerAngleLimit;
      float         c                   = a*tanf(eulerAngle);

      Vector2 points[]  = {   Vector2(0.0f, 0.0f), Vector2(b, a),
                              Vector2(b, 0.0f),    Vector2(b + c, a) };
      {
        Vector2 intersection  = GetLineIntersection(points[0], points[1], points[2], points[3]);
        Vector2 difference    = (intersection - points[2]);
        topA                  = difference.Length();
      }

      {
        //Flip the a
        points[1].y          *=-1.0f;
        Vector2 intersection  = GetLineIntersection(points[0], points[1], points[3], points[2]);
        Vector2 difference    = (intersection - points[2]);
        bottomA               = difference.Length();
      }
    }

    Math::Matrix4 transform           = spotLight->GetGodRayClipPlaneTransform();

    Math::Vector4 corners[cMaxPoints] = { Math::Vector4(-a, topA, 0.0f, 1.0f ),
                                          Math::Vector4(-a,-bottomA, 0.0f, 1.0f ),
                                          Math::Vector4( a,-bottomA, 0.0f, 1.0f ),
                                          Math::Vector4( a, topA, 0.0f, 1.0f ),
                                          Math::Vector4(-a, topA, 0.0f, 1.0f )};

    //Transform the above points to world space
    for(u32 i = 0; i < cMaxPoints; ++i)
    {
      corners[i]  = transform  * corners[i];
    }

    //Setup the material and draw the points
    lSpotLight->SetMaterial( s_Material );
    device->SetFVF( D3DFVF_XYZ  );
    device->DrawPrimitiveUP(D3DPT_LINESTRIP, cMaxPoints - 1, &corners[0].x, sizeof(Math::Vector4));
  }
}

bool SpotLight::SetupGodRayClipPlane( IDirect3DDevice9* device, const SceneNode* object )
{
  const Luna::SpotLight*    lSpotLight  = Reflect::ConstAssertCast<Luna::SpotLight>( object );
  const Content::SpotLight*  spotLight   = lSpotLight->GetPackage< Content::SpotLight >();

  if(spotLight->m_GodRayEnabled)
  {
    float clipOffset = spotLight->m_GodRayClipPlaneOffset;

    if(clipOffset > 0.0f)
    {
      Math::Vector4 plane = spotLight->GetGodRayClipPlane();
      device->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
      device->SetClipPlane(0, &plane.x);
      return true;
    }
  }
  
  return false;
}

void SpotLight::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const SpotLight* light = Reflect::ConstAssertCast<SpotLight>( object );

  const Luna::InstanceType* type = Reflect::ConstAssertCast<Luna::InstanceType>( light->GetNodeType() );

  light->SetMaterial( type->GetMaterial() );

  // draw type pointer
  type->GetPointer()->Draw( args );
}

void SpotLight::DrawAreaLight( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const SpotLight* spotLight = Reflect::ConstAssertCast<SpotLight>( object );

  const Luna::InstanceType* type = Reflect::ConstAssertCast<Luna::InstanceType>( spotLight->GetNodeType() );

  const Content::Light* contentLight = spotLight->GetPackage< Content::Light >();

  if( contentLight )
  {
    Math::HDRColor3 color = contentLight->m_Color;
    color.s = 1.0f;
    color.ToFloat( s_Material.Ambient.r, s_Material.Ambient.g, s_Material.Ambient.b );
    s_Material.Ambient.a = 0.25f;
  }

  spotLight->SetMaterial( s_Material );

  if( spotLight->m_AreaLightPrim )
  {
    spotLight->m_AreaLightPrim->Draw( args );
  }
}

void SpotLight::DrawInner( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const SpotLight* spotLight = Reflect::ConstAssertCast<SpotLight>( object );

  const Luna::InstanceType* type = Reflect::ConstAssertCast<Luna::InstanceType>( spotLight->GetNodeType() );

  const Content::Light* contentLight = spotLight->GetPackage< Content::Light >();
  
  if( contentLight )
  {
    Math::HDRColor3 color = contentLight->m_Color;
    color.s = 1.0f;
    color.ToFloat( s_Material.Ambient.r, s_Material.Ambient.g, s_Material.Ambient.b );
    s_Material.Ambient.a = 0.0f;
  }

  spotLight->SetMaterial( s_Material );

  // draw shape primitive
  if (spotLight->GetInnerCone())
  {
    //Setup the clip plane if this is a GodRay
    bool clipSetup = SetupGodRayClipPlane( device, object );

    spotLight->GetInnerCone()->Draw( args );

    //Clean up
    if(clipSetup)
    {
      device->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
    }
  }
}

void SpotLight::DrawOuter( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const SpotLight* spotLight = Reflect::ConstAssertCast<SpotLight>( object );

  const Luna::InstanceType* type = Reflect::ConstAssertCast<Luna::InstanceType>( spotLight->GetNodeType() );

  const Content::Light* contentLight = spotLight->GetPackage< Content::Light >();

  if( contentLight )
  {
    Math::HDRColor3 color = contentLight->m_Color;
    color.s = 1.0f;
    color.ToFloat( s_Material.Ambient.r, s_Material.Ambient.g, s_Material.Ambient.b );
  }

  spotLight->SetMaterial( s_Material );

  // draw shape primitive
  if (spotLight->GetOuterCone())
  {
    //Setup the clip plane if this is a GodRay
    bool clipSetup = SetupGodRayClipPlane( device, object );

    //Render the clipped cone
    spotLight->GetOuterCone()->Draw( args );

    //Clean up
    if(clipSetup)
    {
      device->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Luna::Light.
//
bool SpotLight::ValidatePanel(const std::string& name)
{
  if (name == "SpotLight")
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

void SpotLight::CreatePanel( CreatePanelArgs& args )
{
  args.m_Enumerator->PushPanel("Spot Light", true);
  {
    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Inner Radius");
      args.m_Enumerator->AddValue<SpotLight, float>( args.m_Selection, &SpotLight::GetInnerRadius, &SpotLight::SetInnerRadius );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Outer Radius");
      args.m_Enumerator->AddValue<SpotLight, float>( args.m_Selection, &SpotLight::GetOuterRadius, &SpotLight::SetOuterRadius );
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

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Inner Angle");

      args.m_Enumerator->AddValue<SpotLight, float>( args.m_Selection, &SpotLight::GetInnerConeAngleDegrees, &SpotLight::SetInnerConeAngleDegrees );
      Inspect::Slider* slider = args.m_Enumerator->AddSlider<SpotLight, float>( args.m_Selection, &SpotLight::GetInnerConeAngle, &SpotLight::SetInnerConeAngle );
      slider->SetRangeMin( 0 );
      slider->SetRangeMax( Math::HalfPi );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Outer Angle");
      args.m_Enumerator->AddValue<SpotLight, float>( args.m_Selection, &SpotLight::GetOuterConeAngleDegrees, &SpotLight::SetOuterConeAngleDegrees );
      Inspect::Slider* slider = args.m_Enumerator->AddSlider<SpotLight, float>( args.m_Selection, &SpotLight::GetOuterConeAngle, &SpotLight::SetOuterConeAngle );
      slider->SetRangeMin( 0 );
      slider->SetRangeMax( Math::HalfPi );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushPanel( "RealTime" );
    {
      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("Shadow Map Hi Res");
        args.m_Enumerator->AddCheckBox<SpotLight, bool>( args.m_Selection, &SpotLight::GetShadowMapHiRes, &SpotLight::SetShadowMapHiRes );
      }
      args.m_Enumerator->Pop();
     
      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("GodRay Enabled");
        args.m_Enumerator->AddCheckBox<SpotLight, bool>( args.m_Selection, &SpotLight::GetGodRayEnabled, &SpotLight::SetGodRayEnabled );
      }
      args.m_Enumerator->Pop();
      
      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("GodRay Opacity");

        args.m_Enumerator->AddValue<SpotLight, float>( args.m_Selection, &SpotLight::GetGodRayOpacity, &SpotLight::SetGodRayOpacity );
        Inspect::Slider* slider = args.m_Enumerator->AddSlider<SpotLight, float>( args.m_Selection, &SpotLight::GetGodRayOpacity, &SpotLight::SetGodRayOpacity );
        slider->SetRangeMin( 0.0f );
        slider->SetRangeMax( 1.0f );
      }
      args.m_Enumerator->Pop();

      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("GodRay Density");

        args.m_Enumerator->AddValue<SpotLight, float>( args.m_Selection, &SpotLight::GetGodRayDensity, &SpotLight::SetGodRayDensity );
        Inspect::Slider* slider = args.m_Enumerator->AddSlider<SpotLight, float>( args.m_Selection, &SpotLight::GetGodRayDensity, &SpotLight::SetGodRayDensity );
        slider->SetRangeMin( 0.00f );
        slider->SetRangeMax( 0.95f );
      }
      args.m_Enumerator->Pop();

      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("GodRay Quality");

        args.m_Enumerator->AddValue<SpotLight, float>( args.m_Selection, &SpotLight::GetGodRayQuality, &SpotLight::SetGodRayQuality );
        Inspect::Slider* slider = args.m_Enumerator->AddSlider<SpotLight, float>( args.m_Selection, &SpotLight::GetGodRayQuality, &SpotLight::SetGodRayQuality );
        slider->SetRangeMin( 0.0f );
        slider->SetRangeMax( 1.0f );
      }
      args.m_Enumerator->Pop();

      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("GodRay Fade Near");

        args.m_Enumerator->AddValue<SpotLight, float>( args.m_Selection, &SpotLight::GetGodRayFadeNear, &SpotLight::SetGodRayFadeNear );
        Inspect::Slider* slider = args.m_Enumerator->AddSlider<SpotLight, float>( args.m_Selection, &SpotLight::GetGodRayFadeNear, &SpotLight::SetGodRayFadeNear );
        slider->SetRangeMin( 0.5f );
        slider->SetRangeMax( 1000.0f );
      }
      args.m_Enumerator->Pop();

      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("GodRay Fade Far");

        args.m_Enumerator->AddValue<SpotLight, float>( args.m_Selection, &SpotLight::GetGodRayFadeFar, &SpotLight::SetGodRayFadeFar );
        Inspect::Slider* slider = args.m_Enumerator->AddSlider<SpotLight, float>( args.m_Selection, &SpotLight::GetGodRayFadeFar, &SpotLight::SetGodRayFadeFar );
        slider->SetRangeMin( 10.0f );
        slider->SetRangeMax( 1000.0f );
      }
      args.m_Enumerator->Pop();

      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("GodRay Clip Phi Offset");

        args.m_Enumerator->AddValue<SpotLight, float>( args.m_Selection, &SpotLight::GetGodRayClipPlanePhiOffset, &SpotLight::SetGodRayClipPlanePhiOffset );
        Inspect::Slider* slider = args.m_Enumerator->AddSlider<SpotLight, float>( args.m_Selection, &SpotLight::GetGodRayClipPlanePhiOffset, &SpotLight::SetGodRayClipPlanePhiOffset );
        slider->SetRangeMin(-1.0f );
        slider->SetRangeMax( 1.0f );
      }
      args.m_Enumerator->Pop();

      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("GodRay Clip Offset");

        args.m_Enumerator->AddValue<SpotLight, float>( args.m_Selection, &SpotLight::GetGodRayClipPlaneOffset, &SpotLight::SetGodRayClipPlaneOffset );
        Inspect::Slider* slider = args.m_Enumerator->AddSlider<SpotLight, float>( args.m_Selection, &SpotLight::GetGodRayClipPlaneOffset, &SpotLight::SetGodRayClipPlaneOffset );
        slider->SetRangeMin( 0.0f );
        slider->SetRangeMax( 0.95f );
      }
      args.m_Enumerator->Pop();

      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("Offset Factor");

        args.m_Enumerator->AddValue<SpotLight, u8>( args.m_Selection, &SpotLight::GetOffsetFactor, &SpotLight::SetOffsetFactor );
        Inspect::Slider* slider = args.m_Enumerator->AddSlider<SpotLight, u8>( args.m_Selection, &SpotLight::GetOffsetFactor, &SpotLight::SetOffsetFactor );
        slider->SetRangeMin( 0.0f );
        slider->SetRangeMax( 255.0f );
      }
      args.m_Enumerator->Pop();


      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("Offset Units");

        args.m_Enumerator->AddValue<SpotLight, u8>( args.m_Selection, &SpotLight::GetOffsetUnits, &SpotLight::SetOffsetUnits );
        Inspect::Slider* slider = args.m_Enumerator->AddSlider<SpotLight, u8>( args.m_Selection, &SpotLight::GetOffsetUnits, &SpotLight::SetOffsetUnits );
        slider->SetRangeMin( 0.0f );
        slider->SetRangeMax( 255.0f );
      }
      args.m_Enumerator->Pop();

    }
    args.m_Enumerator->Pop();
  }
  args.m_Enumerator->Pop();
}

float SpotLight::GetInnerRadius() const
{
  return GetPackage< Content::SpotLight >()->GetInnerRadius();
}
void SpotLight::SetInnerRadius( float radius )
{
  Content::SpotLight* light = GetPackage< Content::SpotLight >();
  light->SetInnerRadius( radius );
  radius = light->GetInnerRadius();

  if (m_InnerCone)
  {
    m_InnerCone->m_Length = radius;
    m_InnerCone->m_Radius = tan(GetPackage< Content::SpotLight >()->m_InnerConeAngle ) * radius;
    m_InnerCone->Update();
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

float SpotLight::GetOuterRadius() const
{
  return GetPackage< Content::SpotLight >()->GetOuterRadius();
}
void SpotLight::SetOuterRadius( float radius )
{
  Content::SpotLight* light = GetPackage< Content::SpotLight >();
  light->SetOuterRadius( radius );
  radius = light->GetOuterRadius();

  if (m_OuterCone)
  {
    m_OuterCone->m_Length = radius;
    m_OuterCone->m_Radius = tan(GetPackage< Content::SpotLight >()->m_OuterConeAngle ) * radius;
    m_OuterCone->Update();
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

float SpotLight::GetInnerConeAngleDegrees() const
{
  return Math::RadToDeg * GetInnerConeAngle() * 2.0f;
}

void SpotLight::SetInnerConeAngleDegrees( float innerAngle )
{
  innerAngle *= Math::DegToRad * 0.5f;
  Math::Clamp( innerAngle, 0.0f, Math::HalfPi  );

  SetInnerConeAngle( innerAngle );
}

float SpotLight::GetInnerConeAngle() const
{
  return GetPackage< Content::SpotLight >()->m_InnerConeAngle;
}
void SpotLight::SetInnerConeAngle( float innerAngle )
{
  GetPackage< Content::SpotLight >()->m_InnerConeAngle = innerAngle;

  if (m_InnerCone)
  {
    m_InnerCone->m_Radius = tan( innerAngle ) * m_InnerCone->m_Length;
    m_InnerCone->Update();
  }

  if( GetOuterConeAngle() < innerAngle )
  {
    SetOuterConeAngle( innerAngle );
  }
  // only set params if we don't have to adjust the outer angle
  // because adjusting outer angle will implicitly set params
  else
  {
    m_Changed.Raise( LightChangeArgs( this ) );
  }
}

float SpotLight::GetOuterConeAngleDegrees() const
{
  return Math::RadToDeg * GetOuterConeAngle() * 2.0f;
}

void SpotLight::SetOuterConeAngleDegrees( float outerAngle )
{
  outerAngle *= Math::DegToRad * 0.5f;
  Math::Clamp( outerAngle, 0.0f, Math::HalfPi );

  SetOuterConeAngle( outerAngle );
}

float SpotLight::GetOuterConeAngle() const
{
  return GetPackage< Content::SpotLight >()->m_OuterConeAngle;
}
void SpotLight::SetOuterConeAngle( float outerAngle )
{
  GetPackage< Content::SpotLight >()->m_OuterConeAngle = outerAngle;

  if (m_OuterCone)
  {
    m_OuterCone->m_Radius = tan( outerAngle ) * m_OuterCone->m_Length;
    m_OuterCone->Update();
  }

  if( GetInnerConeAngle() > outerAngle )
  {
    SetInnerConeAngle( outerAngle );
  }
  // only set params if we don't have to adjust the inner angle
  // because adjusting inner angle will implicitly set params
  else
  {
    m_Changed.Raise( LightChangeArgs( this ) );
  }
}

bool SpotLight::GetShadowMapHiRes() const
{
  return GetPackage< Content::SpotLight >()->m_ShadowMapHiRes;
}
void SpotLight::SetShadowMapHiRes( bool b )
{
  GetPackage< Content::SpotLight >()->m_ShadowMapHiRes = b;

  m_Changed.Raise( LightChangeArgs( this ) );
}

u8 SpotLight::GetOffsetFactor() const
{
  return GetPackage< Content::SpotLight >()->m_OffsetFactor;
}

void SpotLight::SetOffsetFactor( u8 factor )
{
  GetPackage< Content::SpotLight >()->m_OffsetFactor = factor;
  m_Changed.Raise( LightChangeArgs( this ) );
}

u8 SpotLight::GetOffsetUnits() const
{
  return GetPackage< Content::SpotLight >()->m_OffsetUnits;
}

void SpotLight::SetOffsetUnits( u8 units )
{
  GetPackage< Content::SpotLight >()->m_OffsetUnits = units;
  m_Changed.Raise( LightChangeArgs( this ) );
}

///////////////////////////////////////////////////////////////////////////////
// GodRay specifics
//
bool SpotLight::GetGodRayEnabled() const
{
  return GetPackage< Content::SpotLight >()->m_GodRayEnabled;
}
void SpotLight::SetGodRayEnabled( bool b )
{
  GetPackage< Content::SpotLight >()->m_GodRayEnabled = b;

  m_Changed.Raise( LightChangeArgs( this ) );
}

///////////////////////////////////////////////////////////////////////////////
// Opacity
float SpotLight::GetGodRayOpacity() const
{
  return GetPackage< Content::SpotLight >()->m_GodRayOpacity;
}
void SpotLight::SetGodRayOpacity( float opacity )
{
  GetPackage< Content::SpotLight >()->m_GodRayOpacity = opacity;

  m_Changed.Raise( LightChangeArgs( this ) );
}

///////////////////////////////////////////////////////////////////////////////
// Slice Distance
float SpotLight::GetGodRayDensity() const
{
  return GetPackage< Content::SpotLight >()->m_GodRayDensity;
}

void SpotLight::SetGodRayDensity(float val)
{
  GetPackage< Content::SpotLight >()->m_GodRayDensity = val;
  m_Changed.Raise( LightChangeArgs( this ) );
}

///////////////////////////////////////////////////////////////////////////////
// Fade Near
float SpotLight::GetGodRayFadeNear() const
{
  return GetPackage< Content::SpotLight >()->m_GodRayFadeNear;
}

void SpotLight::SetGodRayFadeNear(float val)
{
  GetPackage< Content::SpotLight >()->m_GodRayFadeNear = val;
  
  if( GetGodRayFadeFar() < val )
  {
    SetGodRayFadeFar( val );
  }
  else
  {
    m_Changed.Raise( LightChangeArgs( this ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Fade Far
float SpotLight::GetGodRayFadeFar() const
{
  return GetPackage< Content::SpotLight >()->m_GodRayFadeFar;
}

void SpotLight::SetGodRayFadeFar(float val)
{
  GetPackage< Content::SpotLight >()->m_GodRayFadeFar = val;

  if( GetGodRayFadeNear() > val )
  {
    SetGodRayFadeNear( val );
  }
  else
  {
    m_Changed.Raise( LightChangeArgs( this ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Quality
float SpotLight::GetGodRayQuality() const
{
  return GetPackage< Content::SpotLight >()->m_GodRayQuality;
}

void SpotLight::SetGodRayQuality(float val)
{
  GetPackage< Content::SpotLight >()->m_GodRayQuality = val;
  m_Changed.Raise( LightChangeArgs( this ) );
}

///////////////////////////////////////////////////////////////////////////////
// Phi Offset
float SpotLight::GetGodRayClipPlanePhiOffset() const
{
  return GetPackage< Content::SpotLight >()->m_GodRayClipPlanePhiOffset;
}

void SpotLight::SetGodRayClipPlanePhiOffset(float val)
{
  GetPackage< Content::SpotLight >()->m_GodRayClipPlanePhiOffset = val;
  m_Changed.Raise( LightChangeArgs( this ) );
}

///////////////////////////////////////////////////////////////////////////////
// Offset
float SpotLight::GetGodRayClipPlaneOffset() const
{
  return GetPackage< Content::SpotLight >()->m_GodRayClipPlaneOffset;
}

void SpotLight::SetGodRayClipPlaneOffset(float val)
{
  GetPackage< Content::SpotLight >()->m_GodRayClipPlaneOffset = val;
  m_Changed.Raise( LightChangeArgs( this ) );
}
