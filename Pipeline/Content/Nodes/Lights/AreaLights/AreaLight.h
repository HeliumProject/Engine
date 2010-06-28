#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Content/Nodes/Transform/PivotTransform.h"
#include "Foundation/Reflect/Serializers.h"

namespace Content
{
  class ContentVisitor; 

  namespace AreaLightTypes
  {
    enum AreaLightType
    {
      None,
      Sphere,
      Rectangle,
      Cylinder,
      Disc,
    };
    static void AreaLightTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(None, TXT( "None" ) );
      info->AddElement(Sphere, TXT( "Sphere" ) );
      info->AddElement(Rectangle, TXT( "Rectangle" ) );
      info->AddElement(Cylinder, TXT( "Cylinder" ) );
      info->AddElement(Disc, TXT( "Disc" ) );
    }
  };

  typedef AreaLightTypes::AreaLightType AreaLightType;

  typedef std::vector<u32> SamplingDensity;

  class PIPELINE_API AreaLight : public PivotTransform
  {
  public:
    virtual void Host(ContentVisitor* visitor) = 0; 

    SamplingDensity m_RegularDensity;
    SamplingDensity m_LowDensity;
    float           m_SamplesPerMeter;

    AreaLight ()
      : PivotTransform()
      , m_SamplesPerMeter( 5.0f )
    {
      m_RegularDensity.resize(2);
      m_LowDensity.resize(2);

      m_RegularDensity[0] = 20;
      m_RegularDensity[1] = 20;

      m_LowDensity[0] = 2;
      m_LowDensity[1] = 2;
    }

    virtual void CalculateSampleDensity() = 0;

    REFLECT_DECLARE_ABSTRACT(AreaLight, Reflect::Element);

    static void EnumerateClass( Reflect::Compositor<AreaLight>& comp );

    void operator=( const AreaLight& light )
    {
      m_RegularDensity = light.m_RegularDensity;
      m_LowDensity = light.m_LowDensity;
      m_SamplesPerMeter = m_SamplesPerMeter;
    }
  };

  typedef Nocturnal::SmartPtr<AreaLight> AreaLightPtr;
  typedef std::vector<AreaLightPtr> V_AreaLight;
}