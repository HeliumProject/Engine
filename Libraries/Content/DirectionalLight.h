#pragma once

#include "API.h"
#include "Light.h"

namespace Content
{
  class CONTENT_API DirectionalLight : public Light
  {
  public:
    virtual void Host(ContentVisitor* visitor);

    DirectionalLight ()
      : Light(),
        m_GlobalSun( false ),
        m_ShadowSoftness( 0.0f ),
        m_SoftShadowSamples( 32 )
    {
    }

    DirectionalLight (Nocturnal::TUID &id)
      : Light (id),
        m_GlobalSun( false ),
        m_ShadowSoftness( 0.0f ),
        m_SoftShadowSamples( 32 )
    {

    }

    REFLECT_DECLARE_CLASS(DirectionalLight, Light);

    void GetDirection( Math::Vector3& direction );

    bool m_GlobalSun;

    f32 m_ShadowSoftness;
    u32 m_SoftShadowSamples;

    static void EnumerateClass( Reflect::Compositor<DirectionalLight>& comp );
  };

  typedef Nocturnal::SmartPtr<DirectionalLight> DirectionalLightPtr;
  typedef std::vector<DirectionalLightPtr> V_DirectionalLight;
}
