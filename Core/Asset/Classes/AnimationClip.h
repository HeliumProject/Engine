#pragma once

#include "Core/API.h"
#include "Core/Asset/AssetClass.h"

namespace Helium
{
    namespace Asset
    {
        namespace AnimationClipModes
        {
            enum AnimationClipMode
            {
                Default,
                Once,
                Loop,
                PingPong
            };
            static void AnimationClipModeEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement( Default,  TXT( "Default" ),  TXT( "Default" ) );
                info->AddElement( Once,     TXT( "Once" ),     TXT( "Once" ) );
                info->AddElement( Loop,     TXT( "Loop" ),     TXT( "Loop" ) );
                info->AddElement( PingPong, TXT( "PingPong" ), TXT( "PingPong" ) );
            }
        }
        typedef AnimationClipModes::AnimationClipMode AnimationClipMode;

        class CORE_API AnimationClip : public AssetClass
        {
        private:
            bool              m_Compressed;
            uint32_t               m_SampleRate;
            AnimationClipMode m_Mode;

        public:
            REFLECT_DECLARE_CLASS( AnimationClip, AssetClass );

            static void EnumerateClass( Reflect::Compositor< AnimationClip >& comp );

        public:
            AnimationClip()
                : m_Compressed( false )
                , m_SampleRate( 30 )
                , m_Mode( AnimationClipModes::Default )
            {
            }
        };

        typedef Helium::SmartPtr< AnimationClip > AnimationClipPtr;
        typedef std::vector< AnimationClipPtr > V_AnimationClip;
    }
}