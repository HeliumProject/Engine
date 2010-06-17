#pragma once

#include "Pipeline/API.h"
#include "Light.h"

namespace Content
{
  namespace VolumeLightTypes
  {
    enum VolumeLightType
    {
      Sphere,
      Cuboid,
    };
    static void VolumeLightTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {

      info->AddElement(Sphere, "Sphere");
      info->AddElement(Cuboid, "Cuboid");
    }
  };

  typedef VolumeLightTypes::VolumeLightType VolumeLightType;

  class PIPELINE_API AmbientVolumeLight : public Light
  {
  public:
    virtual void Host(ContentVisitor* visitor); 

    float m_InnerRadius;
    float m_OuterRadius;
    VolumeLightType m_VolumeLightType;

    AmbientVolumeLight ()
      : m_InnerRadius( 0.0f )
      , m_OuterRadius( 1.0f )
      , m_VolumeLightType( VolumeLightTypes::Sphere )
    {

    }

    AmbientVolumeLight (Nocturnal::TUID &id)
      : m_InnerRadius( 0.0f )
      , m_OuterRadius( 1.0f )
      , m_VolumeLightType( VolumeLightTypes::Sphere )
    {

    }

    REFLECT_DECLARE_CLASS(AmbientVolumeLight, Light);

    static void EnumerateClass( Reflect::Compositor<AmbientVolumeLight>& comp );

  };

  typedef Nocturnal::SmartPtr<AmbientVolumeLight> AmbientVolumeLightPtr;
  typedef std::vector<AmbientVolumeLightPtr> V_AmbientVolumeLight;
}