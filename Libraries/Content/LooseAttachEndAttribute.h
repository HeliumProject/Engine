#pragma once
#include "JointAttribute.h" 

namespace Content
{
  class CONTENT_API LooseAttachEndAttribute : public JointAttribute
  {
  public: 
    LooseAttachEndAttribute(); 
    ~LooseAttachEndAttribute();

  private: 

    REFLECT_DECLARE_CLASS(LooseAttachEndAttribute, JointAttribute); 
    static void EnumerateClass( Reflect::Compositor<LooseAttachEndAttribute>& comp );

  }; 

}