#include "Core/Content/Nodes/JointTransform.h"

using namespace Helium;
using namespace Helium::Content;
using namespace Helium::Component;

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
