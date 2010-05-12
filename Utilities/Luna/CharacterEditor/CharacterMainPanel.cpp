#include "Precompile.h"
#include "CharacterMainPanel.h"
#include "CharacterEditor.h" 
#include "Content/HierarchyNode.h" 
#include "Content/Scene.h" 

#include "UIToolKit/ImageManager.h"

using namespace Content; 

namespace Luna
{

  CharacterMainPanel::CharacterMainPanel(CharacterEditor* editor)
    : MainPanel(editor)
    , m_Editor(editor)
  {
    CharacterManager* manager = m_Editor->GetManager(); 

    manager->OpenedEvent()->Add( GenericDelegate(this, &CharacterMainPanel::OnOpen) ); 
    manager->ClosedEvent()->Add( GenericDelegate(this, &CharacterMainPanel::OnClose) ); 
    manager->JointSelectedEvent()->Add( JointSelectionDelegate(this, &CharacterMainPanel::OnJointSelection) ); 

    m_SkeletonTree->SetImageList( UIToolKit::GlobalImageManager().GetGuiImageList() ); 

    NoFileLoaded(); 

  }

  CharacterMainPanel::~CharacterMainPanel()
  {
    CharacterManager* manager = m_Editor->GetManager(); 

    manager->OpenedEvent()->Remove( GenericDelegate(this, &CharacterMainPanel::OnOpen) ); 
    manager->ClosedEvent()->Remove( GenericDelegate(this, &CharacterMainPanel::OnClose) ); 
    manager->JointSelectedEvent()->Remove( JointSelectionDelegate(this, &CharacterMainPanel::OnJointSelection) ); 
 
  }

  void CharacterMainPanel::RecursiveBuildTree(wxTreeCtrl* skeletonTree, 
                                               const JointTransformPtr& node, 
                                               wxTreeItemId parent, 
                                               const Content::ScenePtr& scene)
  {
    static int jointIcon = UIToolKit::GlobalImageManager().GetImageIndex("joint_16.png"); 

    TreeData* data = new TreeData; 
    data->node = node; 

    // put it in the tree
    wxTreeItemId treeItem = skeletonTree->AppendItem(parent, node->GetName(), jointIcon, jointIcon, data); 

    // manage the reverse mapping so we can use it at selection time
    m_NodeToId[node] = treeItem; 

    V_HierarchyNode children; 
    scene->GetChildren(children, node); 

    for(u32 i = 0; i < children.size(); i++)
    {
      JointTransformPtr joint = Reflect::ObjectCast<Content::JointTransform>(children[i]); 

      if(joint)
      {
        RecursiveBuildTree(skeletonTree, joint, treeItem, scene); 
      }
    }
  }

  void CharacterMainPanel::OnOpen(EmptyArgs& args)
  {
    static int rootIcon = UIToolKit::GlobalImageManager().GetImageIndex("skeleton_16.png");

    CharacterManager* manager = m_Editor->GetManager(); 
    Content::ScenePtr  scene   = manager->GetScene(); 
    DocumentPtr     file    = manager->GetFile(); 

    // sort in hierarchy order. 
    // however, there may be joints that are not in a strict hierarchy
    // that's why we do multiple calls to RecursiveBuildTree below...

    V_JointTransform transforms  = scene->m_Joints; 
    std::sort(transforms.begin(), transforms.end(), HierarchySort(*scene)); 

    wxTreeCtrl* skeletonTree = m_SkeletonTree; 

    skeletonTree->DeleteAllItems(); 
    wxTreeItemId root = skeletonTree->AddRoot(file->GetFilePath(), rootIcon, rootIcon); 

    for(size_t i = 0; i < transforms.size(); ++i)
    {
      NodeToIdMap::iterator itr = m_NodeToId.find( transforms[i] ); 
      if(itr == m_NodeToId.end())
      {
        RecursiveBuildTree(skeletonTree, transforms[i], root, scene); 
      }
    }
  }

  void CharacterMainPanel::OnClose(EmptyArgs& args)
  {
    m_SkeletonTree->DeleteAllItems(); 
    m_NodeToId.clear(); 

    NoFileLoaded(); 
  }

  // helper routine to populate the tree with a nice little bit
  // of text that says < no file loaded > 
  // 
  void CharacterMainPanel::NoFileLoaded()
  {
    static int rootIcon = UIToolKit::GlobalImageManager().GetImageIndex("skeleton_16.png");
    m_SkeletonTree->AddRoot("<no file loaded>", rootIcon, rootIcon); 
  }

  void CharacterMainPanel::OnJointSelection(JointSelectionArgs& args)
  {
    wxTreeItemId treeSelected = m_SkeletonTree->GetSelection();
    TreeData*    data = NULL; 
    
    // only look up if we actually have anything
    if(treeSelected)
    {
      data = (TreeData*) m_SkeletonTree->GetItemData(treeSelected); 
    }

    const JointTransformPtr& old = (data ? data->node : NULL); 
    const JointTransformPtr& ptr = m_Editor->GetManager()->GetSelectedJoint(); 

    if(old == ptr)
    {
      m_SkeletonTree->EnsureVisible(treeSelected); 
      return; 
    }
    
    if(ptr)
    {
      wxTreeItemId item = m_NodeToId[ptr]; 
      NOC_ASSERT(item); 
      NOC_ASSERT(item ? (ptr == ((TreeData*)m_SkeletonTree->GetItemData(item))->node) : 1); 

      m_SkeletonTree->SelectItem(item, true); 
      m_SkeletonTree->EnsureVisible(item); 
    }
    else
    {
      m_SkeletonTree->SelectItem(0x0, true); 
    }

  }

  void CharacterMainPanel::OnTreeSelect(wxTreeEvent& event)
  {
    wxTreeItemId item = event.GetItem(); 

    if(!item)
    {
      return; 
    }

    TreeData* data = (TreeData*) m_SkeletonTree->GetItemData(item); 
    CharacterManager* manager = m_Editor->GetManager(); 

    if(data)
    {
      manager->SetSelectedJoint(data->node); 
    }
    else
    {
      manager->SetSelectedJoint(Content::JointTransformPtr(NULL)); 
    }
  }

  void CharacterMainPanel::OnKeyDown(wxTreeEvent& event)
  {

  }
}
