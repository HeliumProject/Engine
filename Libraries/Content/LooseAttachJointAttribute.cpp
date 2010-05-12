#include "LooseAttachJointAttribute.h" 

namespace Content
{
  REFLECT_DEFINE_CLASS(LooseAttachJointAttribute); 

void LooseAttachJointAttribute::EnumerateClass( Reflect::Compositor<LooseAttachJointAttribute>& comp )
{
  comp.GetComposite().m_UIName = "Loose Attach Joint";
  Reflect::ElementField* elemJointData = comp.AddField( &LooseAttachJointAttribute::m_JointData, "m_JointData" );
  elemJointData->m_UIName = "Loose Attach Settings";
}


  LooseAttachJointAttribute::LooseAttachJointAttribute()
    : m_JointData( new LooseAttachJointData() )
  {
    m_JointData->AddChangedListener( Reflect::ElementChangeSignature::Delegate(this, &LooseAttachJointAttribute::OnDataChanged)); 
  }

  LooseAttachJointAttribute::~LooseAttachJointAttribute()
  {
    m_JointData->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate(this, &LooseAttachJointAttribute::OnDataChanged)); 
  }

  void LooseAttachJointAttribute::PreDeserialize()
  {
    m_JointData->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate(this, &LooseAttachJointAttribute::OnDataChanged)); 
  }

  void LooseAttachJointAttribute::PostDeserialize()
  {
    m_JointData->AddChangedListener( Reflect::ElementChangeSignature::Delegate(this, &LooseAttachJointAttribute::OnDataChanged)); 
  }

  void LooseAttachJointAttribute::OnDataChanged(const Reflect::ElementChangeArgs& args)
  {
    this->RaiseChanged(NULL); 
  }

}