#include "AudioClip.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS( AudioClip );

void AudioClip::EnumerateClass( Reflect::Compositor< AudioClip >& comp )
{
    comp.GetComposite().m_UIName = TXT( "Audio Clip" );
    comp.GetComposite().SetProperty( AssetProperties::FileFilter, "*.wav;*.mp3;*.ogg" );

    comp.AddEnumerationField( &AudioClip::m_Mode, "m_Mode" );
}

void AudioClip::GatherSearchableProperties( Helium::SearchableProperties* properties ) const
{
    const Reflect::Enumeration* modeEnum = Reflect::GetEnumeration< AudioClipMode >();
    if ( modeEnum )
    {
        tstring mode;
        if ( modeEnum->GetElementLabel( m_Mode, mode ) )
        {
            properties->Insert( modeEnum->m_Name, mode );
        }
    }

    __super::GatherSearchableProperties( properties );
}