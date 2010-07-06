#include "AnimationClip.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( AnimationClip );

void AnimationClip::EnumerateClass( Reflect::Compositor< AnimationClip >& comp )
{
    comp.GetComposite().m_UIName = TXT( "Animation Clip" );
    comp.GetComposite().SetProperty( AssetProperties::FileFilter, "*.fbx;" );

    comp.AddField( &AnimationClip::m_Compressed, "m_Compressed" );
    comp.AddField( &AnimationClip::m_SampleRate, "m_SampleRate" );
    comp.AddEnumerationField( &AnimationClip::m_Mode, "m_Mode" );
}
