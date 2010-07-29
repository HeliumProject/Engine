#pragma once

#include "Pipeline/API.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/SimpleSerializer.h"

namespace Content
{
  class PIPELINE_API ParametricKey HELIUM_ABSTRACT : public Reflect::Element
  {
  public:
    f32 m_Param;

  public:
    ParametricKey();
    virtual ~ParametricKey();

    virtual void PostDeserialize() HELIUM_OVERRIDE;
    virtual void PreSerialize() HELIUM_OVERRIDE;

    virtual Math::Color3 GetColor() const = 0;
    virtual void SetColor( const Math::Color3& color ) = 0;

    REFLECT_DECLARE_ABSTRACT( ParametricKey, Reflect::Element );

    static void EnumerateClass( Reflect::Compositor<ParametricKey>& comp );
  };
  typedef Helium::SmartPtr< ParametricKey > ParametricKeyPtr;
  typedef std::vector< ParametricKeyPtr > V_ParametricKeyPtr;
}