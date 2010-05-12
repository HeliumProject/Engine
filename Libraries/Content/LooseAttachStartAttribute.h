#pragma once
#include "JointAttribute.h" 
#include "LooseAttachJointData.h"

namespace Content
{
  class CONTENT_API LooseAttachStartAttribute : public JointAttribute
  {
  public: 
    LooseAttachStartAttribute(); 
    ~LooseAttachStartAttribute(); 

    const LooseAttachJointDataPtr& GetJointData()
    {
      return m_ChainDefaults; 
    }

  private: 
    LooseAttachJointDataPtr m_ChainDefaults; 

    REFLECT_DECLARE_CLASS(LooseAttachStartAttribute, JointAttribute); 
    static void EnumerateClass( Reflect::Compositor<LooseAttachStartAttribute>& comp );

    virtual void PreDeserialize(); 
    virtual void PostDeserialize(); 
    void OnDataChanged(const Reflect::ElementChangeArgs& args); 

  }; 

}