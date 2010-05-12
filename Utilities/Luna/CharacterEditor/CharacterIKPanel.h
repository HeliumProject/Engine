#pragma once

#include "JointListPanel.h"
#include "CharacterManager.h"
#include "Attribute/AttributeCollection.h" 

namespace Luna
{
  class CharacterEditor; 

  class CharacterIKPanel : public JointListPanel
  {
  public: 
    CharacterIKPanel(CharacterEditor* editor); 
    virtual ~CharacterIKPanel(); 

    virtual bool ShouldAdd(u32 index, const Content::JointTransformPtr& joint, i32& uniqueId) NOC_OVERRIDE; 
    virtual void OnSelectInternal(JointSelectionArgs& args) NOC_OVERRIDE; 

  private: 
    void OnForceUpdate(EmptyArgs& args); 
    void OnAttributeAdded(const Attribute::AttributeCollectionChanged& args); 
    void OnAttributeRemoved(const Attribute::AttributeCollectionChanged& args); 

  }; 

}
