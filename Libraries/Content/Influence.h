#pragma once

#include "API.h"
#include "Reflect/Serializers.h"

namespace Content
{
  // Influence stores the object ID indices and weights of transforms that influence or deform a vertex
  class CONTENT_API Influence : public Reflect::Element
  {
  public:
    // The object ID indices of the transforms that influence this vertex
    V_u32 m_Objects;

    // The weights of the transforms that influence this vertex (CORRESPONDS TO objects)
    V_f32 m_Weights;

    REFLECT_DECLARE_CLASS(Influence, Reflect::Element);

    static void EnumerateClass( Reflect::Compositor<Influence>& comp );
  };

  typedef Nocturnal::SmartPtr<Influence> InfluencePtr;
  typedef std::vector<InfluencePtr> V_Influence;
}