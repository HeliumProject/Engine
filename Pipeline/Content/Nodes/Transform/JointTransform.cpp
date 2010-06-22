#include "Pipeline/Content/Nodes/Transform/JointTransform.h"

using namespace Reflect;
using namespace Content;
using namespace Component;

REFLECT_DEFINE_CLASS(JointTransform)

void JointTransform::EnumerateClass( Reflect::Compositor<JointTransform>& comp )
{
  Reflect::Field* fieldSegmentScaleCompensate = comp.AddField( &JointTransform::m_SegmentScaleCompensate, "m_SegmentScaleCompensate" );
}

void JointTransform::ResetTransform()
{
  __super::ResetTransform();

  m_SegmentScaleCompensate = false;
}
