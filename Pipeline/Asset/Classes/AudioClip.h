#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetClass.h"

namespace Helium
{
    namespace Asset
    {
        class AudioClipMode
        {
        public:
            enum Enum
            {
                Default,
                Once,
                Loop,
                PingPong
            };

            REFLECT_DECLARE_ENUMERATION( AudioClipMode );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement( Default,  TXT( "Default" ) );
                info.AddElement( Once,     TXT( "Once" ) );
                info.AddElement( Loop,     TXT( "Loop" ) );
                info.AddElement( PingPong, TXT( "PingPong" ) );
            }
        };

        class HELIUM_PIPELINE_API AudioClip : public AssetClass
        {
        private:
            AudioClipMode     m_Mode;

        public:
            REFLECT_DECLARE_OBJECT( AudioClip, AssetClass );

            static void PopulateComposite( Reflect::Composite& comp );

        public:
            AudioClip()
                : m_Mode( AudioClipMode::Default )
            {
            }

            virtual void GatherSearchableProperties( Helium::SearchableProperties* properties ) const HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr< AudioClip > AudioClipPtr;
        typedef std::vector< AudioClipPtr > V_AudioClip;
    }
}