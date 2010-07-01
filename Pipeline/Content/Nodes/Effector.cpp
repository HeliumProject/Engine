#include "Effector.h"

using namespace Content;

REFLECT_DEFINE_CLASS(Effector);

void Effector::EnumerateClass( Reflect::Compositor<Effector>& comp )
{
  Reflect::Field* fieldEffectorIndex = comp.AddField( &Effector::m_EffectorIndex, "m_EffectorIndex" );
}