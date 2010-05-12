#pragma once

#include "CharacterForms.h" 
#include "CharacterManager.h" 
#include "Content/JointAttribute.h" 

#include <map>

namespace Luna
{
  class CharacterEditor; 

  class CharacterAttributesPanel : public AttributesPanel
  {
  public: 
    CharacterAttributesPanel(CharacterEditor* editor); 
    virtual ~CharacterAttributesPanel(); 

  private: 

    void BuildTreeView(); 
    void OnSelect(wxTreeEvent& event); 

    void OnJointSelect(JointSelectionArgs& args); 
    void OnAttributeSelect(AttributeSelectionArgs& args); 
    void OnAttributeAdded(const Attribute::AttributeCollectionChanged& args);
    void OnAttributeRemoved(const Attribute::AttributeCollectionChanged& args);

    class TreeData : public wxTreeItemData
    {
    public: 
      Content::JointAttributePtr attr; 
    };

    template <class AttributeType> 
    void MakeTreeNode(wxTreeItemId root, 
                      const Content::JointTransformPtr& node,
                      const std::string& tag, int icon); 

    typedef std::map<Content::JointAttributePtr, wxTreeItemId> AttrToIdMap; 

    CharacterEditor* m_Editor; 
    AttrToIdMap       m_AttrToId; 
    i32               m_SelectedSlot; 

  }; 
}
