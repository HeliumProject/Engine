#pragma once

#include "Pipeline/API.h"
#include "ParametricKey.h"
#include "Reflect/SimpleSerializer.h"

namespace Content
{
  class PIPELINE_API ParametricColorKey : public ParametricKey
  {
  public:
    Math::Color3 m_Color;

  public:
    ParametricColorKey();
    virtual ~ParametricColorKey();

    virtual Math::Color3 GetColor() const NOC_OVERRIDE;
    virtual void SetColor( const Math::Color3& color ) NOC_OVERRIDE;

    REFLECT_DECLARE_CLASS( ParametricColorKey, ParametricKey );

    static void EnumerateClass( Reflect::Compositor<ParametricColorKey>& comp );
  };
  typedef Nocturnal::SmartPtr< ParametricColorKey > ParametricColorKeyPtr;
  typedef std::vector< ParametricColorKeyPtr > V_ParametricColorKeyPtr;
}