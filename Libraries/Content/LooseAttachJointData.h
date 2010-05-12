#pragma once

#include "API.h" 
#include "Reflect/Serializers.h" 

namespace Content
{
  class CONTENT_API LooseAttachJointData : public Reflect::Element
  {
  public: 
    LooseAttachJointData(); 
    ~LooseAttachJointData(); 

    f32  m_Mass; 
    f32  m_SolidAngleLimit; 
    f32  m_SpringConstant; 
    f32  m_DampingFactor; 

    bool m_KeepOrientation; 

    REFLECT_DECLARE_CLASS(LooseAttachJointData, Reflect::Element); 
    static void EnumerateClass( Reflect::Compositor<LooseAttachJointData>& comp );
  }; 

  typedef Nocturnal::SmartPtr<LooseAttachJointData> LooseAttachJointDataPtr; 

}