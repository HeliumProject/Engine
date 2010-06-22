#include "ParametricKey.h"

using namespace Content;

REFLECT_DEFINE_ABSTRACT( ParametricKey )

void ParametricKey::EnumerateClass( Reflect::Compositor<ParametricKey>& comp )
{
  Reflect::Field* fieldParam = comp.AddField( &ParametricKey::m_Param, "m_Param" );
}


ParametricKey::ParametricKey()
: m_Param( 0.0f )
{
}

ParametricKey::~ParametricKey()
{
}

void ParametricKey::PostDeserialize()
{
  __super::PostDeserialize();
  Math::Clamp( m_Param, 0.0f, 1.0f );
}

void ParametricKey::PreSerialize()
{
  __super::PreSerialize();
  Math::Clamp( m_Param, 0.0f, 1.0f );
}