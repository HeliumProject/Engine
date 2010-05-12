#include "Precompile.h" 
#include "CharacterAttributesPanel.h" 
#include "CharacterEditor.h" 

#include "Content/JointAttribute.h" 
#include "Content/PhysicsJointAttribute.h" 
#include "Content/LooseAttachStartAttribute.h" 
#include "Content/LooseAttachEndAttribute.h" 
#include "Content/LooseAttachJointAttribute.h" 
#include "Content/LooseAttachCollisionAttribute.h" 
#include "Content/IKJointAttribute.h"

#include "UIToolKit/ImageManager.h"

using namespace Content; 
using namespace Attribute; 

namespace Luna
{
  CharacterAttributesPanel::CharacterAttributesPanel(CharacterEditor* editor)
    : AttributesPanel(editor)
    , m_Editor(editor)
    , m_SelectedSlot(-1)
  {
    CharacterManager* manager = m_Editor->GetManager(); 

    manager->JointSelectedEvent()->Add( JointSelectionDelegate(this, &CharacterAttributesPanel::OnJointSelect)); 
    manager->AttributeSelectedEvent()->Add( AttributeSelectionDelegate(this, &CharacterAttributesPanel::OnAttributeSelect)); 


    m_AttributesTree->SetImageList( UIToolKit::GlobalImageManager().GetGuiImageList() ); 
  }

  CharacterAttributesPanel::~CharacterAttributesPanel()
  {
    CharacterManager* manager = m_Editor->GetManager(); 
    manager->JointSelectedEvent()->Remove( JointSelectionDelegate(this, &CharacterAttributesPanel::OnJointSelect)); 
    manager->AttributeSelectedEvent()->Remove( AttributeSelectionDelegate(this, &CharacterAttributesPanel::OnAttributeSelect)); 

  }

  void CharacterAttributesPanel::OnSelect(wxTreeEvent& event)
  {
    CharacterManager* manager = m_Editor->GetManager(); 
    wxTreeItemId item = event.GetItem(); 
    TreeData*    data = NULL; 
    
    if(item)
    { 
      data = (TreeData*) m_AttributesTree->GetItemData(item);
    }

    if(data)
    {
      manager->SetSelectedAttribute(data->attr); 
      m_SelectedSlot = data->attr->GetSlot(); 
    }
    else
    {
      manager->SetSelectedAttribute(JointAttributePtr(NULL)); 
      m_SelectedSlot = -1; 
    }

  }

  template <class AttributeType> 
  void CharacterAttributesPanel::MakeTreeNode(wxTreeItemId root, const JointTransformPtr& node, const std::string& tag, int icon)
  {
    Nocturnal::SmartPtr<AttributeType> ptr = node->GetAttribute<AttributeType>(); 
    if(ptr)
    {
      TreeData* data = new TreeData; 
      data->attr = ptr; 

      wxTreeItemId id = m_AttributesTree->AppendItem(root, 
                                                     tag.c_str(), 
                                                     icon, icon,
                                                     data); 
      m_AttrToId[ptr] = id; 

    }
  }

  void CharacterAttributesPanel::BuildTreeView()
  {
    static int physicsIcon           = UIToolKit::GlobalImageManager().GetImageIndex("attribute_physics_16.png"); // greenish box
    static int ikIcon                = UIToolKit::GlobalImageManager().GetImageIndex("attribute_ik_16.png"); // blueish box
    static int looseAttachEndIcon    = UIToolKit::GlobalImageManager().GetImageIndex("attribute_procedural_animation_16.png"); // maroon joint
    static int looseAttachNormalIcon = UIToolKit::GlobalImageManager().GetImageIndex("attribute_animation_16.png"); // green joint
    static int rootIcon              = UIToolKit::GlobalImageManager().GetImageIndex("attribute_attributes_16.png"); 

    // rebuild the panel display... 
    // 
    CharacterManager* manager = m_Editor->GetManager(); 
    const Content::JointTransformPtr& node = manager->GetSelectedJoint(); 

    m_AttributesTree->Freeze(); 

    m_AttributesTree->DeleteAllItems(); 
    m_AttrToId.clear(); 

    if(node == NULL)
    {
      wxTreeItemId root = m_AttributesTree->AddRoot("<no joint selected>", rootIcon); 
      m_AttributesTree->Thaw(); 
      return; 
    }

    std::string rootTag("Joint: "); 
    rootTag += node->GetName(); 

    wxTreeItemId root = m_AttributesTree->AddRoot(rootTag.c_str(), rootIcon); 

    // explicitly lookup and add the nodes that we support...
    // 
    MakeTreeNode<PhysicsJointAttribute>(root, node, "Physics Joint", physicsIcon); 
    MakeTreeNode<LooseAttachStartAttribute>(root, node, "Loose Attach Chain Start", looseAttachEndIcon); 
    MakeTreeNode<LooseAttachEndAttribute>(root, node, "Loose Attach Chain End", looseAttachEndIcon); 
    MakeTreeNode<LooseAttachJointAttribute>(root, node, "Loose Attach Joint", looseAttachNormalIcon); 
    MakeTreeNode<LooseAttachCollisionAttribute>(root, node, "Loose Attach Collision", looseAttachNormalIcon); 
    MakeTreeNode<IKJointAttribute>(root, node, "IK Joint", ikIcon); 

    m_AttributesTree->Expand(root); 
    m_AttributesTree->Thaw(); 

    // if we had something selected before, and we have just 
    // moved to a different joint, we want to try and keep it selected

    JointAttributePtr attr; 

    if(m_SelectedSlot != -1)
    {
      attr = Reflect::ObjectCast<JointAttribute>(node->GetAttribute(m_SelectedSlot)); 
    }

    // if we have that attribute, select it. 
    // otherwise, just select the first one that we do have..
    // 
    
    if(attr)
    {
      wxTreeItemId item = m_AttrToId[attr]; 
      m_AttributesTree->SelectItem(item);  
    }
    else
    {
      AttrToIdMap::iterator it = m_AttrToId.begin(); 
      if(it != m_AttrToId.end())
      {
        const JointAttributePtr& attr = it->first; 
        wxTreeItemId             item = it->second; 

        // yes, this causes an "OnSelect" event above, and 
        // will reset the "selected slot"
        //
        m_AttributesTree->SelectItem(item); 
        
      }
    }
  }

  void CharacterAttributesPanel::OnJointSelect(JointSelectionArgs& args)
  {
    const Content::JointTransformPtr& nextJoint = args.nextJoint; 
    const Content::JointTransformPtr& prevJoint = args.prevJoint; 

    if(prevJoint)
    {
      prevJoint->RemoveAttributeAddedListener( AttributeCollectionChangedSignature::Delegate(this, &CharacterAttributesPanel::OnAttributeAdded)); 
      prevJoint->RemoveAttributeRemovedListener( AttributeCollectionChangedSignature::Delegate(this, &CharacterAttributesPanel::OnAttributeRemoved)); 
    }

    if(nextJoint)
    {
      nextJoint->AddAttributeAddedListener( AttributeCollectionChangedSignature::Delegate(this, &CharacterAttributesPanel::OnAttributeAdded)); 
      nextJoint->AddAttributeRemovedListener( AttributeCollectionChangedSignature::Delegate(this, &CharacterAttributesPanel::OnAttributeRemoved)); 
    }

    BuildTreeView(); 
  }

  void CharacterAttributesPanel::OnAttributeSelect(AttributeSelectionArgs& args)
  {
    Content::JointAttributePtr nextAttr = Reflect::ObjectCast<Content::JointAttribute>(args.nextAttr); 
    Content::JointAttributePtr prevAttr = Reflect::ObjectCast<Content::JointAttribute>(args.prevAttr); 

    if(nextAttr)
    {
      wxTreeItemId item = m_AttrToId[nextAttr]; 

      if(item)
      {
        // yes, this causes an "OnSelect" event above, and 
        // will reset the "selected slot"
        //
        // it will also call back into m_Manager->SetSelectedAttribute
        // but that will be fine, because that tests for changes. 
        //
        m_AttributesTree->SelectItem(item); 
      }
    }

  }

  void CharacterAttributesPanel::OnAttributeAdded(const Attribute::AttributeCollectionChanged& args)
  {
    BuildTreeView(); 
  }

  void CharacterAttributesPanel::OnAttributeRemoved(const Attribute::AttributeCollectionChanged& args)
  {
    BuildTreeView(); 
  }

}
