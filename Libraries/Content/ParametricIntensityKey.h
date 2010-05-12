#pragma once

#include "API.h"
#include "ParametricKey.h"
#include "Reflect/SimpleSerializer.h"

namespace Content
{
  class CONTENT_API ParametricIntensityKey : public ParametricKey
  {
  public:
    u8 m_Intensity;

  public:
    ParametricIntensityKey();
    virtual ~ParametricIntensityKey();

    virtual Math::Color3 GetColor() const NOC_OVERRIDE;
    virtual void SetColor( const Math::Color3& color ) NOC_OVERRIDE;

    REFLECT_DECLARE_CLASS( ParametricIntensityKey, ParametricKey );

    static void EnumerateClass( Reflect::Compositor<ParametricIntensityKey>& comp );
  };
  typedef Nocturnal::SmartPtr< ParametricIntensityKey > ParametricIntensityKeyPtr;
  typedef std::vector< ParametricIntensityKeyPtr > V_ParametricIntensityKeyPtr;
}