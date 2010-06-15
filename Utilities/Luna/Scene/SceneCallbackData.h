#pragma once

#include "Luna/API.h"

namespace Luna
{
  class SceneNode;
  class SceneNodeType;
  class InstanceSet;

  namespace ContextCallbackTypes
  {
    enum ContextCallbackType
    {
      All,
      Item,

      Instance,

      Entity_Visible_Geometry,
      Entity_Invisible_Geometry,

      Count
    };
  }

  class ContextCallbackData: public wxObject
  {
  public:
    ContextCallbackData();
    virtual ~ContextCallbackData();

    ContextCallbackTypes::ContextCallbackType m_ContextCallbackType;
    const Luna::SceneNodeType* m_NodeType;
    const Luna::InstanceSet* m_InstanceSet;
    Luna::SceneNode* m_NodeInstance;
  };

  class GeneralCallbackData: public wxObject
  {
  public:
    GeneralCallbackData();
    virtual ~GeneralCallbackData();

    void* m_GeneralData;
  };
}
