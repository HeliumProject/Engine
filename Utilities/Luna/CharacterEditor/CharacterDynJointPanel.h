#pragma once

#include "JointListPanel.h"
#include "CharacterManager.h" 

namespace Luna
{
  class CharacterEditor; 

  class CharacterDynJointPanel : public JointListPanel
  {
    public:
      CharacterDynJointPanel(CharacterEditor* editor); 
      virtual ~CharacterDynJointPanel(); 

      virtual bool ShouldAdd(u32 index, const Content::JointTransformPtr& joint, i32& uniqueId) NOC_OVERRIDE; 
  };

}
