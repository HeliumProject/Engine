#include "AnimationClip.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_ENUMERATION( AnimationClipMode );
REFLECT_DEFINE_CLASS( AnimationClip );

void AnimationClip::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.SetProperty( AssetProperties::FileFilter, "*.fbx;" );

    comp.AddField( &AnimationClip::m_Compressed, "m_Compressed" );
    comp.AddField( &AnimationClip::m_SampleRate, "m_SampleRate" );
    comp.AddEnumerationField( &AnimationClip::m_Mode, "m_Mode" );
}
