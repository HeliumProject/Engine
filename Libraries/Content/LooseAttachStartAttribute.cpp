#include "LooseAttachStartAttribute.h" 

namespace Content
{
  REFLECT_DEFINE_CLASS(LooseAttachStartAttribute); 

void LooseAttachStartAttribute::EnumerateClass( Reflect::Compositor<LooseAttachStartAttribute>& comp )
{
  comp.GetComposite().m_UIName = "Loose Attach Chain Start";
  Reflect::ElementField* elemChainDefaults = comp.AddField( &LooseAttachStartAttribute::m_ChainDefaults, "m_ChainDefaults" );
  elemChainDefaults->m_UIName = "Chain Default Settings";
}

    
  LooseAttachStartAttribute::LooseAttachStartAttribute()
    : m_ChainDefaults( new LooseAttachJointData )
  {
    m_ChainDefaults->AddChangedListener( Reflect::ElementChangeSignature::Delegate(this, &LooseAttachStartAttribute::OnDataChanged)); 
  }

  LooseAttachStartAttribute::~LooseAttachStartAttribute()
  {
    m_ChainDefaults->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate(this, &LooseAttachStartAttribute::OnDataChanged)); 
  }

  void LooseAttachStartAttribute::PreDeserialize()
  {
    m_ChainDefaults->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate(this, &LooseAttachStartAttribute::OnDataChanged)); 
  }

  void LooseAttachStartAttribute::PostDeserialize()
  {
    m_ChainDefaults->AddChangedListener( Reflect::ElementChangeSignature::Delegate(this, &LooseAttachStartAttribute::OnDataChanged)); 
  }
  
  void LooseAttachStartAttribute::OnDataChanged(const Reflect::ElementChangeArgs& args)
  { 
    this->RaiseChanged(NULL); 
  }

}