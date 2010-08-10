#include "Light.h"
#include "Core/Content/ContentVisitor.h" 

using namespace Helium;
using namespace Helium::Content;

REFLECT_DEFINE_ABSTRACT(Light)

void Light::EnumerateClass( Reflect::Compositor<Light>& comp )
{
  Reflect::Field* fieldColor = comp.AddField( &Light::m_Color, "m_Color" );
  Reflect::Field* fieldAttenuate = comp.AddField( &Light::m_Attenuate, "m_Attenuate" );
  Reflect::Field* fieldCastShadows = comp.AddField( &Light::m_CastShadows, "m_CastShadows" );
  Reflect::Field* fieldDoVisibilityRayTest = comp.AddField( &Light::m_DoVisibilityRayTest, "m_DoVisibilityRayTest" );
  Reflect::Field* fieldFactor = comp.AddField( &Light::m_Factor, "m_Factor" );

  Reflect::EnumerationField* enumRenderType = comp.AddEnumerationField( &Light::m_RenderType, "m_RenderType" );
  Reflect::EnumerationField* enumFlareType = comp.AddEnumerationField( &Light::m_FlareType, "m_FlareType" );
  Reflect::Field* fieldDrawDist = comp.AddField( &Light::m_DrawDist, "m_DrawDist" );
  Reflect::Field* fieldKillIfInactive = comp.AddField( &Light::m_KillIfInactive, "m_KillIfInactive" );
  Reflect::Field* fieldAllowOversized = comp.AddField( &Light::m_AllowOversized, "m_AllowOversized" );
  Reflect::Field* fieldEmitPhotons = comp.AddField( &Light::m_EmitPhotons, "m_EmitPhotons" );
  Reflect::Field* fieldNumPhotons = comp.AddField( &Light::m_NumPhotons, "m_NumPhotons" );
  Reflect::Field* fieldPhotonEnergy = comp.AddField( &Light::m_PhotonEnergy, "m_PhotonEnergy" );
  Reflect::Field* fieldAnimationColor = comp.AddField( &Light::m_AnimationColor, "m_AnimationColor" );
  Reflect::Field* fieldAnimationIntensity = comp.AddField( &Light::m_AnimationIntensity, "m_AnimationIntensity" );
  Reflect::Field* fieldAnimationDuration = comp.AddField( &Light::m_AnimationDuration, "m_AnimationDuration" );
  Reflect::Field* fieldRandomAnimOffset = comp.AddField( &Light::m_RandomAnimOffset, "m_RandomAnimOffset" );
  Reflect::Field* fieldPhysicalLight = comp.AddField( &Light::m_PhysicalLight, "m_PhysicalLight" );

}


void Light::Host(ContentVisitor* visitor)
{
  visitor->VisitLight(this); 
}
