#pragma once

#include "CharacterForms.h"
#include "CharacterManager.h"

namespace Luna
{
  class CharacterEditor; 

  class JointListPanel : public ::JointListPanel
  {
  public: 
    JointListPanel(const char* jointLabel, const char* indexLabel, CharacterEditor* editor); 
    virtual ~JointListPanel(); 

    virtual void OnOpen(EmptyArgs& args); 
    virtual void OnClose(EmptyArgs& args); 
    virtual void OnSelect(wxListEvent& event); 
    virtual void OnSelectInternal(JointSelectionArgs& args); 

  protected:
    void BuildList(); 

    virtual bool ShouldAdd(u32 index, const Content::JointTransformPtr& joint, i32& indexValue) = 0; 
  
    typedef std::map<Content::JointTransformPtr, int> NodeToIntMap; 
    typedef std::map<int, Content::JointTransformPtr> IntToNodeMap; 

    CharacterEditor* m_Editor; 
    NodeToIntMap      m_NodeToInt; 
    IntToNodeMap      m_IntToNode; 

  };

}


