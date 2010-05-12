#pragma once

#include "API.h"
#include "Reflect/Element.h"
#include "NodeVisibility.h"

namespace Content
{
  typedef std::map<tuid, NodeVisibilityPtr> M_TuidVisibility; 

  class CONTENT_API SceneVisibility : public Reflect::Element
  {
  public: 
    SceneVisibility(); 

    /// @brief return the scene visibility options for this node if they exist
    /// If the visibility options for this node have not yet been set, this 
    /// function will try to fallback on a different set of visibility settings provided.
    /// If they do not exist, then return the default visibility settings.
    /// @param nodeId the id of the node who's visibility settings we want
    /// @param fallbackId the id of a fallback set of visibility settings we want
    /// @return return a pointer to visibility settings if they exist in the preference order Node>Fallback>Default
    NodeVisibilityPtr GetVisibility(tuid nodeId, tuid fallbackId = UniqueID::TUID::Null); 

    void ActivateNode(tuid nodeId); 
    void DeactivateNode(tuid nodeId); 

    void SetNodeDefaults(const NodeVisibilityPtr& nodeDefaults); 

  public: 
    M_TuidVisibility m_NodeVisibility; 
    M_TuidVisibility m_TempVisibility; // not saved, used for temporary nodes
    NodeVisibilityPtr m_NodeDefaults; // not saved 

  public: 
    REFLECT_DECLARE_CLASS(SceneVisibility, Reflect::Element); 
    static void EnumerateClass( Reflect::Compositor<SceneVisibility>& comp );

  }; 

  typedef Nocturnal::SmartPtr<SceneVisibility> SceneVisibilityPtr; 
}
