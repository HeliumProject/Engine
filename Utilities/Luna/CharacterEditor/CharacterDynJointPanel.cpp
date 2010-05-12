#include "Precompile.h" 
#include "CharacterDynJointPanel.h"
#include "CharacterEditor.h" 
#include "CharacterManager.h" 

using namespace Content; 

namespace Luna
{
  CharacterDynJointPanel::CharacterDynJointPanel(CharacterEditor* editor) 
    : JointListPanel("Dynamic Joint", "Game ID", editor)
  {

  }

  CharacterDynJointPanel::~CharacterDynJointPanel()
  {
 
  }

  bool CharacterDynJointPanel::ShouldAdd(u32 index, const Content::JointTransformPtr& joint, i32& uniqueId)
  {
    if(joint->IsDynamicJoint())
    {
      uniqueId = joint->GetDynamicJointID(); 
    }

    return joint->IsDynamicJoint(); 
  }

}
