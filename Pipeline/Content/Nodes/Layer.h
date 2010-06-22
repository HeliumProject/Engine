#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Content/Nodes/SceneNode.h"
#include "LayerTypes.h"

namespace Content
{
  // 
  // A layer is a collection of entity instances
  // 
  class PIPELINE_API Layer : public SceneNode
  {
  public:
    bool              m_Visible;
    bool              m_Selectable;
    Nocturnal::V_TUID  m_Members;
    Math::Color3      m_Color;
    u32               m_Type;


    Layer();
    Layer( Nocturnal::TUID& id );
    virtual ~Layer();

    REFLECT_DECLARE_CLASS( Layer, SceneNode );

    static void EnumerateClass( Reflect::Compositor<Layer>& comp );
  };

  typedef Nocturnal::SmartPtr< Layer > LayerPtr;
  typedef std::vector< LayerPtr > V_Layer;
}