#pragma once

#include "Application/API.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Math/Color3.h"
#include "Reflect/Element.h"

namespace Inspect
{
  /////////////////////////////////////////////////////////////////////////////
  // Represents a color at a position along a gradient color control.
  // 
  class APPLICATION_API Key : public Reflect::Element
  {
  public:
    enum Constants
    {
      InvalidKey = -1,
    };

  private:
    Math::Color3 m_Color;
    float m_Location;

    friend class KeyArray;

    // RTTI
  public:
    REFLECT_DECLARE_CLASS( Key, Reflect::Element );
    static void EnumerateClass( Reflect::Compositor< Key >& comp );

  public:
    Key();
    Key( const Math::Color3& color, float location );

    Math::Color3 GetColor() const;

    float GetLocation() const;
  };
  typedef Nocturnal::SmartPtr< Key > KeyPtr;
  typedef std::vector< KeyPtr > V_KeyPtr;
}
