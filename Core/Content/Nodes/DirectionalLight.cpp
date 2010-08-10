#include "DirectionalLight.h"
#include "Core/Content/ContentVisitor.h"

using namespace Helium;
using namespace Helium::Content;

REFLECT_DEFINE_CLASS(DirectionalLight)

void DirectionalLight::EnumerateClass( Reflect::Compositor<DirectionalLight>& comp )
{
  Reflect::Field* fieldGlobalSun = comp.AddField( &DirectionalLight::m_GlobalSun, "m_GlobalSun" );
  comp.AddField( &DirectionalLight::m_ShadowSoftness, "m_ShadowSoftness" );
  comp.AddField( &DirectionalLight::m_SoftShadowSamples, "m_SoftShadowSamples" );
}

void DirectionalLight::Host(ContentVisitor* visitor)
{
  visitor->VisitDirectionalLight(this);
}

void DirectionalLight::GetDirection( Math::Vector3& direction )
{
  direction.x = 0;
  direction.y = 0;
  direction.z = 1.0;

  Math::Matrix3 rotation;
  Math::Scale scale;
  Math::Vector3 translation;

  m_GlobalTransform.Decompose( scale, rotation, translation );

  direction = rotation * direction;
}