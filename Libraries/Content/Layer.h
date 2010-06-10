#pragma once

#include "API.h"
#include "SceneNode.h"
#include "LayerTypes.h"

namespace Content
{
  // 
  // A layer is a collection of entity instances
  // 
  class CONTENT_API Layer : public SceneNode
  {
  public:
    bool              m_Visible;
    bool              m_Selectable;
    Nocturnal::UID::V_TUID  m_Members;
    Math::Color3      m_Color;
    u32               m_Type;


    Layer();
    Layer( Nocturnal::UID::TUID& id );
    virtual ~Layer();

    REFLECT_DECLARE_CLASS( Layer, SceneNode );

    static void EnumerateClass( Reflect::Compositor<Layer>& comp );
  };

  typedef Nocturnal::SmartPtr< Layer > LayerPtr;
  typedef std::vector< LayerPtr > V_Layer;
}