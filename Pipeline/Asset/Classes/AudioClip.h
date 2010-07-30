#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetClass.h"

namespace Helium
{
    namespace Asset
    {
        namespace AudioClipModes
        {
            enum AudioClipMode
            {
                Default,
                Once,
                Loop,
                PingPong
            };
            static void AudioClipModeEnumerateEnumeration( Reflect::Enumeration* info )
            {
                info->AddElement( Default,  TXT( "Default" ),  TXT( "Default" ) );
                info->AddElement( Once,     TXT( "Once" ),     TXT( "Once" ) );
                info->AddElement( Loop,     TXT( "Loop" ),     TXT( "Loop" ) );
                info->AddElement( PingPong, TXT( "PingPong" ), TXT( "PingPong" ) );
            }
        }
        typedef AudioClipModes::AudioClipMode AudioClipMode;

        class PIPELINE_API AudioClip : public AssetClass
        {
        private:
            AudioClipMode     m_Mode;

        public:
            REFLECT_DECLARE_ABSTRACT( AudioClip, AssetClass );

            static void EnumerateClass( Reflect::Compositor< AudioClip >& comp );

        public:
            AudioClip()
                : m_Mode( AudioClipModes::Default )
            {
            }

            virtual void GatherSearchableProperties( Helium::SearchableProperties* properties ) const HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr< AudioClip > AudioClipPtr;
        typedef std::vector< AudioClipPtr > V_AudioClip;
    }
}