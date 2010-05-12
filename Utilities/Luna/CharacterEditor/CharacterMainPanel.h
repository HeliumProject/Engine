#pragma once

#include "CharacterForms.h" 
#include "CharacterManager.h" 
#include "Content/Scene.h" 
#include "Content/HierarchyNode.h" 

namespace Luna
{
  class CharacterEditor; 

  class CharacterMainPanel : public MainPanel
  {
  public: 
    CharacterMainPanel(CharacterEditor* editor); 
    virtual ~CharacterMainPanel(); 

  private: 

    void OnOpen(EmptyArgs& args); 
    void OnClose(EmptyArgs& args); 
    void OnJointSelection(JointSelectionArgs& args); 

    void OnTreeSelect(wxTreeEvent& event); 
    void OnKeyDown(wxTreeEvent& event); 

    void NoFileLoaded(); 

    class TreeData : public wxTreeItemData
    {
    public: 
      Content::JointTransformPtr node; 
    }; 

    void RecursiveBuildTree(wxTreeCtrl* skeletonTree, 
                            const Content::JointTransformPtr& node, 
                            wxTreeItemId parent, 
                            const Content::ScenePtr& scene); 

    typedef std::map<Content::JointTransformPtr, wxTreeItemId> NodeToIdMap; 

    CharacterEditor* m_Editor; 
    NodeToIdMap       m_NodeToId; 
  };
} 
