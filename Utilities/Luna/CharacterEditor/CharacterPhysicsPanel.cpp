#include "Precompile.h" 
#include "CharacterPhysicsPanel.h"
#include "CharacterEditor.h" 
#include "CharacterManager.h" 

#include "Content/PhysicsJointAttribute.h"

using namespace Content; 
using namespace Attribute; 

namespace Luna
{
  CharacterPhysicsPanel::CharacterPhysicsPanel(CharacterEditor* editor) 
    : JointListPanel("Joint Name", "Anim Index", editor)
  {
    CharacterManager* manager = m_Editor->GetManager(); 

    manager->ForceUpdateEvent()->Add( GenericDelegate(this, &CharacterPhysicsPanel::OnForceUpdate) ); 

  }

  CharacterPhysicsPanel::~CharacterPhysicsPanel()
  {
    CharacterManager* manager = m_Editor->GetManager(); 

    manager->ForceUpdateEvent()->Remove( GenericDelegate(this, &CharacterPhysicsPanel::OnForceUpdate) ); 
  }

  void CharacterPhysicsPanel::OnForceUpdate(EmptyArgs& args)
  {
    BuildList(); 
  }

  bool CharacterPhysicsPanel::ShouldAdd(u32 index, const Content::JointTransformPtr& joint, i32& uniqueId)
  {
    if( joint->GetAttribute<PhysicsJointAttribute>() )
    {
      uniqueId = (i32) index; 
      return true; 
    }
    else
    {
      return false; 
    }
  }

  void CharacterPhysicsPanel::OnSelectInternal(JointSelectionArgs& args)
  {
    Content::JointTransformPtr prevJoint = args.prevJoint; 
    Content::JointTransformPtr nextJoint = args.nextJoint; 

    if(prevJoint)
    {
      prevJoint->RemoveAttributeAddedListener( AttributeCollectionChangedSignature::Delegate(this, &CharacterPhysicsPanel::OnAttributeAdded)); 
      prevJoint->RemoveAttributeRemovedListener( AttributeCollectionChangedSignature::Delegate(this, &CharacterPhysicsPanel::OnAttributeRemoved)); 
    }

    if(nextJoint)
    {
      nextJoint->AddAttributeAddedListener( AttributeCollectionChangedSignature::Delegate(this, &CharacterPhysicsPanel::OnAttributeAdded)); 
      nextJoint->AddAttributeRemovedListener( AttributeCollectionChangedSignature::Delegate(this, &CharacterPhysicsPanel::OnAttributeRemoved)); 
    }

    __super::OnSelectInternal(args); 
  }

  // why can't i use ObjectCast in these two functions? presumably it has something
  // to do with const-ness, but i'm not sure what. or else i am stupid
  // 

  void CharacterPhysicsPanel::OnAttributeAdded(const Attribute::AttributeCollectionChanged& args)
  {
    if(args.m_Attribute->HasType( Reflect::GetType<PhysicsJointAttribute>() ) )
    {
      BuildList(); 
    }
  }

  void CharacterPhysicsPanel::OnAttributeRemoved(const Attribute::AttributeCollectionChanged& args)
  {
    if(args.m_Attribute->HasType( Reflect::GetType<PhysicsJointAttribute>() ) )
    {
      BuildList(); 
    }
  }

}
