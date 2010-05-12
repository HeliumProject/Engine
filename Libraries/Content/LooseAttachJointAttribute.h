#pragma once
#include "JointAttribute.h" 
#include "LooseAttachJointData.h" 

namespace Content
{
  class CONTENT_API LooseAttachJointAttribute : public JointAttribute
  {
  public: 
    LooseAttachJointAttribute(); 
    ~LooseAttachJointAttribute(); 

    const LooseAttachJointDataPtr& GetJointData()
    {
      return m_JointData; 
    }

  private: 
    LooseAttachJointDataPtr m_JointData; 

    REFLECT_DECLARE_CLASS(LooseAttachJointAttribute, JointAttribute); 
    static void EnumerateClass( Reflect::Compositor<LooseAttachJointAttribute>& comp );

    virtual void PreDeserialize(); 
    virtual void PostDeserialize(); 
    void OnDataChanged(const Reflect::ElementChangeArgs& args); 

  }; 

}