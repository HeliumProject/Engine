#pragma once
#include "CharacterForms.h" 
#include "CharacterManager.h" 

#include <map>

namespace Luna
{
  class CharacterEditor; 

  class CharacterLooseAttachPanel : public LooseAttachPanel
  {
  public: 
    CharacterLooseAttachPanel(CharacterEditor* editor); 
    virtual ~CharacterLooseAttachPanel(); 

    void DeleteSelectedChain(); 

  private: 
    void OnTreeSelect(wxTreeEvent& event); 

    void OnOpen(EmptyArgs& args); 
    void OnClose(EmptyArgs& args); 
    void OnLooseAttachAdded(EmptyArgs& args); 
    void OnLooseAttachRemoved(EmptyArgs& args); 
    void OnJointSelection(JointSelectionArgs& args); 

    void BuildTree(); 

    class JointData : public wxTreeItemData
    {
    public: 
      JointData(const Content::JointTransformPtr& joint)
        : m_Joint(joint)
      {

      }

      Content::JointTransformPtr   m_Joint; 
    }; 

    class ChainData : public wxTreeItemData
    {
    public: 
      ChainData(const Content::LooseAttachChainPtr& chain)
        : m_Chain(chain)
      {

      }

      Content::LooseAttachChainPtr m_Chain; 
    };

    typedef std::multimap<Content::JointTransformPtr, wxTreeItemId> MM_JointToItem; 
    typedef std::pair< MM_JointToItem::const_iterator, MM_JointToItem::const_iterator > MM_JointToItemRange; 

    CharacterEditor* m_Editor; 
    MM_JointToItem    m_JointToItems; 

  }; 

}
