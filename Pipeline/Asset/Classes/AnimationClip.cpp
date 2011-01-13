#include "AnimationClip.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_ENUMERATION( AnimationClipMode );
REFLECT_DEFINE_OBJECT( AnimationClip );

void AnimationClip::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.SetProperty( AssetProperties::FileFilter, TXT( "*.fbx;" ) );

    comp.AddField( &AnimationClip::m_Compressed, TXT( "m_Compressed" ) );
    comp.AddField( &AnimationClip::m_SampleRate, TXT( "m_SampleRate" ) );
    comp.AddEnumerationField( &AnimationClip::m_Mode, TXT( "m_Mode" ) );
}
