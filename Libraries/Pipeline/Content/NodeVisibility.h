#pragma once

#include "Pipeline/API.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Serializers.h"

namespace Content
{
  //! This class contains the visibility settings for a particular
  //! scene node. It is meant to be used across multiple subtypes 
  //! of scene node, so it aggregates settings that may apply
  //! to only a particular derived type of node
  //! 
  //! This is in preference to having parallel type trees, and should
  //! make things easier to deal with in code. 
  //!
  class PIPELINE_API NodeVisibility : public Reflect::Element
  {
  public: 
    NodeVisibility(); 

    bool GetHiddenNode(); 
    void SetHiddenNode(bool hidden); 

    bool GetVisibleLayer(); 
    void SetVisibleLayer(bool visible); 

    bool GetShowGeometry(); 
    void SetShowGeometry(bool show); 

    bool GetShowBounds(); 
    void SetShowBounds(bool show); 

    bool GetShowPointer(); 
    void SetShowPointer(bool show); 

  private: 
    bool m_HiddenNode;   // from hierarchy node
    bool m_VisibleLayer; // from layer
    bool m_ShowGeometry; // from entity
    bool m_ShowBounds;   // from entity
    bool m_ShowPointer;  // from entity

  public:
    REFLECT_DECLARE_CLASS(NodeVisibility, Reflect::Element); 
    static void EnumerateClass( Reflect::Compositor<NodeVisibility>& comp );
  }; 

  typedef Nocturnal::SmartPtr<NodeVisibility> NodeVisibilityPtr; 

}
