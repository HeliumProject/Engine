#include "SunLight.h"
#include "ContentVisitor.h"

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_CLASS(SunLight)

void SunLight::EnumerateClass( Reflect::Compositor<SunLight>& comp )
{
  Reflect::Field* fieldHaze = comp.AddField( &SunLight::m_Haze, "m_Haze" );
  Reflect::Field* fieldRedBlueShift = comp.AddField( &SunLight::m_RedBlueShift, "m_RedBlueShift" );
  Reflect::Field* fieldSaturation = comp.AddField( &SunLight::m_Saturation, "m_Saturation" );
  Reflect::Field* fieldHorizonHeight = comp.AddField( &SunLight::m_HorizonHeight, "m_HorizonHeight" );
  Reflect::Field* fieldHorizonBlur = comp.AddField( &SunLight::m_HorizonBlur, "m_HorizonBlur" );
  Reflect::Field* fieldSunIntensity = comp.AddField( &SunLight::m_SunIntensity, "m_SunIntensity" );
  Reflect::Field* fieldSkyIntensity = comp.AddField( &SunLight::m_SkyIntensity, "m_SkyIntensity" );
  Reflect::Field* fieldGroundColor = comp.AddField( &SunLight::m_GroundColor, "m_GroundColor" );  
}



const f32 SunLight::MinHaze = 0.0f;
const f32 SunLight::MaxHaze = 15.0f;

const f32 SunLight::MinRedBlueShift = -1.0f;
const f32 SunLight::MaxRedBlueShift = 1.0f;

const f32 SunLight::MinSaturation = 0.0f;
const f32 SunLight::MaxSaturation = 2.0f;

const f32 SunLight::MinHorizonHeight = -10.0f;
const f32 SunLight::MaxHorizonHeight = 10.0f;

const f32 SunLight::MinHorizonBlur =  0.0f;
const f32 SunLight::MaxHorizonBlur = 10.0f;

void SunLight::Host(ContentVisitor* visitor)
{
  visitor->VisitSunLight(this); 
}