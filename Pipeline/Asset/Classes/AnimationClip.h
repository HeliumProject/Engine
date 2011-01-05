#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetClass.h"

namespace Helium
{
    namespace Asset
    {
        class AnimationClipMode
        {
        public:
            enum Enum
            {
                Default,
                Once,
                Loop,
                PingPong
            };

            REFLECT_DECLARE_ENUMERATION( AnimationClipMode );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement( Default,  TXT( "Default" ) );
                info.AddElement( Once,     TXT( "Once" ) );
                info.AddElement( Loop,     TXT( "Loop" ) );
                info.AddElement( PingPong, TXT( "PingPong" ) );
            }
        };

        class PIPELINE_API AnimationClip : public AssetClass
        {
        private:
            bool                m_Compressed;
            uint32_t            m_SampleRate;
            AnimationClipMode   m_Mode;

        public:
            REFLECT_DECLARE_CLASS( AnimationClip, AssetClass );

            static void AcceptCompositeVisitor( Reflect::Composite& comp );

        public:
            AnimationClip()
                : m_Compressed( false )
                , m_SampleRate( 30 )
                , m_Mode( AnimationClipMode::Default )
            {
            }
        };

        typedef Helium::SmartPtr< AnimationClip > AnimationClipPtr;
        typedef std::vector< AnimationClipPtr > V_AnimationClip;
    }
}