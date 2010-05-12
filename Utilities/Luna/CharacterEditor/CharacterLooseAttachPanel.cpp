#include "Precompile.h" 
#include "CharacterLooseAttachPanel.h" 
#include "CharacterEditor.h"

#include "Asset/SkeletonAsset.h" 
#include "Content/LooseAttachChain.h"

#include "UIToolKit/ImageManager.h"

using namespace Asset; 
using namespace Content; 

namespace Luna
{
  CharacterLooseAttachPanel::CharacterLooseAttachPanel(CharacterEditor* editor)
    : LooseAttachPanel(editor)
    , m_Editor(editor)
  {
    CharacterManager* manager = m_Editor->GetManager(); 

    manager->OpenedEvent()->Add( GenericDelegate(this, &CharacterLooseAttachPanel::OnOpen) ); 
    manager->ClosedEvent()->Add( GenericDelegate(this, &CharacterLooseAttachPanel::OnClose) ); 
    manager->LooseAttachmentAddedEvent()->Add( GenericDelegate(this, &CharacterLooseAttachPanel::OnLooseAttachAdded) ); 
    manager->LooseAttachmentRemovedEvent()->Add( GenericDelegate(this, &CharacterLooseAttachPanel::OnLooseAttachRemoved) ); 

    manager->JointSelectedEvent()->Add( JointSelectionDelegate(this, &CharacterLooseAttachPanel::OnJointSelection) ); 

    m_LooseAttachTree->SetImageList( UIToolKit::GlobalImageManager().GetGuiImageList() ); 

    BuildTree(); 
  }

  CharacterLooseAttachPanel::~CharacterLooseAttachPanel()
  {
    CharacterManager* manager = m_Editor->GetManager(); 

    manager->OpenedEvent()->Remove( GenericDelegate(this, &CharacterLooseAttachPanel::OnOpen) ); 
    manager->ClosedEvent()->Remove( GenericDelegate(this, &CharacterLooseAttachPanel::OnClose) ); 
    manager->LooseAttachmentAddedEvent()->Remove( GenericDelegate(this, &CharacterLooseAttachPanel::OnLooseAttachAdded) ); 
    manager->LooseAttachmentRemovedEvent()->Remove( GenericDelegate(this, &CharacterLooseAttachPanel::OnLooseAttachRemoved) ); 
    manager->JointSelectedEvent()->Remove( JointSelectionDelegate(this, &CharacterLooseAttachPanel::OnJointSelection) ); 
  }

  void CharacterLooseAttachPanel::OnOpen(EmptyArgs& args)
  {
    BuildTree(); 
  }

  void CharacterLooseAttachPanel::OnClose(EmptyArgs& args)
  {
    BuildTree(); 
  }

  void CharacterLooseAttachPanel::OnLooseAttachAdded(EmptyArgs& args)
  {
    BuildTree(); 
  }

  void CharacterLooseAttachPanel::OnLooseAttachRemoved(EmptyArgs& args)
  {
    BuildTree(); 
  }

  void CharacterLooseAttachPanel::BuildTree()
  {
    static int chainIcon = UIToolKit::GlobalImageManager().GetImageIndex("folder_16.png"); 
    static int jointIcon = UIToolKit::GlobalImageManager().GetImageIndex("joint_16.png"); 

    CharacterManager* manager = m_Editor->GetManager(); 

    m_LooseAttachTree->DeleteAllItems(); 
    m_JointToItems.clear(); 
    
    if(manager->GetScene() == NULL)
    {
      m_LooseAttachTree->AddRoot("<no file loaded>"); 
      return; 
    }

    wxTreeItemId root = m_LooseAttachTree->AddRoot("Loose Attach Chains"); 
    V_LooseAttachChain& chains = manager->GetSkeletonAsset()->GetLooseAttachChains(); 

    for(size_t i = 0; i < chains.size(); ++i)
    {
      wxTreeItemId chainItem = m_LooseAttachTree->AppendItem(root, chains[i]->m_Name, chainIcon, chainIcon, new ChainData(chains[i])); 

      int count = (chains[i]->GetChainType() == LooseAttachChain::SingleEnded) ? 1 : 3; 
      for(int j = 0; j < count; j++)
      {
        const JointTransformPtr& joint = chains[i]->GetJoint( (LooseAttachChain::JointIndex) j); 
        wxTreeItemId jointItem = m_LooseAttachTree->AppendItem(chainItem, joint->GetName(), jointIcon, jointIcon, new JointData(joint)); 

        m_JointToItems.insert( MM_JointToItem::value_type(joint, jointItem) );

      }
      
      m_LooseAttachTree->Expand(chainItem); 
    }
  }

  void CharacterLooseAttachPanel::OnJointSelection(JointSelectionArgs& args)
  {
    // since we may have the same joint multiple times in the tree view, 
    // we don't cause anything to become selected
    // 
    // however, we also don't invalidate the current selection if it is 
    // an instance of the joint we've just selected through the manager
    // 

    bool             seenSelected = false; 
    wxTreeItemId     treeSelected = m_LooseAttachTree->GetSelection();
    wxTreeItemData*  treeData     = NULL; 
    bool             treeHasJoint = false; 

    if(treeSelected)
    {
      treeData = m_LooseAttachTree->GetItemData(treeSelected); 
      if(dynamic_cast<JointData*>(treeData))
      {
        treeHasJoint = true; 
      }
    }

    // if we have a chain selected, and someone tells us that there is
    // no selected joint, then we don't have to do anything, because we
    // don't have a joint selected anyway. 
    // 
    if(treeHasJoint == false && args.nextJoint == NULL)
    {
      return; 
    }

    MM_JointToItemRange range = m_JointToItems.equal_range(args.nextJoint); 
    int appearances = 0; 

    for(MM_JointToItem::const_iterator it = range.first; it != range.second; ++it)
    {
      wxTreeItemId item = it->second; 
      m_LooseAttachTree->EnsureVisible( item ); 

      if(item == treeSelected)
      {
        seenSelected = true; 
      }

      appearances++;  
    }

    if(seenSelected)
    {
      return; 
    }

    // we did not see the selected tree instance on our traversal. 
    // if we have only one item pointing to this joint, select it

    if(appearances == 1)
    {
      m_LooseAttachTree->SelectItem(range.first->second, true); 
    }
    else
    {
      // set selection to 0x0 -- SelectItem(.., false) would only work for multiselect ctrls. 
      m_LooseAttachTree->SelectItem(0x0, true); 
    }

  }

  void CharacterLooseAttachPanel::OnTreeSelect(wxTreeEvent& event)
  {
    wxTreeItemId item = event.GetItem(); 
    if(!item)
    {
      return; 
    }

    CharacterManager* manager = m_Editor->GetManager(); 
    JointData* data = dynamic_cast<JointData*>(m_LooseAttachTree->GetItemData(item)); 
    if(data)
    {
      manager->SetSelectedJoint(data->m_Joint); 
    }
    else
    {
      manager->SetSelectedJoint(Content::JointTransformPtr(NULL)); 
    }
  }

  void CharacterLooseAttachPanel::DeleteSelectedChain()
  {
    wxTreeItemId treeSelected = m_LooseAttachTree->GetSelection();
    if(!treeSelected)
    {
      return; 
    }

    wxTreeItemData* treeData = m_LooseAttachTree->GetItemData(treeSelected); 
    ChainData*  chainData = dynamic_cast<ChainData*>(treeData); 

    if(chainData)
    {
      m_Editor->GetManager()->RemoveLooseAttachChain(chainData->m_Chain); 
    }
  }
}
