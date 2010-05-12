#include "Precompile.h"
#include "JointListPanel.h"
#include "CharacterEditor.h"
#include "CharacterManager.h"

using namespace Content; 

namespace Luna
{
  JointListPanel::JointListPanel(const char* jointLabel, const char* indexLabel, CharacterEditor* editor)
    : ::JointListPanel(editor)
    , m_Editor(editor)
  {
    m_JointLister->InsertColumn(0, jointLabel); 
    m_JointLister->InsertColumn(1, indexLabel, wxLIST_FORMAT_RIGHT); 
    m_JointLister->SetColumnWidth(0, 100); 

    CharacterManager* manager = m_Editor->GetManager(); 

    manager->OpenedEvent()->Add( GenericDelegate(this, &JointListPanel::OnOpen) ); 
    manager->ClosedEvent()->Add( GenericDelegate(this, &JointListPanel::OnClose) ); 

    manager->JointSelectedEvent()->Add( JointSelectionDelegate(this, &JointListPanel::OnSelectInternal) ); 

  }

  JointListPanel::~JointListPanel()
  {
    CharacterManager* manager = m_Editor->GetManager(); 

    manager->OpenedEvent()->Remove( GenericDelegate(this, &JointListPanel::OnOpen) ); 
    manager->ClosedEvent()->Remove( GenericDelegate(this, &JointListPanel::OnClose) ); 

    manager->JointSelectedEvent()->Remove( JointSelectionDelegate(this, &JointListPanel::OnSelectInternal) ); 
  }

  void JointListPanel::BuildList()
  {
    // clear the list, because this could be called from an event handler 
    // after they have already been populated
    // 

    m_JointLister->DeleteAllItems(); 
    m_NodeToInt.clear(); 
    m_IntToNode.clear(); 

    wxListCtrl* list = m_JointLister; 

    CharacterManager* manager = m_Editor->GetManager(); 
    Content::ScenePtr  scene   = manager->GetScene(); 

    const JointOrderingPtr& jointOrdering = scene->GetJointOrdering(); 

    u32 counter = 0; 
    for(u32 i = 0; i < jointOrdering->m_JointOrdering.size(); i++)
    {
      const UniqueID::TUID& uid = jointOrdering->m_JointOrdering[i]; 
      JointTransformPtr joint = scene->Get<JointTransform>(uid); 

      i32 uniqueId = -1; 

      if( ShouldAdd(i, joint, uniqueId) )
      {
        char buffer[32]; 

        list->InsertItem(counter, joint->GetName().c_str()); 
        list->SetItem(counter, 1, itoa(uniqueId, buffer, 10)); 

        m_NodeToInt[joint] = counter; 
        m_IntToNode[counter] = joint; 

        counter++; 
      }

    }
 

  }

  void JointListPanel::OnOpen(EmptyArgs& args)
  {
    BuildList(); 
  }

  void JointListPanel::OnClose(EmptyArgs& args)
  {
    m_NodeToInt.clear(); 
    m_IntToNode.clear(); 

    wxListCtrl* list = m_JointLister; 
    list->DeleteAllItems(); 
  }

  void JointListPanel::OnSelectInternal(JointSelectionArgs& args)
  {
    Content::JointTransformPtr nextJoint = m_Editor->GetManager()->GetSelectedJoint(); 
    Content::JointTransformPtr prevJoint = NULL; 

    long prevId = m_JointLister->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); 
    long nextId = -1; 

    if(prevId >= 0)
    {
      IntToNodeMap::iterator it = m_IntToNode.find(prevId); 
      if(it != m_IntToNode.end())
      {
        prevJoint = it->second; 
      }
    }
    
    NodeToIntMap::iterator it = m_NodeToInt.find(nextJoint); 
    if(it != m_NodeToInt.end())
    {
      nextId = it->second; 
    }

    // we are not changing selection, as far as we are concerned
    if(prevId == nextId && prevId >= 0)
    {
      m_JointLister->EnsureVisible(prevId); 
      return; 
    }

    if(prevId >= 0)
    {
      //prevJoint->RemoveChangedListener(...); 
      m_JointLister->SetItemState(prevId, 0, wxLIST_STATE_SELECTED); 
    }

    if(nextId >= 0)
    {
      //nextJoint->AddChangedListener(...); 
      m_JointLister->EnsureVisible(nextId); 
      m_JointLister->SetItemState(nextId, wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED,
                                          wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED); 
    }

  }

  void JointListPanel::OnSelect(wxListEvent& event)
  {
    long index = event.GetIndex(); 
    Content::JointTransformPtr node = m_IntToNode[index]; 

    if(node != m_Editor->GetManager()->GetSelectedJoint())
    {
      m_Editor->GetManager()->SetSelectedJoint(node); 
    }

  }
}
