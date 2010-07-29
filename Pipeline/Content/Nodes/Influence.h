#pragma once

#include "Pipeline/API.h"
#include "Foundation/Reflect/Serializers.h"

namespace Content
{
  // Influence stores the object ID indices and weights of transforms that influence or deform a vertex
  class PIPELINE_API Influence : public Reflect::Element
  {
  public:
    // The object ID indices of the transforms that influence this vertex
    std::vector< u32 > m_Objects;

    // The weights of the transforms that influence this vertex (CORRESPONDS TO objects)
    std::vector< f32 > m_Weights;

    REFLECT_DECLARE_CLASS(Influence, Reflect::Element);

    static void EnumerateClass( Reflect::Compositor<Influence>& comp );
  };

  typedef Helium::SmartPtr<Influence> InfluencePtr;
  typedef std::vector<InfluencePtr> V_Influence;
}