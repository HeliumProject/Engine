#include "AudioClip.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( AudioClip );

void AudioClip::EnumerateClass( Reflect::Compositor< AudioClip >& comp )
{
    comp.GetComposite().m_UIName = TXT( "Audio Clip" );
    comp.GetComposite().SetProperty( AssetProperties::FileFilter, "*.wav;*.mp3;*.ogg" );

    comp.AddEnumerationField( &AudioClip::m_Mode, "m_Mode" );
}
