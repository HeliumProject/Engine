#include "Precompile.h"
#include "CharacterIKPanel.h"
#include "CharacterEditor.h"
#include "CharacterManager.h" 

#include "Content/IKJointAttribute.h" 

using namespace Content; 
using namespace Attribute; 

namespace Luna
{
  CharacterIKPanel::CharacterIKPanel(CharacterEditor* editor) 
    : JointListPanel("Joint Name", "Anim Index", editor)
  {
    CharacterManager* manager = m_Editor->GetManager(); 

    manager->ForceUpdateEvent()->Add( GenericDelegate(this, &CharacterIKPanel::OnForceUpdate) ); 

  }

  CharacterIKPanel::~CharacterIKPanel()
  {
    CharacterManager* manager = m_Editor->GetManager(); 

    manager->ForceUpdateEvent()->Remove( GenericDelegate(this, &CharacterIKPanel::OnForceUpdate) ); 
  }

  void CharacterIKPanel::OnForceUpdate(EmptyArgs& args)
  {
    BuildList(); 
  }

  bool CharacterIKPanel::ShouldAdd(u32 index, const Content::JointTransformPtr& joint, i32& uniqueId)
  {
    if( joint->GetAttribute<IKJointAttribute>() )
    {
      uniqueId = (i32) index; 
      return true; 
    }
    else
    {
      return false; 
    }
  }

  void CharacterIKPanel::OnSelectInternal(JointSelectionArgs& args)
  {
    Content::JointTransformPtr prevJoint = args.prevJoint; 
    Content::JointTransformPtr nextJoint = args.nextJoint; 

    if(prevJoint)
    {
      prevJoint->RemoveAttributeAddedListener( AttributeCollectionChangedSignature::Delegate(this, &CharacterIKPanel::OnAttributeAdded)); 
      prevJoint->RemoveAttributeRemovedListener( AttributeCollectionChangedSignature::Delegate(this, &CharacterIKPanel::OnAttributeRemoved)); 
    }

    if(nextJoint)
    {
      nextJoint->AddAttributeAddedListener( AttributeCollectionChangedSignature::Delegate(this, &CharacterIKPanel::OnAttributeAdded)); 
      nextJoint->AddAttributeRemovedListener( AttributeCollectionChangedSignature::Delegate(this, &CharacterIKPanel::OnAttributeRemoved)); 
    }

    __super::OnSelectInternal(args); 
  }

  // why can't i use ObjectCast in these two functions? presumably it has something
  // to do with const-ness, but i'm not sure what. or else i am stupid
  // 

  void CharacterIKPanel::OnAttributeAdded(const Attribute::AttributeCollectionChanged& args)
  {
    if(args.m_Attribute->HasType( Reflect::GetType<IKJointAttribute>() ) )
    {
      BuildList(); 
    }
  }

  void CharacterIKPanel::OnAttributeRemoved(const Attribute::AttributeCollectionChanged& args)
  {
    if(args.m_Attribute->HasType( Reflect::GetType<IKJointAttribute>() ) )
    {
      BuildList(); 
    }
  }
}
