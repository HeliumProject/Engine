#pragma once

#include "Pipeline/API.h"
#include "ParametricKey.h"
#include "Foundation/Reflect/SimpleSerializer.h"

namespace Content
{
  class PIPELINE_API ParametricColorKey : public ParametricKey
  {
  public:
    Math::Color3 m_Color;

  public:
    ParametricColorKey();
    virtual ~ParametricColorKey();

    virtual Math::Color3 GetColor() const HELIUM_OVERRIDE;
    virtual void SetColor( const Math::Color3& color ) HELIUM_OVERRIDE;

    REFLECT_DECLARE_CLASS( ParametricColorKey, ParametricKey );

    static void EnumerateClass( Reflect::Compositor<ParametricColorKey>& comp );
  };
  typedef Helium::SmartPtr< ParametricColorKey > ParametricColorKeyPtr;
  typedef std::vector< ParametricColorKeyPtr > V_ParametricColorKeyPtr;
}