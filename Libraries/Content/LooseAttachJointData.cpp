#include "LooseAttachJointData.h" 

namespace Content
{
  REFLECT_DEFINE_CLASS(LooseAttachJointData); 

void LooseAttachJointData::EnumerateClass( Reflect::Compositor<LooseAttachJointData>& comp )
{
  Reflect::Field* fieldMass = comp.AddField( &LooseAttachJointData::m_Mass, "m_Mass" );
  Reflect::Field* fieldSolidAngleLimit = comp.AddField( &LooseAttachJointData::m_SolidAngleLimit, "m_SolidAngleLimit" );
  Reflect::Field* fieldSpringConstant = comp.AddField( &LooseAttachJointData::m_SpringConstant, "m_SpringConstant" );
  Reflect::Field* fieldDampingFactor = comp.AddField( &LooseAttachJointData::m_DampingFactor, "m_DampingFactor" );
  Reflect::Field* fieldKeepOrientation = comp.AddField( &LooseAttachJointData::m_KeepOrientation, "m_KeepOrientation" );

}


  LooseAttachJointData::LooseAttachJointData() 
    : m_Mass(1.0f)
    , m_SolidAngleLimit(15.0f)
    , m_SpringConstant(1.0f)
    , m_DampingFactor(0.1f)
    , m_KeepOrientation(false)
  {


  }

  LooseAttachJointData::~LooseAttachJointData()
  {


  }
}